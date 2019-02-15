#include "threadconf.h"
#ifdef ENABLE_THREADS
#include <factory/prelude.h>
#else
#define SIMPLE_THREAD_VAR
#endif
#include <iostream>
#include "kernel/mod2.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/links/silink.h"
#include "Singular/lists.h"
#include "Singular/blackbox.h"
#include "Singular/feOpt.h"
#include "Singular/libsingular.h"
#include <cstring>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <iterator>
#include <queue>
#include <assert.h>
#include "thread.h"
#include "lintree.h"

#include "singthreads.h"

using namespace std;

#ifdef ENABLE_THREADS
extern char *global_argv0;
#endif

namespace LibThread {

#ifdef ENABLE_THREADS
const int have_threads = 1;
#else
const int have_threads = 0;
#endif

class Command {
private:
  const char *name;
  const char *error;
  leftv result;
  leftv *args;
  int argc;
public:
  Command(const char *n, leftv r, leftv a)
  {
    name = n;
    result = r;
    error = NULL;
    argc = 0;
    for (leftv t = a; t != NULL; t = t->next) {
      argc++;
    }
    args = (leftv *) omAlloc0(sizeof(leftv) * argc);
    int i = 0;
    for (leftv t = a; t != NULL; t = t->next) {
      args[i++] = t;
    }
    result->rtyp = NONE;
    result->data = NULL;
  }
  ~Command() {
    omFree(args);
  }
  void check_argc(int n) {
    if (error) return;
    if (argc != n) error = "wrong number of arguments";
  }
  void check_argc(int lo, int hi) {
    if (error) return;
    if (argc < lo || argc > hi) error = "wrong number of arguments";
  }
  void check_argc_min(int n) {
    if (error) return;
    if (argc < n) error = "wrong number of arguments";
  }
  void check_arg(int i, int type, const char *err) {
    if (error) return;
    if (args[i]->Typ() != type) error = err;
  }
  void check_init(int i, const char *err) {
    if (error) return;
    leftv arg = args[i];
    if (arg->Data() == NULL || *(void **)(arg->Data()) == NULL)
      error = err;
  }
  void check_arg(int i, int type, int type2, const char *err) {
    if (error) return;
    if (args[i]->Typ() != type && args[i]->Typ() != type2) error = err;
  }
  int argtype(int i) {
    return args[i]->Typ();
  }
  int nargs() {
    return argc;
  }
  void *arg(int i) {
    return args[i]->Data();
  }
  template <typename T>
  T *shared_arg(int i) {
    return *(T **)(arg(i));
  }
  long int_arg(int i) {
    return (long)(args[i]->Data());
  }
  void report(const char *err) {
    error = err;
  }
  // intentionally not bool, so we can also do
  // q = p + test_arg(p, type);
  int test_arg(int i, int type) {
    if (i >= argc) return 0;
    return args[i]->Typ() == type;
  }
  void set_result(long n) {
    result->rtyp = INT_CMD;
    result->data = (char *)n;
  }
  void set_result(const char *s) {
    result->rtyp = STRING_CMD;
    result->data = omStrDup(s);
  }
  void set_result(int type, void *p) {
    result->rtyp = type;
    result->data = (char *) p;
  }
  void set_result(int type, long n) {
    result->rtyp = type;
    result->data = (char *) n;
  }
  void no_result() {
    result->rtyp = NONE;
  }
  bool ok() {
    return error == NULL;
  }
  BOOLEAN status() {
    if (error) {
      Werror("%s: %s", name, error);
    }
    return error != NULL;
  }
  BOOLEAN abort(const char *err) {
    report(err);
    return status();
  }
};

class SharedObject {
private:
  Lock lock;
  long refcount;
  int type;
  std::string name;
public:
  SharedObject(): lock(), refcount(0) { }
  virtual ~SharedObject() { }
  void set_type(int type_init) { type = type_init; }
  int get_type() { return type; }
  void set_name(std::string &name_init) { name = name_init; }
  void set_name(const char *s) {
    name = std::string(s);
  }
  std::string &get_name() { return name; }
  void incref(int by = 1) {
    lock.lock();
    refcount += 1;
    lock.unlock();
  }
  long decref() {
    int result;
    lock.lock();
    result = --refcount;
    lock.unlock();
    return result;
  }
  long getref() {
    return refcount;
  }
  virtual BOOLEAN op2(int op, leftv res, leftv a1, leftv a2) {
    return TRUE;
  }
  virtual BOOLEAN op3(int op, leftv res, leftv a1, leftv a2, leftv a3) {
    return TRUE;
  }
};

void acquireShared(SharedObject *obj) {
  obj->incref();
}

void releaseShared(SharedObject *obj) {
  if (obj->decref() == 0) {
    // delete obj;
  }
}

typedef std::map<std::string, SharedObject *> SharedObjectTable;

class Region : public SharedObject {
private:
  Lock region_lock;
public:
  SharedObjectTable objects;
  Region() : SharedObject(), region_lock(), objects() { }
  virtual ~Region() { }
  Lock *get_lock() { return &region_lock; }
  void lock() {
    if (!region_lock.is_locked())
      region_lock.lock();
  }
  void unlock() {
    if (region_lock.is_locked())
      region_lock.unlock();
  }
  int is_locked() {
    return region_lock.is_locked();
  }
};

Lock global_objects_lock;
SharedObjectTable global_objects;
Lock master_lock(true);
Lock name_lock(true);
SIMPLE_THREAD_VAR long thread_id;
long thread_counter;

int type_region;
int type_regionlock;
int type_channel;
int type_syncvar;
int type_atomic_table;
int type_shared_table;
int type_atomic_list;
int type_shared_list;
int type_thread;
int type_threadpool;
int type_job;
int type_trigger;

typedef SharedObject *SharedObjectPtr;
typedef SharedObjectPtr (*SharedConstructor)();

SharedObject *makeSharedObject(SharedObjectTable &table,
  Lock *lock, int type, string &name, SharedConstructor scons)
{
  int was_locked = lock->is_locked();
  SharedObject *result = NULL;
  if (!was_locked)
    lock->lock();
  if (table.count(name)) {
    result = table[name];
    if (result->get_type() != type)
      result = NULL;
  } else {
    result = scons();
    result->set_type(type);
    result->set_name(name);
    table.insert(pair<string,SharedObject *>(name, result));
  }
  if (!was_locked)
    lock->unlock();
  return result;
}

SharedObject *findSharedObject(SharedObjectTable &table,
  Lock *lock, string &name)
{
  int was_locked = lock->is_locked();
  SharedObject *result = NULL;
  if (!was_locked)
    lock->lock();
  if (table.count(name)) {
    result = table[name];
  }
  if (!was_locked)
    lock->unlock();
  return result;
}

class Transactional: public SharedObject {
private:
  Region *region;
  Lock *lock;
protected:
  int tx_begin() {
    if (!region)
      lock->lock();
    else {
      if (!lock->is_locked()) {
	return 0;
      }
    }
    return 1;
  }
  void tx_end() {
    if (!region)
      lock->unlock();
  }
public:
  Transactional() :
      SharedObject(), region(NULL), lock(NULL) {
  }
  void set_region(Region *region_init) {
    region = region_init;
    if (region_init) {
      lock = region_init->get_lock();
    } else {
      lock = new Lock();
    }
  }
  virtual ~Transactional() { if (!region && lock) delete lock; }
};

class TxTable: public Transactional {
private:
  std::map<string, string> entries;
public:
  TxTable() : Transactional(), entries() { }
  virtual ~TxTable() { }
  int put(string &key, string &value) {
    int result = 0;
    if (!tx_begin()) return -1;
    if (entries.count(key)) {
      entries[key] = value;
    } else {
      entries.insert(pair<string, string>(key, value));
      result = 1;
    }
    tx_end();
    return result;
  }
  int get(string &key, string &value) {
    int result = 0;
    if (!tx_begin()) return -1;
    if (entries.count(key)) {
      value = entries[key];
      result = 1;
    }
    tx_end();
    return result;
  }
  int check(string &key) {
    int result;
    if (!tx_begin()) return -1;
    result = entries.count(key);
    tx_end();
    return result;
  }
};

class TxList: public Transactional {
private:
  vector<string> entries;
public:
  TxList() : Transactional(), entries() { }
  virtual ~TxList() { }
  int put(size_t index, string &value) {
    int result = -1;
    if (!tx_begin()) return -1;
    if (index >= 1 && index <= entries.size()) {
      entries[index-1] = value;
      result = 1;
    } else {
      entries.resize(index+1);
      entries[index-1] = value;
      result = 0;
    }
    tx_end();
    return result;
  }
  int get(size_t index, string &value) {
    int result = 0;
    if (!tx_begin()) return -1;
    if (index >= 1 && index <= entries.size()) {
      result = (entries[index-1].size() != 0);
      if (result)
        value = entries[index-1];
    }
    tx_end();
    return result;
  }
  long size() {
    long result;
    if (!tx_begin()) return -1;
    result = (long) entries.size();
    tx_end();
    return result;
  }
};

class SingularChannel : public SharedObject {
private:
  queue<string> q;
  Lock lock;
  ConditionVariable cond;
public:
  SingularChannel(): SharedObject(), lock(), cond(&lock) { }
  virtual ~SingularChannel() { }
  void send(string item) {
    lock.lock();
    q.push(item);
    cond.signal();
    lock.unlock();
  }
  string receive() {
    lock.lock();
    while (q.empty()) {
      cond.wait();
    }
    string result = q.front();
    q.pop();
    if (!q.empty())
      cond.signal();
    lock.unlock();
    return result;
  }
  long count() {
    lock.lock();
    long result = q.size();
    lock.unlock();
    return result;
  }
};

class SingularSyncVar : public SharedObject {
private:
  string value;
  int init;
  Lock lock;
  ConditionVariable cond;
public:
  SingularSyncVar(): SharedObject(), init(0), lock(), cond(&lock) { }
  virtual ~SingularSyncVar() { }
  void acquire() {
    lock.lock();
  }
  void release() {
    lock.unlock();
  }
  void wait_init() {
    while (!init)
      cond.wait();
  }
  leftv get() {
    if (value.size() == 0) return NULL;
    return LinTree::from_string(value);
  }
  void update(leftv val) {
    value = LinTree::to_string(val);
    init = 1;
    cond.broadcast();
  }
  int write(string item) {
    int result = 0;
    lock.lock();
    if (!init) {
      value = item;
      init = 1;
      cond.broadcast();
      result = 1;
    }
    lock.unlock();
    return result;
  }
  string read() {
    lock.lock();
    while (!init)
      cond.wait();
    string result = value;
    lock.unlock();
    return result;
  }
  int check() {
    lock.lock();
    int result = init;
    lock.unlock();
    return result;
  }
};

void *shared_init(blackbox *b) {
  return omAlloc0(sizeof(SharedObject *));
}

void *new_shared(SharedObject *obj) {
  acquireShared(obj);
  void *result = omAlloc0(sizeof(SharedObject *));
  *(SharedObject **)result = obj;
  return result;
}

void shared_destroy(blackbox *b, void *d) {
  SharedObject *obj = *(SharedObject **)d;
  if (obj) {
    releaseShared(*(SharedObject **)d);
    *(SharedObject **)d = NULL;
  }
}

void rlock_destroy(blackbox *b, void *d) {
  SharedObject *obj = *(SharedObject **)d;
  ((Region *) obj)->unlock();
  if (obj) {
    releaseShared(*(SharedObject **)d);
    *(SharedObject **)d = NULL;
  }
}

void *shared_copy(blackbox *b, void *d) {
  SharedObject *obj = *(SharedObject **)d;
  void *result = shared_init(b);
  *(SharedObject **)result = obj;
  if (obj)
    acquireShared(obj);
  return result;
}

BOOLEAN shared_assign(leftv l, leftv r) {
  if (r->Typ() == l->Typ()) {
    if (l->rtyp == IDHDL) {
      omFree(IDDATA((idhdl)l->data));
      IDDATA((idhdl)l->data) = (char*)shared_copy(NULL,r->Data());
    } else {
      leftv ll=l->LData();
      if (ll==NULL)
      {
	return TRUE; // out of array bounds or similiar
      }
      if (ll->data) {
	shared_destroy(NULL, ll->data);
	omFree(ll->data);
      }
      ll->data = shared_copy(NULL,r->Data());
    }
  } else {
    Werror("assign %s(%d) = %s(%d)",
        Tok2Cmdname(l->Typ()),l->Typ(),Tok2Cmdname(r->Typ()),r->Typ());
    return TRUE;
  }
  return FALSE;
}

BOOLEAN rlock_assign(leftv l, leftv r) {
  if (r->Typ() == l->Typ()) {
    if (l->rtyp == IDHDL) {
      omFree(IDDATA((idhdl)l->data));
      IDDATA((idhdl)l->data) = (char*)shared_copy(NULL,r->Data());
    } else {
      leftv ll=l->LData();
      if (ll==NULL)
      {
	return TRUE; // out of array bounds or similiar
      }
      rlock_destroy(NULL, ll->data);
      omFree(ll->data);
      ll->data = shared_copy(NULL,r->Data());
    }
  } else {
    Werror("assign %s(%d) = %s(%d)",
        Tok2Cmdname(l->Typ()),l->Typ(),Tok2Cmdname(r->Typ()),r->Typ());
    return TRUE;
  }
  return FALSE;
}


BOOLEAN shared_check_assign(blackbox *b, leftv l, leftv r) {
  int lt = l->Typ();
  int rt = r->Typ();
  if (lt != DEF_CMD && lt != rt) {
    const char *rn=Tok2Cmdname(rt);
    const char *ln=Tok2Cmdname(lt);
    Werror("cannot assign %s (%d) to %s (%d)\n", rn, rt, ln, lt);
    return TRUE;
  }
  return FALSE;
}

BOOLEAN shared_op2(int op, leftv res, leftv a1, leftv a2) {
  SharedObject *obj = *(SharedObject **)a1->Data();
  return obj->op2(op, res, a1, a2);
}

BOOLEAN shared_op3(int op, leftv res, leftv a1, leftv a2, leftv a3) {
  SharedObject *obj = *(SharedObject **)a1->Data();
  return obj->op3(op, res, a1, a2, a3);
}

char *shared_string(blackbox *b, void *d) {
  char buf[80];
  SharedObject *obj = *(SharedObject **)d;
  if (!obj)
    return omStrDup("<uninitialized shared object>");
  int type = obj->get_type();
  string &name = obj->get_name();
  const char *type_name = "unknown";
  if (type == type_channel)
    type_name = "channel";
  else if (type == type_atomic_table)
    type_name = "atomic_table";
  else if (type == type_shared_table)
    type_name = "shared_table";
  else if (type == type_atomic_list)
    type_name = "atomic_list";
  else if (type == type_shared_list)
    type_name = "shared_list";
  else if (type == type_syncvar)
    type_name = "syncvar";
  else if (type == type_region)
    type_name = "region";
  else if (type == type_regionlock)
    type_name = "regionlock";
  else if (type == type_thread) {
    sprintf(buf, "<thread #%s>", name.c_str());
    return omStrDup(buf);
  }
  else if (type == type_threadpool) {
    if (name.size() > 0) {
      name_lock.lock();
      sprintf(buf, "<threadpool \"%.40s\"@%p>", name.c_str(), obj);
      name_lock.unlock();
    } else
      sprintf(buf, "<threadpool @%p>", obj);
    return omStrDup(buf);
  }
  else if (type == type_job) {
    if (name.size() > 0) {
      name_lock.lock();
      sprintf(buf, "<job \"%.40s\"@%p>", name.c_str(), obj);
      name_lock.unlock();
    } else
      sprintf(buf, "<job @%p>", obj);
    return omStrDup(buf);
  }
  else if (type == type_trigger) {
    if (name.size() > 0) {
      name_lock.lock();
      sprintf(buf, "<trigger \"%.40s\"@%p>", name.c_str(), obj);
      name_lock.unlock();
    } else
      sprintf(buf, "<trigger @%p>", obj);
    return omStrDup(buf);
  } else {
    sprintf(buf, "<unknown type %d>", type);
    return omStrDup(buf);
  }
  sprintf(buf, "<%s \"%.40s\">", type_name, name.c_str());
  return omStrDup(buf);
}

char *rlock_string(blackbox *b, void *d) {
  char buf[80];
  SharedObject *obj = *(SharedObject **)d;
  if (!obj)
    return omStrDup("<uninitialized region lock>");
  sprintf(buf, "<region lock \"%.40s\">", obj->get_name().c_str());
  return omStrDup(buf);
}

void report(const char *fmt, const char *name) {
  char buf[80];
  sprintf(buf, fmt, name);
  WerrorS(buf);
}

int wrong_num_args(const char *name, leftv arg, int n) {
  for (int i=1; i<=n; i++) {
    if (!arg) {
      report("%s: too few arguments", name);
      return TRUE;
    }
    arg = arg->next;
  }
  if (arg) {
    report("%s: too many arguments", name);
    return TRUE;
  }
  return FALSE;
}

int not_a_uri(const char *name, leftv arg) {
  if (arg->Typ() != STRING_CMD) {
    report("%s: not a valid URI", name);
    return TRUE;
  }
  return FALSE;
}

int not_a_region(const char *name, leftv arg) {
  if (arg->Typ() != type_region || !arg->Data()) {
    report("%s: not a region", name);
    return TRUE;
  }
  return FALSE;
}


char *str(leftv arg) {
  return (char *)(arg->Data());
}

SharedObject *consTable() {
  return new TxTable();
}

SharedObject *consList() {
  return new TxList();
}

SharedObject *consChannel() {
  return new SingularChannel();
}

SharedObject *consSyncVar() {
  return new SingularSyncVar();
}

SharedObject *consRegion() {
  return new Region();
}

static void appendArg(vector<leftv> &argv, string &s) {
  if (s.size() == 0) return;
  leftv val = LinTree::from_string(s);
  if (val->Typ() == NONE) {
    omFreeBin(val, sleftv_bin);
    return;
  }
  argv.push_back(val);
}

static void appendArg(vector<leftv> &argv, leftv arg) {
  argv.push_back(arg);
}

static void appendArgCopy(vector<leftv> &argv, leftv arg) {
  leftv val = (leftv) omAlloc0Bin(sleftv_bin);
  val->Copy(arg);
  argv.push_back(val);
}


static BOOLEAN executeProc(sleftv &result,
  const char *procname, const vector<leftv> &argv)
{
  leftv procnode = (leftv) omAlloc0Bin(sleftv_bin);
  procnode->name = omStrDup(procname);
  procnode->req_packhdl = basePack;
  int error = procnode->Eval();
  if (error) {
    Werror("procedure \"%s\" not found", procname);
    omFreeBin(procnode, sleftv_bin);
    return TRUE;
  }
  memset(&result, 0, sizeof(result));
  leftv *tail = &procnode->next;
  for (int i = 0; i < argv.size(); i++) {
    *tail = argv[i];
    tail = &(*tail)->next;
  }
  *tail = NULL;
  error = iiExprArithM(&result, procnode, '(');
  procnode->CleanUp();
  omFreeBin(procnode, sleftv_bin);
  if (error) {
    Werror("procedure call of \"%s\" failed", procname);
    return TRUE;
  }
  return FALSE;
}

BOOLEAN makeAtomicTable(leftv result, leftv arg) {
  if (wrong_num_args("makeAtomicTable", arg, 1))
    return TRUE;
  if (not_a_uri("makeAtomicTable", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = makeSharedObject(global_objects,
    &global_objects_lock, type_atomic_table, uri, consTable);
  ((TxTable *) obj)->set_region(NULL);
  result->rtyp = type_atomic_table;
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN makeAtomicList(leftv result, leftv arg) {
  if (wrong_num_args("makeAtomicList", arg, 1))
    return TRUE;
  if (not_a_uri("makeAtomicList", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = makeSharedObject(global_objects,
    &global_objects_lock, type_atomic_list, uri, consList);
  ((TxList *) obj)->set_region(NULL);
  result->rtyp = type_atomic_list;
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN makeSharedTable(leftv result, leftv arg) {
  if (wrong_num_args("makeSharedTable", arg, 2))
    return TRUE;
  if (not_a_region("makeSharedTable", arg))
    return TRUE;
  if (not_a_uri("makeSharedTable", arg->next))
    return TRUE;
  Region *region = *(Region **) arg->Data();
  fflush(stdout);
  string s = str(arg->next);
  SharedObject *obj = makeSharedObject(region->objects,
    region->get_lock(), type_shared_table, s, consTable);
  ((TxTable *) obj)->set_region(region);
  result->rtyp = type_shared_table;
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN makeSharedList(leftv result, leftv arg) {
  if (wrong_num_args("makeSharedList", arg, 2))
    return TRUE;
  if (not_a_region("makeSharedList", arg))
    return TRUE;
  if (not_a_uri("makeSharedList", arg->next))
    return TRUE;
  Region *region = *(Region **) arg->Data();
  string s = str(arg->next);
  SharedObject *obj = makeSharedObject(region->objects,
    region->get_lock(), type_shared_list, s, consList);
  ((TxList *) obj)->set_region(region);
  result->rtyp = type_shared_list;
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN makeChannel(leftv result, leftv arg) {
  if (wrong_num_args("makeChannel", arg, 1))
    return TRUE;
  if (not_a_uri("makeChannel", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = makeSharedObject(global_objects,
    &global_objects_lock, type_channel, uri, consChannel);
  result->rtyp = type_channel;
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN makeSyncVar(leftv result, leftv arg) {
  if (wrong_num_args("makeSyncVar", arg, 1))
    return TRUE;
  if (not_a_uri("makeSyncVar", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = makeSharedObject(global_objects,
    &global_objects_lock, type_syncvar, uri, consSyncVar);
  result->rtyp = type_syncvar;
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN makeRegion(leftv result, leftv arg) {
  if (wrong_num_args("makeRegion", arg, 1))
    return TRUE;
  if (not_a_uri("makeRegion", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = makeSharedObject(global_objects,
    &global_objects_lock, type_region, uri, consRegion);
  result->rtyp = type_region;
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN findSharedObject(leftv result, leftv arg) {
  if (wrong_num_args("findSharedObject", arg, 1))
    return TRUE;
  if (not_a_uri("findSharedObject", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = findSharedObject(global_objects,
    &global_objects_lock, uri);
  result->rtyp = INT_CMD;
  result->data = (char *)(long)(obj != NULL);
  return FALSE;
}

BOOLEAN typeSharedObject(leftv result, leftv arg) {
  if (wrong_num_args("findSharedObject", arg, 1))
    return TRUE;
  if (not_a_uri("findSharedObject", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = findSharedObject(global_objects,
    &global_objects_lock, uri);
  int type = obj ? obj->get_type() : -1;
  const char *type_name = "undefined";
  if (type == type_channel)
    type_name = "channel";
  else if (type == type_atomic_table)
    type_name = "atomic_table";
  else if (type == type_shared_table)
    type_name = "shared_table";
  else if (type == type_atomic_list)
    type_name = "atomic_list";
  else if (type == type_shared_list)
    type_name = "shared_list";
  else if (type == type_syncvar)
    type_name = "syncvar";
  else if (type == type_region)
    type_name = "region";
  else if (type == type_regionlock)
    type_name = "regionlock";
  result->rtyp = STRING_CMD;
  result->data = (char *)(omStrDup(type_name));
  return FALSE;
}

BOOLEAN bindSharedObject(leftv result, leftv arg) {
  if (wrong_num_args("bindSharedObject", arg, 1))
    return TRUE;
  if (not_a_uri("bindSharedObject", arg))
    return TRUE;
  string uri = str(arg);
  SharedObject *obj = findSharedObject(global_objects,
    &global_objects_lock, uri);
  if (!obj) {
    WerrorS("bindSharedObject: cannot find object");
    return TRUE;
  }
  result->rtyp = obj->get_type();
  result->data = new_shared(obj);
  return FALSE;
}

BOOLEAN getTable(leftv result, leftv arg) {
  if (wrong_num_args("getTable", arg, 2))
    return TRUE;
  if (arg->Typ() != type_atomic_table && arg->Typ() != type_shared_table) {
    WerrorS("getTable: not a valid table");
    return TRUE;
  }
  if (arg->next->Typ() != STRING_CMD) {
    WerrorS("getTable: not a valid table key");
    return TRUE;
  }
  TxTable *table = *(TxTable **) arg->Data();
  if (!table) {
    WerrorS("getTable: table has not been initialized");
    return TRUE;
  }
  string key = (char *)(arg->next->Data());
  string value;
  int success = table->get(key, value);
  if (success < 0) {
    WerrorS("getTable: region not acquired");
    return TRUE;
  }
  if (success == 0) {
    WerrorS("getTable: key not found");
    return TRUE;
  }
  leftv tmp = LinTree::from_string(value);
  result->rtyp = tmp->Typ();
  result->data = tmp->Data();
  return FALSE;
}

BOOLEAN inTable(leftv result, leftv arg) {
  if (wrong_num_args("inTable", arg, 2))
    return TRUE;
  if (arg->Typ() != type_atomic_table && arg->Typ() != type_shared_table) {
    WerrorS("inTable: not a valid table");
    return TRUE;
  }
  if (arg->next->Typ() != STRING_CMD) {
    WerrorS("inTable: not a valid table key");
    return TRUE;
  }
  TxTable *table = *(TxTable **) arg->Data();
  if (!table) {
    WerrorS("inTable: table has not been initialized");
    return TRUE;
  }
  string key = (char *)(arg->next->Data());
  int success = table->check(key);
  if (success < 0) {
    WerrorS("inTable: region not acquired");
    return TRUE;
  }
  result->rtyp = INT_CMD;
  result->data = (char *)(long)(success);
  return FALSE;
}

BOOLEAN putTable(leftv result, leftv arg) {
  if (wrong_num_args("putTable", arg, 3))
    return TRUE;
  if (arg->Typ() != type_atomic_table && arg->Typ() != type_shared_table) {
    WerrorS("putTable: not a valid table");
    return TRUE;
  }
  if (arg->next->Typ() != STRING_CMD) {
    WerrorS("putTable: not a valid table key");
    return TRUE;
  }
  TxTable *table = *(TxTable **) arg->Data();
  if (!table) {
    WerrorS("putTable: table has not been initialized");
    return TRUE;
  }
  string key = (char *)(arg->next->Data());
  string value = LinTree::to_string(arg->next->next);
  int success = table->put(key, value);
  if (success < 0) {
    WerrorS("putTable: region not acquired");
    return TRUE;
  }
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN getList(leftv result, leftv arg) {
  if (wrong_num_args("getList", arg, 2))
    return TRUE;
  if (arg->Typ() != type_atomic_list && arg->Typ() != type_shared_list) {
    WerrorS("getList: not a valid list (atomic or shared)");
    return TRUE;
  }
  if (arg->next->Typ() != INT_CMD) {
    WerrorS("getList: index must be an integer");
    return TRUE;
  }
  TxList *list = *(TxList **) arg->Data();
  if (!list) {
    WerrorS("getList: list has not been initialized");
    return TRUE;
  }
  long index = (long)(arg->next->Data());
  string value;
  int success = list->get(index, value);
  if (success < 0) {
    WerrorS("getList: region not acquired");
    return TRUE;
  }
  if (success == 0) {
    WerrorS("getList: no value at position");
    return TRUE;
  }
  leftv tmp = LinTree::from_string(value);
  result->rtyp = tmp->Typ();
  result->data = tmp->Data();
  return FALSE;
}

BOOLEAN putList(leftv result, leftv arg) {
  if (wrong_num_args("putList", arg, 3))
    return TRUE;
  if (arg->Typ() != type_atomic_list && arg->Typ() != type_shared_list) {
    WerrorS("putList: not a valid list (shared or atomic)");
    return TRUE;
  }
  if (arg->next->Typ() != INT_CMD) {
    WerrorS("putList: index must be an integer");
    return TRUE;
  }
  TxList *list = *(TxList **) arg->Data();
  if (!list) {
    WerrorS("putList: list has not been initialized");
    return TRUE;
  }
  long index = (long)(arg->next->Data());
  string value = LinTree::to_string(arg->next->next);
  int success = list->put(index, value);
  if (success < 0) {
    WerrorS("putList: region not acquired");
    return TRUE;
  }
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN lockRegion(leftv result, leftv arg) {
  if (wrong_num_args("lockRegion", arg, 1))
    return TRUE;
  if (not_a_region("lockRegion", arg))
    return TRUE;
  Region *region = *(Region **)arg->Data();
  if (region->is_locked()) {
    WerrorS("lockRegion: region is already locked");
    return TRUE;
  }
  region->lock();
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN regionLock(leftv result, leftv arg) {
  if (wrong_num_args("lockRegion", arg, 1))
    return TRUE;
  if (not_a_region("lockRegion", arg))
    return TRUE;
  Region *region = *(Region **)arg->Data();
  if (region->is_locked()) {
    WerrorS("lockRegion: region is already locked");
    return TRUE;
  }
  region->lock();
  result->rtyp = type_regionlock;
  result->data = new_shared(region);
  return FALSE;
}


BOOLEAN unlockRegion(leftv result, leftv arg) {
  if (wrong_num_args("unlockRegion", arg, 1))
    return TRUE;
  if (not_a_region("unlockRegion", arg))
    return TRUE;
  Region *region = *(Region **)arg->Data();
  if (!region->is_locked()) {
    WerrorS("unlockRegion: region is not locked");
    return TRUE;
  }
  region->unlock();
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN sendChannel(leftv result, leftv arg) {
  if (wrong_num_args("sendChannel", arg, 2))
    return TRUE;
  if (arg->Typ() != type_channel) {
    WerrorS("sendChannel: argument is not a channel");
    return TRUE;
  }
  SingularChannel *channel = *(SingularChannel **)arg->Data();
  if (!channel) {
    WerrorS("sendChannel: channel has not been initialized");
    return TRUE;
  }
  channel->send(LinTree::to_string(arg->next));
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN receiveChannel(leftv result, leftv arg) {
  if (wrong_num_args("receiveChannel", arg, 1))
    return TRUE;
  if (arg->Typ() != type_channel) {
    WerrorS("receiveChannel: argument is not a channel");
    return TRUE;
  }
  SingularChannel *channel = *(SingularChannel **)arg->Data();
  if (!channel) {
    WerrorS("receiveChannel: channel has not been initialized");
    return TRUE;
  }
  string item = channel->receive();
  leftv val = LinTree::from_string(item);
  result->rtyp = val->Typ();
  result->data = val->Data();
  return FALSE;
}

BOOLEAN statChannel(leftv result, leftv arg) {
  if (wrong_num_args("statChannel", arg, 1))
    return TRUE;
  if (arg->Typ() != type_channel) {
    WerrorS("statChannel: argument is not a channel");
    return TRUE;
  }
  SingularChannel *channel = *(SingularChannel **)arg->Data();
  if (!channel) {
    WerrorS("receiveChannel: channel has not been initialized");
    return TRUE;
  }
  long n = channel->count();
  result->rtyp = INT_CMD;
  result->data = (char *)n;
  return FALSE;
}

BOOLEAN writeSyncVar(leftv result, leftv arg) {
  if (wrong_num_args("writeSyncVar", arg, 2))
    return TRUE;
  if (arg->Typ() != type_syncvar) {
    WerrorS("writeSyncVar: argument is not a syncvar");
    return TRUE;
  }
  SingularSyncVar *syncvar = *(SingularSyncVar **)arg->Data();
  if (!syncvar) {
    WerrorS("writeSyncVar: syncvar has not been initialized");
    return TRUE;
  }
  if (!syncvar->write(LinTree::to_string(arg->next))) {
    WerrorS("writeSyncVar: variable already has a value");
    return TRUE;
  }
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN updateSyncVar(leftv result, leftv arg) {
  Command cmd("updateSyncVar", result, arg);
  cmd.check_argc_min(2);
  cmd.check_arg(0, type_syncvar, "first argument must be a syncvar");
  cmd.check_init(0, "syncvar has not been initialized");
  cmd.check_arg(1, STRING_CMD, "second argument must be a string");
  if (cmd.ok()) {
    SingularSyncVar *syncvar = cmd.shared_arg<SingularSyncVar>(0);
    char *procname = (char *) cmd.arg(1);
    arg = arg->next->next;
    syncvar->acquire();
    syncvar->wait_init();
    vector<leftv> argv;
    appendArg(argv, syncvar->get());
    while (arg) {
      appendArgCopy(argv, arg);
      arg = arg->next;
    }
    int error = executeProc(*result, procname, argv);
    if (!error) {
      syncvar->update(result);
    }
    syncvar->release();
    return error;
  }
  return cmd.status();
}


BOOLEAN readSyncVar(leftv result, leftv arg) {
  if (wrong_num_args("readSyncVar", arg, 1))
    return TRUE;
  if (arg->Typ() != type_syncvar) {
    WerrorS("readSyncVar: argument is not a syncvar");
    return TRUE;
  }
  SingularSyncVar *syncvar = *(SingularSyncVar **)arg->Data();
  if (!syncvar) {
    WerrorS("readSyncVar: syncvar has not been initialized");
    return TRUE;
  }
  string item = syncvar->read();
  leftv val = LinTree::from_string(item);
  result->rtyp = val->Typ();
  result->data = val->Data();
  return FALSE;
}

BOOLEAN statSyncVar(leftv result, leftv arg) {
  if (wrong_num_args("statSyncVar", arg, 1))
    return TRUE;
  if (arg->Typ() != type_syncvar) {
    WerrorS("statSyncVar: argument is not a syncvar");
    return TRUE;
  }
  SingularSyncVar *syncvar = *(SingularSyncVar **)arg->Data();
  if (!syncvar) {
    WerrorS("statSyncVar: syncvar has not been initialized");
    return TRUE;
  }
  int init = syncvar->check();
  result->rtyp = INT_CMD;
  result->data = (char *)(long) init;
  return FALSE;
}

void encode_shared(LinTree::LinTree &lintree, leftv val) {
  SharedObject *obj = *(SharedObject **)(val->Data());
  acquireShared(obj);
  lintree.put(obj);
}

leftv decode_shared(LinTree::LinTree &lintree) {
  int type = lintree.get_prev<int>();
  SharedObject *obj = lintree.get<SharedObject *>();
  leftv result = (leftv) omAlloc0Bin(sleftv_bin);
  result->rtyp = type;
  result->data = (void *)new_shared(obj);
  return result;
}

void ref_shared(LinTree::LinTree &lintree, int by) {
  SharedObject *obj = lintree.get<SharedObject *>();
  while (by > 0) {
    obj->incref();
    by--;
  }
  while (by < 0) {
    obj->decref();
    by++;
  }
}

void installShared(int type) {
  LinTree::install(type, encode_shared, decode_shared, ref_shared);
}

void makeSharedType(int &type, const char *name) {
  if (type != 0) return;
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  b->blackbox_Init = shared_init;
  b->blackbox_destroy = shared_destroy;
  b->blackbox_Copy = shared_copy;
  b->blackbox_String = shared_string;
  b->blackbox_Assign = shared_assign;
  b->blackbox_CheckAssign = shared_check_assign;
  // b->blackbox_Op2 = shared_op2;
  // b->blackbox_Op3 = shared_op3;
  type = setBlackboxStuff(b, name);
  installShared(type);
}

void makeRegionlockType(int &type, const char *name) {
  if (type != 0) return;
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  b->blackbox_Init = shared_init;
  b->blackbox_destroy = rlock_destroy;
  b->blackbox_Copy = shared_copy;
  b->blackbox_String = shared_string;
  b->blackbox_Assign = rlock_assign;
  b->blackbox_CheckAssign = shared_check_assign;
  type = setBlackboxStuff(b, name);
  installShared(type);
}

#define MAX_THREADS 128

class ThreadState {
public:
  bool active;
  bool running;
  int index;
  void *(*thread_func)(ThreadState *, void *);
  void *arg, *result;
  pthread_t id;
  pthread_t parent;
  Lock lock;
  ConditionVariable to_cond;
  ConditionVariable from_cond;
  queue<string> to_thread;
  queue<string> from_thread;
  ThreadState() : lock(), to_cond(&lock), from_cond(&lock),
                  to_thread(), from_thread() {
    active = false;
    running = false;
    index = -1;
  }
  ~ThreadState() {
    // We do nothing here. This is to prevent the condition
    // variable destructor from firing upon program exit,
    // which would invoke undefined behavior if the thread
    // is still running.
  }
};

Lock thread_lock;

ThreadState *thread_state;

void setOption(int ch) {
  int index = feGetOptIndex(ch);
  feSetOptValue((feOptIndex) index, (int) 1);
}

void thread_init() {
  master_lock.lock();
  thread_id = ++thread_counter;
  master_lock.unlock();
#ifdef ENABLE_THREADS
  onThreadInit();
  siInit(global_argv0);
#endif
  setOption('q');
  // setOption('b');
}

void *thread_main(void *arg) {
  ThreadState *ts = (ThreadState *)arg;
  thread_init();
  return ts->thread_func(ts, ts->arg);
}

void *interpreter_thread(ThreadState *ts, void *arg) {
  ts->lock.lock();
  for (;;) {
    bool eval = false;
    while (ts->to_thread.empty())
      ts->to_cond.wait();
    /* TODO */
    string expr = ts->to_thread.front();
    switch (expr[0]) {
      case '\0': case 'q':
        ts->lock.unlock();
	return NULL;
      case 'x':
        eval = false;
	break;
      case 'e':
        eval = true;
	break;
    }
    ts->to_thread.pop();
    expr = ts->to_thread.front();
    /* this will implicitly eval commands */
    leftv val = LinTree::from_string(expr);
    expr = LinTree::to_string(val);
    ts->to_thread.pop();
    if (eval)
      ts->from_thread.push(expr);
    ts->from_cond.signal();
  }
  ts->lock.unlock();
  return NULL;
}

class InterpreterThread : public SharedObject {
private:
  ThreadState *ts;
public:
  InterpreterThread(ThreadState *ts_init) : SharedObject(), ts(ts_init) { }
  virtual ~InterpreterThread() { }
  ThreadState *getThreadState() { return ts; }
  void clearThreadState() {
    ts = NULL;
  }
};

static ThreadState *newThread(void *(*thread_func)(ThreadState *, void *),
    void *arg, const char **error) {
  ThreadState *ts = NULL;
  if (error) *error = NULL;
  thread_lock.lock();
  for (int i=0; i<MAX_THREADS; i++) {
    if (!thread_state[i].active) {
      ts = thread_state + i;
      ts->index = i;
      ts->parent = pthread_self();
      ts->active = true;
      ts->running = true;
      ts->to_thread = queue<string>();
      ts->from_thread = queue<string>();
      ts->thread_func = thread_func;
      ts->arg = arg;
      ts->result = NULL;
      if (pthread_create(&ts->id, NULL, thread_main, ts)<0) {
	if (error)
	  *error = "createThread: internal error: failed to create thread";
	goto fail;
      }
      goto exit;
    }
  }
  if (error) *error = "createThread: too many threads";
  fail:
  ts = NULL;
  exit:
  thread_lock.unlock();
  return ts;
}

ThreadState *createThread(void *(*thread_func)(ThreadState *, void *),
    void *arg) {
  return newThread(thread_func, arg, NULL);
}

void *joinThread(ThreadState *ts) {
  void *result;
  pthread_join(ts->id, NULL);
  result = ts->result;
  thread_lock.lock();
  ts->running = false;
  ts->active = false;
  thread_lock.unlock();
}

static InterpreterThread *createInterpreterThread(const char **error) {
  ThreadState *ts = newThread(interpreter_thread, NULL, error);
  if (*error) return NULL;
  InterpreterThread *thread = new InterpreterThread(ts);
  char buf[10];
  sprintf(buf, "%d", ts->index);
  string name(buf);
  thread->set_name(name);
  thread->set_type(type_thread);
  return thread;
}

static BOOLEAN createThread(leftv result, leftv arg) {
  Command cmd("createThread", result, arg);
  cmd.check_argc(0);
  const char *error;
  if (!have_threads)
    cmd.report("thread support not available");
  if (!cmd.ok()) return cmd.status();
  InterpreterThread *thread = createInterpreterThread(&error);
  if (error) {
    return cmd.abort(error);
  }
  cmd.set_result(type_thread, new_shared(thread));
  return cmd.status();
}

static bool joinInterpreterThread(InterpreterThread *thread) {
  ThreadState *ts = thread->getThreadState();
  if (ts && ts->parent != pthread_self()) {
    return false;
  }
  ts->lock.lock();
  string quit("q");
  ts->to_thread.push(quit);
  ts->to_cond.signal();
  ts->lock.unlock();
  pthread_join(ts->id, NULL);
  thread_lock.lock();
  ts->running = false;
  ts->active = false;
  thread->clearThreadState();
  thread_lock.unlock();
  return true;
}

static BOOLEAN joinThread(leftv result, leftv arg) {
  if (wrong_num_args("joinThread", arg, 1))
    return TRUE;
  if (arg->Typ() != type_thread) {
    WerrorS("joinThread: argument is not a thread");
    return TRUE;
  }
  InterpreterThread *thread = *(InterpreterThread **)arg->Data();
  if (!joinInterpreterThread(thread)) {
    WerrorS("joinThread: can only be called from parent thread");
    return TRUE;
  }
  return FALSE;
}

class ThreadPool;
class Trigger;

class Job : public SharedObject {
public:
  ThreadPool *pool;
  long prio;
  size_t id;
  long pending_index;
  vector<Job *> deps;
  vector<Job *> notify;
  vector<Trigger *> triggers;
  vector<string> args;
  string result; // lintree-encoded
  void *data;
  bool fast;
  bool done;
  bool queued;
  bool running;
  bool cancelled;
  Job() : SharedObject(), pool(NULL), deps(), pending_index(-1), fast(false),
    done(false), running(false), queued(false), cancelled(false), data(NULL),
    result(), args(), notify(), triggers(), prio(0)
  { set_type(type_job); }
  ~Job();
  void addDep(Job *job) {
    deps.push_back(job);
  }
  void addDep(vector<Job *> &jobs);
  void addDep(long ndeps, Job **jobs);
  void addNotify(vector<Job *> &jobs);
  void addNotify(Job *job);
  virtual bool ready();
  virtual void execute() = 0;
  void run();
};

struct JobCompare {
  bool operator()(const Job* lhs, const Job* rhs) {
    if (lhs->fast < rhs->fast) {
      return true;
    }
    if (lhs->prio < rhs->prio) {
      return true;
    }
    if (lhs->prio == rhs->prio) {
      return lhs->id > rhs->id;
    }
    return false;
  }
};

class Trigger : public Job {
public:
  virtual bool accept(leftv arg) = 0;
  virtual void activate(leftv arg) = 0;
  Trigger() : Job() { set_type(type_trigger); fast = true; }
};

bool Job::ready() {
  vector<Job *>::iterator it;
  for (it = deps.begin(); it != deps.end(); it++) {
    if (!(*it)->done) return false;
  }
  return true;
}

Job::~Job() {
  vector<Job *>::iterator it;
  for (it = deps.begin(); it != deps.end(); it++) {
    releaseShared(*it);
  }
}

typedef queue<Job *> JobQueue;

class Scheduler;

struct SchedInfo {
  Scheduler *scheduler;
  Job *job;
  int num;
};

static SIMPLE_THREAD_VAR ThreadPool *currentThreadPoolRef;
static SIMPLE_THREAD_VAR Job *currentJobRef;

class ThreadPool : public SharedObject {
public:
  Scheduler *scheduler;
  int nthreads;
  ThreadPool(Scheduler *sched, int n);
  ThreadPool(int n);
  ~ThreadPool();
  ThreadState *getThread(int i);
  void shutdown(bool wait);
  void addThread(ThreadState *thread);
  void attachJob(Job *job);
  void detachJob(Job *job);
  void queueJob(Job *job);
  void broadcastJob(Job *job);
  void cancelDeps(Job * job);
  void cancelJob(Job *job);
  void waitJob(Job *job);
  void clearThreadState();
};


class Scheduler : public SharedObject {
private:
  bool single_threaded;
  size_t jobid;
  int nthreads;
  int maxconcurrency;
  int running;
  bool shutting_down;
  int shutdown_counter;
  vector<ThreadState *> threads;
  vector<ThreadPool *> thread_owners;
  priority_queue<Job *, vector<Job *>, JobCompare> global_queue;
  vector<JobQueue *> thread_queues;
  vector<Job *> pending;
  ConditionVariable cond;
  ConditionVariable response;
  friend class Job;
public:
  Lock lock;
  Scheduler(int n) :
    SharedObject(), threads(), thread_owners(), global_queue(), thread_queues(),
    single_threaded(n==0), nthreads(n == 0 ? 1 : n),
    lock(true), cond(&lock), response(&lock),
    shutting_down(false), shutdown_counter(0), jobid(0),
    maxconcurrency(n), running(0)
  {
    thread_queues.push_back(new JobQueue());
  }
  void set_maxconcurrency(int n) {
    maxconcurrency = n;
  }
  int get_maxconcurrency() {
    return maxconcurrency;
  }
  int threadpool_size(ThreadPool *pool) {
    int n;
    for (int i = 0; i <thread_owners.size(); i++) {
      if (thread_owners[i] == pool)
        n++;
    }
    return n;
  }
  virtual ~Scheduler() {
    for (int i = 0; i < thread_queues.size(); i++) {
      JobQueue *q = thread_queues[i];
      while (!q->empty()) {
        Job *job = q->front();
	q->pop();
	releaseShared(job);
      }
    }
    thread_queues.clear();
    threads.clear();
  }
  ThreadState *getThread(int i) { return threads[i]; }
  void shutdown(bool wait) {
    if (single_threaded) {
      SchedInfo *info = new SchedInfo();
      info->num = 0;
      info->scheduler = this;
      acquireShared(this);
      info->job = NULL;
      Scheduler::main(NULL, info);
      return;
    }
    lock.lock();
    if (wait) {
      while (!global_queue.empty()) {
        response.wait();
      }
    }
    shutting_down = true;
    while (shutdown_counter < nthreads) {
      cond.broadcast();
      response.wait();
    }
    lock.unlock();
    for (int i = 0; i <threads.size(); i++) {
      joinThread(threads[i]);
    }
  }
  void addThread(ThreadPool *owner, ThreadState *thread) {
    lock.lock();
    thread_owners.push_back(owner);
    threads.push_back(thread);
    thread_queues.push_back(new JobQueue());
    lock.unlock();
  }
  void attachJob(ThreadPool *pool, Job *job) {
    lock.lock();
    job->pool = pool;
    job->id = jobid++;
    acquireShared(job);
    if (job->ready()) {
      global_queue.push(job);
      cond.signal();
    }
    else if (job->pending_index < 0) {
      job->pool = pool;
      job->pending_index = pending.size();
      pending.push_back(job);
    }
    lock.unlock();
  }
  void detachJob(Job *job) {
    lock.lock();
    long i = job->pending_index;
    job->pending_index = -1;
    if (i >= 0) {
      job = pending.back();
      pending.resize(pending.size()-1);
      pending[i] = job;
      job->pending_index = i;
    }
    lock.unlock();
  }
  void queueJob(Job *job) {
    lock.lock();
    global_queue.push(job);
    cond.signal();
    lock.unlock();
  }
  void broadcastJob(ThreadPool *pool, Job *job) {
    lock.lock();
    for (int i = 0; i <thread_queues.size(); i++) {
      if (thread_owners[i] == pool) {
	acquireShared(job);
	thread_queues[i]->push(job);
      }
    }
    lock.unlock();
  }
  void cancelDeps(Job * job) {
    vector<Job *> &notify = job->notify;
    for (int i = 0; i <notify.size(); i++) {
      Job *next = notify[i];
      if (!next->cancelled) {
        cancelJob(next);
      }
    }
  }
  void cancelJob(Job *job) {
    lock.lock();
    if (!job->cancelled) {
      job->cancelled = true;
      if (!job->running && !job->done) {
        job->done = true;
	cancelDeps(job);
      }
    }
    lock.unlock();
  }
  void waitJob(Job *job) {
    if (single_threaded) {
      SchedInfo *info = new SchedInfo();
      info->num = 0;
      info->scheduler = this;
      acquireShared(this);
      info->job = job;
      Scheduler::main(NULL, info);
    } else {
      lock.lock();
      for (;;) {
	if (job->done || job->cancelled) {
	  break;
	}
	response.wait();
      }
      response.signal(); // forward signal
      lock.unlock();
    }
  }
  void clearThreadState() {
    threads.clear();
  }
  static void notifyDeps(Scheduler *scheduler, Job *job) {
    vector<Job *> &notify = job->notify;
    job->incref(notify.size());
    for (int i = 0; i <notify.size(); i++) {
      Job *next = notify[i];
      if (!next->queued && next->ready() && !next->cancelled) {
        next->queued = true;
        scheduler->queueJob(next);
      }
    }
    vector<Trigger *> &triggers = job->triggers;
    leftv arg = NULL;
    if (triggers.size() > 0 && job->result.size() > 0)
      arg = LinTree::from_string(job->result);
    for (int i = 0; i < triggers.size(); i++) {
      Trigger *trigger = triggers[i];
      if (trigger->accept(arg)) {
        trigger->activate(arg);
	if (trigger->ready())
	   scheduler->queueJob(trigger);
      }
    }
    if (arg) {
      arg->CleanUp();
      omFreeBin(arg, sleftv_bin);
    }
  }
  static void *main(ThreadState *ts, void *arg) {
    SchedInfo *info = (SchedInfo *) arg;
    Scheduler *scheduler = info->scheduler;
    ThreadPool *oldThreadPool = currentThreadPoolRef;
    // TODO: set current thread pool
    // currentThreadPoolRef = pool;
    Lock &lock = scheduler->lock;
    ConditionVariable &cond = scheduler->cond;
    ConditionVariable &response = scheduler->response;
    JobQueue *my_queue = scheduler->thread_queues[info->num];
    if (!scheduler->single_threaded)
      thread_init();
    lock.lock();
    for (;;) {
      if (info->job && info->job->done)
        break;
      if (scheduler->shutting_down) {
        scheduler->shutdown_counter++;
        scheduler->response.signal();
	break;
      }
      if (!my_queue->empty()) {
       Job *job = my_queue->front();
       my_queue->pop();
       if (!scheduler->global_queue.empty())
         cond.signal();
       currentJobRef = job;
       job->run();
       currentJobRef = NULL;
       notifyDeps(scheduler, job);
       releaseShared(job);
       scheduler->response.signal();
       continue;
      } else if (!scheduler->global_queue.empty()) {
       Job *job = scheduler->global_queue.top();
       scheduler->global_queue.pop();
       if (!scheduler->global_queue.empty())
         cond.signal();
       currentJobRef = job;
       job->run();
       currentJobRef = NULL;
       notifyDeps(scheduler, job);
       releaseShared(job);
       scheduler->response.signal();
       continue;
      } else {
        if (scheduler->single_threaded) {
          break;
        }
        cond.wait();
      }
    }
    // TODO: correct current thread pool
    // releaseShared(currentThreadPoolRef);
    currentThreadPoolRef = oldThreadPool;
    scheduler->lock.unlock();
    delete info;
    return NULL;
  }
};

ThreadPool::ThreadPool(int n) : SharedObject(), nthreads(n) {
  scheduler = new Scheduler(n);
  acquireShared(scheduler);
}
ThreadPool::ThreadPool(Scheduler *sched, int n) : SharedObject(), nthreads(n) {
  scheduler = sched;
  acquireShared(sched);
}
ThreadPool::~ThreadPool() {
  releaseShared(scheduler);
}
ThreadState *ThreadPool::getThread(int i) { return scheduler->getThread(i); }
void ThreadPool::shutdown(bool wait) { scheduler->shutdown(wait); }
void ThreadPool::addThread(ThreadState *thread) {
  scheduler->addThread(this, thread);
}
void ThreadPool::attachJob(Job *job) {
  scheduler->attachJob(this, job);
}
void ThreadPool::detachJob(Job *job) {
  scheduler->detachJob(job);
}
void ThreadPool::queueJob(Job *job) {
  scheduler->queueJob(job);
}
void ThreadPool::broadcastJob(Job *job) {
  scheduler->broadcastJob(this, job);
}
void ThreadPool::cancelDeps(Job * job) {
  scheduler->cancelDeps(job);
}
void ThreadPool::cancelJob(Job *job) {
  scheduler->cancelJob(job);
}
void ThreadPool::waitJob(Job *job) {
  scheduler->waitJob(job);
}
void ThreadPool::clearThreadState() {
  scheduler->clearThreadState();
}

void Job::addDep(vector<Job *> &jobs) {
  deps.insert(deps.end(), jobs.begin(), jobs.end());
}

void Job::addDep(long ndeps, Job **jobs) {
  for (long i = 0; i < ndeps; i++) {
    deps.push_back(jobs[i]);
  }
}

void Job::addNotify(vector<Job *> &jobs) {
  notify.insert(notify.end(), jobs.begin(), jobs.end());
  if (done) {
    Scheduler::notifyDeps(pool->scheduler, this);
  }
}

void Job::addNotify(Job *job) {
  notify.push_back(job);
  if (done) {
    Scheduler::notifyDeps(pool->scheduler, this);
  }
}

void Job::run() {
  if (!cancelled) {
    running = true;
    pool->scheduler->lock.unlock();
    pool->scheduler->running++;
    execute();
    pool->scheduler->running--;
    pool->scheduler->lock.lock();
    running = false;
  }
  done = true;
}

class AccTrigger : public Trigger {
private:
  long count;
public:
  AccTrigger(long count_init): Trigger(), count(count_init) {
  }
  virtual bool ready() {
    if (!Trigger::ready()) return false;
    return args.size() >= count;
  }
  virtual bool accept(leftv arg) {
    return true;
  }
  virtual void activate(leftv arg) {
    while (arg != NULL && !ready()) {
      args.push_back(LinTree::to_string(arg));
      if (ready()) {
	return;
      }
      arg = arg->next;
    }
  }
  virtual void execute() {
    lists l = (lists) omAlloc0Bin(slists_bin);
    l->Init(args.size());
    for (int i = 0; i < args.size(); i++) {
      leftv val = LinTree::from_string(args[i]);
      memcpy(&l->m[i], val, sizeof(*val));
      omFreeBin(val, sleftv_bin);
    }
    sleftv val;
    memset(&val, 0, sizeof(val));
    val.rtyp = LIST_CMD;
    val.data = l;
    result = LinTree::to_string(&val);
    // val.CleanUp();
  }
};

class CountTrigger : public Trigger {
private:
  long count;
public:
  CountTrigger(long count_init): Trigger(), count(count_init) {
  }
  virtual bool ready() {
    if (!Trigger::ready()) return false;
    return count <= 0;
  }
  virtual bool accept(leftv arg) {
    return arg == NULL;
  }
  virtual void activate(leftv arg) {
    if (!ready()) {
      count--;
    }
  }
  virtual void execute() {
    // do nothing
  }
};

class SetTrigger : public Trigger {
private:
  vector<bool> set;
  long count;
public:
  SetTrigger(long count_init) : Trigger(), count(0),
    set(count_init) {
  }
  virtual bool ready() {
    if (!Trigger::ready()) return false;
    return count == set.size();
  }
  virtual bool accept(leftv arg) {
    return arg->Typ() == INT_CMD;
  }
  virtual void activate(leftv arg) {
    if (!ready()) {
      long value = (long) arg->Data();
      if (value < 0 || value >= count) return;
      if (set[value]) return;
      set[value] = true;
      count++;
    }
  }
  virtual void execute() {
    // do nothing
  }
};


class ProcTrigger : public Trigger {
private:
  string procname;
  bool success;
public:
  ProcTrigger(const char *p) : Trigger(), procname(p), success(false) {
  }
  virtual bool ready() {
    if (!Trigger::ready()) return false;
    return success;
  }
  virtual bool accept(leftv arg) {
    return TRUE;
  }
  virtual void activate(leftv arg) {
    if (!ready()) {
      pool->scheduler->lock.unlock();
      vector<leftv> argv;
      for (int i = 0; i < args.size(); i++) {
        appendArg(argv, args[i]);
      }
      int error = false;
      while (arg) {
        appendArgCopy(argv, arg);
	arg = arg->next;
      }
      sleftv val;
      if (!error)
	error = executeProc(val, procname.c_str(), argv);
      if (!error) {
        if (val.Typ() == NONE || (val.Typ() == INT_CMD &&
	                          (long) val.Data()))
	{
	  success = true;
	}
        val.CleanUp();
      }
      pool->scheduler->lock.lock();
    }
  }
  virtual void execute() {
    // do nothing
  }
};

static BOOLEAN createThreadPool(leftv result, leftv arg) {
  long n;
  Command cmd("createThreadPool", result, arg);
  cmd.check_argc(1, 2);
  cmd.check_arg(0, INT_CMD, "first argument must be an integer");
  if (cmd.ok()) {
    n = (long) cmd.arg(0);
    if (n < 0) cmd.report("number of threads must be non-negative");
    else if (n >= 256) cmd.report("number of threads too large");
    if (!have_threads && n != 0)
      cmd.report("in single-threaded mode, number of threads must be zero");
  }
  if (cmd.ok()) {
    ThreadPool *pool = new ThreadPool((int) n);
    pool->set_type(type_threadpool);
    for (int i = 0; i <n; i++) {
      const char *error;
      SchedInfo *info = new SchedInfo();
      info->scheduler = pool->scheduler;
      acquireShared(pool->scheduler);
      info->job = NULL;
      info->num = i;
      ThreadState *thread = newThread(Scheduler::main, info, &error);
      if (!thread) {
        // TODO: clean up bad pool
        return cmd.abort(error);
      }
      pool->addThread(thread);
    }
    cmd.set_result(type_threadpool, new_shared(pool));
  }
  return cmd.status();
}

static BOOLEAN createThreadPoolSet(leftv result, leftv arg) {
  Command cmd("createThreadPoolSet", result, arg);
  cmd.check_argc(2);
  cmd.check_arg(0, INT_CMD, "first argument must be an integer");
  cmd.check_arg(1, LIST_CMD, "second argument must be a list of integers");
  lists l;
  int n;
  if (cmd.ok()) {
    l = (lists) (cmd.arg(1));
    n = lSize(l)+1;
    if (n == 0)
      return cmd.abort("second argument must not be empty");
    for (int i = 0; i < n; i++) {
      if (l->m[i].Typ() != INT_CMD)
        return cmd.abort("second argument must be a list of integers");
    }
  }
  lists pools = (lists) omAlloc0Bin(slists_bin);
  pools->Init(n);
  if (cmd.ok()) {
    long s = 0;
    for (int i = 0; i < n; i++) {
      s += (long) (l->m[i].Data());
    }
    Scheduler *sched = new Scheduler((int)s);
    sched->set_maxconcurrency(cmd.int_arg(0));
    for (int i = 0; i < n; i++) {
      long m = (long) (l->m[i].Data());
      ThreadPool *pool = new ThreadPool(sched, (int) m);
      pool->set_type(type_threadpool);
      for (int j = 0; j < m; j++) {
	const char *error;
	SchedInfo *info = new SchedInfo();
	info->scheduler = pool->scheduler;
	acquireShared(pool->scheduler);
	info->job = NULL;
	info->num = i;
	ThreadState *thread = newThread(Scheduler::main, info, &error);
	if (!thread) {
	  // TODO: clean up bad pool
	  return cmd.abort(error);
	}
	pool->addThread(thread);
      }
      pools->m[i].rtyp = type_threadpool;
      pools->m[i].data = new_shared(pool);
    }
    cmd.set_result(LIST_CMD, pools);
  }
  return cmd.status();
}

ThreadPool *createThreadPool(int nthreads, int prioThreads = 0) {
  ThreadPool *pool = new ThreadPool((int) nthreads);
  pool->set_type(type_threadpool);
  for (int i = 0; i <nthreads; i++) {
    const char *error;
    SchedInfo *info = new SchedInfo();
    info->scheduler = pool->scheduler;
    acquireShared(pool);
    info->job = NULL;
    info->num = i;
    ThreadState *thread = newThread(Scheduler::main, info, &error);
    if (!thread) {
      return NULL;
    }
    pool->addThread(thread);
  }
  return pool;
}

void release(ThreadPool *pool) {
  releaseShared(pool);
}

void retain(ThreadPool *pool) {
  acquireShared(pool);
}

ThreadPool *getCurrentThreadPool() {
  return currentThreadPoolRef;
}

static BOOLEAN getThreadPoolWorkers(leftv result, leftv arg) {
  Command cmd("getThreadPoolWorkers", result, arg);
  cmd.check_argc(1);
  cmd.check_arg(0, type_threadpool, "argument must be a threadpool");
  cmd.check_init(0, "threadpool not initialized");
  int r = 0;
  if (cmd.ok()) {
    ThreadPool *pool = cmd.shared_arg<ThreadPool>(0);
    Scheduler *sched = pool->scheduler;
    sched->lock.lock();
    r = sched->threadpool_size(pool);
    sched->lock.unlock();
    cmd.set_result(INT_CMD, r);
  }
  return cmd.status();
}

static BOOLEAN setThreadPoolWorkers(leftv result, leftv arg) {
  Command cmd("setThreadPoolWorkers", result, arg);
  cmd.check_argc(2);
  cmd.check_arg(0, type_threadpool, "first argument must be a threadpool");
  cmd.check_arg(1, INT_CMD, "second argument must be an integer");
  cmd.check_init(0, "threadpool not initialized");
  if (cmd.ok()) {
    ThreadPool *pool = cmd.shared_arg<ThreadPool>(0);
    Scheduler *sched = pool->scheduler;
    // TODO: count/add threads
    cmd.no_result();
  }
  return cmd.status();
}

static BOOLEAN getThreadPoolConcurrency(leftv result, leftv arg) {
  Command cmd("getThreadPoolConcurrency", result, arg);
  cmd.check_argc(1);
  cmd.check_arg(0, type_threadpool, "argument must be a threadpool");
  cmd.check_init(0, "threadpool not initialized");
  if (cmd.ok()) {
    ThreadPool *pool = cmd.shared_arg<ThreadPool>(0);
    Scheduler *sched = pool->scheduler;
    sched->lock.lock();
    cmd.set_result(INT_CMD, sched->get_maxconcurrency());
    sched->lock.unlock();
  }
  return cmd.status();
}

static BOOLEAN setThreadPoolConcurrency(leftv result, leftv arg) {
  Command cmd("setThreadPoolWorkers", result, arg);
  cmd.check_argc(2);
  cmd.check_arg(0, type_threadpool, "first argument must be a threadpool");
  cmd.check_arg(1, INT_CMD, "second argument must be an integer");
  cmd.check_init(0, "threadpool not initialized");
  if (cmd.ok()) {
    ThreadPool *pool = cmd.shared_arg<ThreadPool>(0);
    Scheduler *sched = pool->scheduler;
    sched->lock.lock();
    sched->set_maxconcurrency(cmd.int_arg(1));
    sched->lock.unlock();
    cmd.no_result();
  }
  return cmd.status();
}

static BOOLEAN closeThreadPool(leftv result, leftv arg) {
  Command cmd("closeThreadPool", result, arg);
  cmd.check_argc(1, 2);
  cmd.check_arg(0, type_threadpool, "first argument must be a threadpool");
  cmd.check_init(0, "threadpool not initialized");
  if (cmd.nargs() > 1)
    cmd.check_arg(1, INT_CMD, "optional argument must be an integer");
  if (cmd.ok()) {
    ThreadPool *pool = *(ThreadPool **)(cmd.arg(0));
    bool wait = cmd.nargs() == 2 ? (cmd.int_arg(1) != 0) : 1;
    pool->shutdown(wait);
    cmd.no_result();
  }
  return cmd.status();
}

void closeThreadPool(ThreadPool *pool, bool wait) {
  pool->shutdown(wait);
}


BOOLEAN currentThreadPool(leftv result, leftv arg) {
  Command cmd("currentThreadPool", result, arg);
  cmd.check_argc(0);
  ThreadPool *pool = currentThreadPoolRef;
  if (pool) {
    cmd.set_result(type_threadpool, new_shared(pool));
  } else {
    cmd.report("no current threadpool");
  }
  return cmd.status();
}

BOOLEAN setCurrentThreadPool(leftv result, leftv arg) {
  Command cmd("setCurrentThreadPool", result, arg);
  cmd.check_argc(1);
  cmd.check_init(0, "threadpool not initialized");
  if (cmd.ok()) {
    ThreadPool *pool = *(ThreadPool **)(cmd.arg(0));
    acquireShared(pool);
    if (currentThreadPoolRef)
      releaseShared(currentThreadPoolRef);
    currentThreadPoolRef = pool;
  }
  return cmd.status();
}

class EvalJob : public Job {
public:
  EvalJob() : Job() { }
  virtual void execute() {
    leftv val = LinTree::from_string(args[0]);
    result = (LinTree::to_string(val));
    val->CleanUp();
    omFreeBin(val, sleftv_bin);
  }
};

class ExecJob : public Job {
public:
  ExecJob() : Job() { }
  virtual void execute() {
    leftv val = LinTree::from_string(args[0]);
    val->CleanUp();
    omFreeBin(val, sleftv_bin);
  }
};

class ProcJob : public Job {
  string procname;
public:
  ProcJob(const char *procname_init) : Job(),
    procname(procname_init) {
    set_name(procname_init);
  }
  virtual void execute() {
    vector<leftv> argv;
    for (int i = 0; i <args.size(); i++) {
      appendArg(argv, args[i]);
    }
    for (int i = 0; i < deps.size(); i++) {
      appendArg(argv, deps[i]->result);
    }
    sleftv val;
    int error = executeProc(val, procname.c_str(), argv);
    if (!error) {
      result = (LinTree::to_string(&val));
      val.CleanUp();
    }
  }
};

class KernelJob : public Job {
private:
  void (*cfunc)(leftv result, leftv arg);
public:
  KernelJob(void (*func)(leftv result, leftv arg)) : cfunc(func) { }
  virtual void execute() {
    vector<leftv> argv;
    for (int i = 0; i <args.size(); i++) {
      appendArg(argv, args[i]);
    }
    for (int i = 0; i < deps.size(); i++) {
      appendArg(argv, deps[i]->result);
    }
    sleftv val;
    memset(&val, 0, sizeof(val));
    if (argv.size() > 0) {
      leftv *tail = &argv[0]->next;
      for (int i = 1; i < argv.size(); i++) {
	*tail = argv[i];
	tail = &(*tail)->next;
      }
      *tail = NULL;
    }
    cfunc(&val, argv[0]);
    result = (LinTree::to_string(&val));
    val.CleanUp();
  }
};

class RawKernelJob : public Job {
private:
  void (*cfunc)(long ndeps, Job **deps);
public:
  RawKernelJob(void (*func)(long ndeps, Job **deps)) : cfunc(func) { }
  virtual void execute() {
    long ndeps = deps.size();
    Job **jobs = (Job **) omAlloc0(sizeof(Job *) * ndeps);
    for (long i = 0; i < ndeps; i++)
      jobs[i] = deps[i];
    cfunc(ndeps, jobs);
    omFree(jobs);
  }
};

static BOOLEAN createJob(leftv result, leftv arg) {
  Command cmd("createJob", result, arg);
  cmd.check_argc_min(1);
  cmd.check_arg(0, STRING_CMD, COMMAND,
    "job name must be a string or quote expression");
  if (cmd.ok()) {
    if (cmd.test_arg(0, STRING_CMD)) {
      ProcJob *job = new ProcJob((char *)(cmd.arg(0)));
      for (leftv a = arg->next; a != NULL; a = a->next) {
        job->args.push_back(LinTree::to_string(a));
      }
      cmd.set_result(type_job, new_shared(job));
    } else {
      cmd.check_argc(1);
      Job *job = new EvalJob();
      job->args.push_back(LinTree::to_string(arg));
      cmd.set_result(type_job, new_shared(job));
    }
  }
  return cmd.status();
}

Job *createJob(void (*func)(leftv result, leftv arg)) {
  KernelJob *job = new KernelJob(func);
  return job;
}

Job *createJob(void (*func)(long ndeps, Job **deps)) {
  RawKernelJob *job = new RawKernelJob(func);
  return job;
}

Job *startJob(ThreadPool *pool, Job *job, leftv arg) {
  if (job->pool) return NULL;
  while (arg) {
    job->args.push_back(LinTree::to_string(arg));
    arg = arg->next;
  }
  pool->attachJob(job);
  return job;
}

Job *startJob(ThreadPool *pool, Job *job) {
  return startJob(pool, job, NULL);
}

Job *scheduleJob(ThreadPool *pool, Job *job, long ndeps, Job **deps) {
  if (job->pool) return NULL;
  pool->scheduler->lock.lock();
  bool cancelled = false;
  job->addDep(ndeps, deps);
  for (long i = 0; i < ndeps; i++) {
    deps[i]->addNotify(job);
    cancelled |= deps[i]->cancelled;
  }
  if (cancelled) {
    job->pool = pool;
    pool->cancelJob(job);
  }
  else
    pool->attachJob(job);
  pool->scheduler->lock.unlock();
}

void cancelJob(Job *job) {
  ThreadPool *pool = job->pool;
  if (pool) pool->cancelJob(job);
}

Job *getCurrentJob() {
  return currentJobRef;
}

static BOOLEAN startJob(leftv result, leftv arg) {
  Command cmd("startJob", result, arg);
  cmd.check_argc_min(1);
  int has_pool = cmd.test_arg(0, type_threadpool);
  cmd.check_argc_min(1+has_pool);
  if (has_pool)
    cmd.check_init(0, "threadpool not initialized");
  int has_prio = cmd.test_arg(has_pool, INT_CMD);
  long prio = has_prio ? (long) cmd.arg(has_pool) : 0L;
  int first_arg = has_pool + has_prio;
  cmd.check_arg(first_arg, type_job, STRING_CMD,
    "job argument must be a job or string");
  if (cmd.ok() && cmd.argtype(first_arg) == type_job)
    cmd.check_init(first_arg, "job not initialized");
  if (!cmd.ok()) return cmd.status();
  ThreadPool *pool;
  if (has_pool)
    pool = cmd.shared_arg<ThreadPool>(0);
  else {
    if (!currentThreadPoolRef)
      return cmd.abort("no current threadpool defined");
    pool = currentThreadPoolRef;
  }
  Job *job;
  if (cmd.argtype(first_arg) == type_job)
    job = *(Job **)(cmd.arg(first_arg));
  else
    job = new ProcJob((char *)(cmd.arg(first_arg)));
  leftv a = arg->next;
  if (has_pool) a = a->next;
  if (has_prio) a = a->next;
  for (; a != NULL; a = a->next) {
    job->args.push_back(LinTree::to_string(a));
  }
  if (job->pool)
    return cmd.abort("job has already been scheduled");
  job->prio = prio;
  pool->attachJob(job);
  cmd.set_result(type_job, new_shared(job));
  return cmd.status();
}

static BOOLEAN waitJob(leftv result, leftv arg) {
  Command cmd("waitJob", result, arg);
  cmd.check_argc(1);
  cmd.check_arg(0, type_job, "argument must be a job");
  cmd.check_init(0, "job not initialized");
  if (cmd.ok()) {
    Job *job = *(Job **)(cmd.arg(0));
    ThreadPool *pool = job->pool;
    if (!pool) {
      return cmd.abort("job has not yet been started or scheduled");
    }
    pool->waitJob(job);
    if (job->cancelled) {
      return cmd.abort("job has been cancelled");
    }
    if (job->result.size() == 0)
      cmd.no_result();
    else {
      leftv res = LinTree::from_string(job->result);
      cmd.set_result(res->Typ(), res->Data());
    }
  }
  return cmd.status();
}

void waitJob(Job *job) {
  assert(job->pool != NULL);
  job->pool->waitJob(job);
}

static BOOLEAN cancelJob(leftv result, leftv arg) {
  Command cmd("cancelJob", result, arg);
  cmd.check_argc(1);
  cmd.check_arg(0, type_job, "argument must be a job");
  cmd.check_init(0, "job not initialized");
  if (cmd.ok()) {
    Job *job = cmd.shared_arg<Job>(0);
    ThreadPool *pool = job->pool;
    if (!pool) {
      return cmd.abort("job has not yet been started or scheduled");
    }
    pool->cancelJob(job);
    cmd.no_result();
  }
  return cmd.status();
}

static BOOLEAN jobCancelled(leftv result, leftv arg) {
  Job *job;
  Command cmd("jobCancelled", result, arg);
  cmd.check_argc(0, 1);
  if (cmd.nargs() == 1) {
    cmd.check_arg(0, type_job, "argument must be a job");
    cmd.check_init(0, "job not initialized");
    job = cmd.shared_arg<Job>(0);
  } else {
    job = currentJobRef;
    if (!job)
      cmd.report("no current job");
  }
  if (cmd.ok()) {
    ThreadPool *pool = job->pool;
    if (!pool) {
      return cmd.abort("job has not yet been started or scheduled");
    }
    pool->scheduler->lock.lock();
    cmd.set_result((long) job->cancelled);
    pool->scheduler->lock.unlock();
  }
  return cmd.status();
}

bool getJobCancelled(Job *job) {
  ThreadPool *pool = job->pool;
  if (pool) pool->scheduler->lock.lock();
  bool result = job->cancelled;
  if (pool) pool->scheduler->lock.unlock();
  return result;
}

bool getJobCancelled() {
  return getJobCancelled(currentJobRef);
}

void setJobData(Job *job, void *data) {
  ThreadPool *pool = job->pool;
  if (pool) pool->scheduler->lock.lock();
  job->data = data;
  if (pool) pool->scheduler->lock.unlock();
}


void *getJobData(Job *job) {
  ThreadPool *pool = job->pool;
  if (pool) pool->scheduler->lock.lock();
  void *result = job->data;
  if (pool) pool->scheduler->lock.unlock();
  return result;
}

void addJobArgs(Job *job, leftv arg) {
  ThreadPool *pool = job->pool;
  if (pool) pool->scheduler->lock.lock();
  while (arg) {
    job->args.push_back(LinTree::to_string(arg));
    arg = arg->next;
  }
  if (pool) pool->scheduler->lock.unlock();
}

leftv getJobResult(Job *job) {
  ThreadPool *pool = job->pool;
  if (pool) pool->scheduler->lock.lock();
  leftv result = LinTree::from_string(job->result);
  if (pool) pool->scheduler->lock.unlock();
  return result;
}

const char *getJobName(Job *job) {
  // TODO
  return "";
}

void setJobName(Job *job, const char *name) {
  // TODO
}

static BOOLEAN createTrigger(leftv result, leftv arg) {
  Command cmd("createTrigger", result, arg);
  cmd.check_argc_min(1);
  int has_pool = cmd.test_arg(0, type_threadpool);
  ThreadPool *pool;
  if (has_pool) {
    cmd.check_init(0, "threadpool not initialized");
    pool = cmd.shared_arg<ThreadPool>(0);
  } else {
    pool = currentThreadPoolRef;
    if (!pool)
      return cmd.abort("no default threadpool");
  }
  cmd.check_argc(has_pool + 2);
  cmd.check_arg(has_pool + 0, STRING_CMD, "trigger subtype must be a string");
  const char *kind = (const char *)(cmd.arg(has_pool + 0));
  if (0 == strcmp(kind, "proc")) {
    cmd.check_arg(has_pool + 1, STRING_CMD, "proc trigger argument must be a string");
  } else {
    cmd.check_arg(has_pool + 1, INT_CMD, "trigger argument must be an integer");
  }
  if (cmd.ok()) {
    Trigger *trigger;
    long n = (long) (cmd.arg(has_pool + 1));
    if (n < 0)
      return cmd.abort("trigger argument must be a non-negative integer");
    if (0 == strcmp(kind, "acc")) {
      trigger = new AccTrigger(n);
    } else if (0 == strcmp(kind, "count")) {
      trigger = new CountTrigger(n);
    } else if (0 == strcmp(kind, "set")) {
      trigger = new SetTrigger(n);
    } else if (0 == strcmp(kind, "proc")) {
      trigger = new ProcTrigger((const char *) cmd.arg(has_pool + 1));
    } else {
      return cmd.abort("unknown trigger subtype");
    }
    pool->attachJob(trigger);
    cmd.set_result(type_trigger, new_shared(trigger));
  }
  return cmd.status();
}

static BOOLEAN updateTrigger(leftv result, leftv arg) {
  Command cmd("updateTrigger", result, arg);
  cmd.check_argc_min(1);
  cmd.check_arg(0, type_trigger, "first argument must be a trigger");
  cmd.check_init(0, "trigger not initialized");
  if (cmd.ok()) {
    Trigger *trigger = cmd.shared_arg<Trigger>(0);
    trigger->pool->scheduler->lock.lock();
    if (!trigger->accept(arg->next))
      cmd.report("incompatible argument type(s) for this trigger");
    else {
      trigger->activate(arg->next);
      if (trigger->ready()) {
        trigger->run();
	Scheduler::notifyDeps(trigger->pool->scheduler, trigger);
      }
    }
    trigger->pool->scheduler->lock.unlock();
  }
  return cmd.status();
}

static BOOLEAN chainTrigger(leftv result, leftv arg) {
  Command cmd("chainTrigger", result, arg);
  cmd.check_argc(2);
  cmd.check_arg(0, type_trigger, "first argument must be a trigger");
  cmd.check_arg(1, type_trigger, type_job,
    "second argument must be a trigger or job");
  cmd.check_init(0, "trigger not initialized");
  cmd.check_init(1, "trigger/job not initialized");
  if (cmd.ok()) {
    Trigger *trigger = cmd.shared_arg<Trigger>(0);
    Job *job = cmd.shared_arg<Job>(1);
    if (trigger->pool != job->pool)
      return cmd.abort("arguments use different threadpools");
    ThreadPool *pool = trigger->pool;
    pool->scheduler->lock.lock();
    job->triggers.push_back(trigger);
    pool->scheduler->lock.unlock();
  }
  return cmd.status();
}

static BOOLEAN testTrigger(leftv result, leftv arg) {
  Command cmd("testTrigger", result, arg);
  cmd.check_argc(1);
  cmd.check_arg(0, type_trigger, "argument must be a trigger");
  cmd.check_init(0, "trigger not initialized");
  if (cmd.ok()) {
    Trigger *trigger = cmd.shared_arg<Trigger>(0);
    ThreadPool *pool = trigger->pool;
    pool->scheduler->lock.lock();
    cmd.set_result((long)trigger->ready());
    pool->scheduler->lock.unlock();
  }
  return cmd.status();
}


static BOOLEAN scheduleJob(leftv result, leftv arg) {
  vector<Job *> jobs;
  vector<Job *> deps;
  Command cmd("scheduleJob", result, arg);
  cmd.check_argc_min(1);
  int has_pool = cmd.test_arg(0, type_threadpool);
  if (has_pool) {
    cmd.check_arg(0, type_threadpool, "first argument must be a threadpool");
    cmd.check_init(0, "threadpool not initialized");
  }
  cmd.check_argc_min(has_pool+1);
  int has_prio = cmd.test_arg(has_pool, INT_CMD);
  ThreadPool *pool;
  if (has_pool)
    pool = cmd.shared_arg<ThreadPool>(0);
  else {
    if (!currentThreadPoolRef)
      return cmd.abort("no current threadpool defined");
    pool = currentThreadPoolRef;
  }
  long prio = has_prio ? (long) cmd.arg(has_pool) : 0L;
  int first_arg = has_pool + has_prio;
  if (cmd.test_arg(first_arg, type_job)) {
    jobs.push_back(*(Job **)(cmd.arg(first_arg)));
  } else if (cmd.test_arg(first_arg, STRING_CMD)) {
    jobs.push_back(new ProcJob((char *)(cmd.arg(first_arg))));
  } else if (cmd.test_arg(first_arg, LIST_CMD)) {
    lists l = (lists) (cmd.arg(first_arg));
    int n = lSize(l);
    for (int i = 0; i < n; i++) {
      if (l->m[i].Typ() != type_job)
        return cmd.abort("job argument must be a job, string, or list of jobs");
    }
    for (int i = 0; i < n; i++) {
      Job *job = *(Job **) (l->m[i].Data());
      if (!job)
        return cmd.abort("job not initialized");
      jobs.push_back(job);
    }
  } else {
    return cmd.abort("job argument must be a job, string, or list of jobs");
  }
  bool error = false;
  leftv a = arg->next;
  if (has_pool) a = a->next;
  if (has_prio) a = a->next;
  for (; !error && a; a = a->next) {
    if (a->Typ() == type_job || a->Typ() == type_trigger) {
      deps.push_back(*(Job **)(a->Data()));
    } else if (a->Typ() == LIST_CMD) {
      lists l = (lists) a->Data();
      int n = lSize(l);
      for (int i = 0; i < n; i++) {
        if (l->m[i].Typ() == type_job || l->m[i].Typ() == type_trigger) {
          deps.push_back(*(Job **)(l->m[i].Data()));
        } else {
          error = true;
          break;
        }
      }
    }
  }
  if (error) {
    return cmd.abort("illegal dependency");
  }
  for (int i = 0; i < jobs.size(); i++) {
    Job *job = jobs[i];
    if (job->pool) {
      return cmd.abort("job has already been scheduled");
    }
    job->prio = prio;
  }
  for (int i = 0; i < deps.size(); i++) {
    Job *job = deps[i];
    if (!job->pool) {
      return cmd.abort("dependency has not yet been scheduled");
    }
    if (job->pool != pool) {
      return cmd.abort("dependency has been scheduled on a different threadpool");
    }
  }
  pool->scheduler->lock.lock();
  bool cancelled = false;
  for (int i = 0; i < jobs.size(); i++) {
    jobs[i]->addDep(deps);
  }
  for (int i = 0; i < deps.size(); i++) {
    deps[i]->addNotify(jobs);
    cancelled |= deps[i]->cancelled;
  }
  for (int i = 0; i < jobs.size(); i++) {
    if (cancelled) {
      jobs[i]->pool = pool;
      pool->cancelJob(jobs[i]);
    }
    else
      pool->attachJob(jobs[i]);
  }
  pool->scheduler->lock.unlock();
  if (jobs.size() > 0)
    cmd.set_result(type_job, new_shared(jobs[0]));
  return cmd.status();
}

BOOLEAN currentJob(leftv result, leftv arg) {
  Command cmd("currentJob", result, arg);
  cmd.check_argc(0);
  Job *job = currentJobRef;
  if (job) {
    cmd.set_result(type_job, new_shared(job));
  } else {
    cmd.report("no current job");
  }
  return cmd.status();
}


BOOLEAN threadID(leftv result, leftv arg) {
  int i;
  if (wrong_num_args("threadID", arg, 0))
    return TRUE;
  result->rtyp = INT_CMD;
  result->data = (char *)thread_id;
  return FALSE;
}

BOOLEAN mainThread(leftv result, leftv arg) {
  int i;
  if (wrong_num_args("mainThread", arg, 0))
    return TRUE;
  result->rtyp = INT_CMD;
  result->data = (char *)(long)(thread_id == 0L);
  return FALSE;
}

BOOLEAN threadEval(leftv result, leftv arg) {
  if (wrong_num_args("threadEval", arg, 2))
    return TRUE;
  if (arg->Typ() != type_thread) {
    WerrorS("threadEval: argument is not a thread");
    return TRUE;
  }
  InterpreterThread *thread = *(InterpreterThread **)arg->Data();
  string expr = LinTree::to_string(arg->next);
  ThreadState *ts = thread->getThreadState();
  if (ts && ts->parent != pthread_self()) {
    WerrorS("threadEval: can only be called from parent thread");
    return TRUE;
  }
  if (ts) ts->lock.lock();
  if (!ts || !ts->running || !ts->active) {
    WerrorS("threadEval: thread is no longer running");
    if (ts) ts->lock.unlock();
    return TRUE;
  }
  ts->to_thread.push("e");
  ts->to_thread.push(expr);
  ts->to_cond.signal();
  ts->lock.unlock();
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN threadExec(leftv result, leftv arg) {
  if (wrong_num_args("threadExec", arg, 2))
    return TRUE;
  if (arg->Typ() != type_thread) {
    WerrorS("threadExec: argument is not a thread");
    return TRUE;
  }
  InterpreterThread *thread = *(InterpreterThread **)arg->Data();
  string expr = LinTree::to_string(arg->next);
  ThreadState *ts = thread->getThreadState();
  if (ts && ts->parent != pthread_self()) {
    WerrorS("threadExec: can only be called from parent thread");
    return TRUE;
  }
  if (ts) ts->lock.lock();
  if (!ts || !ts->running || !ts->active) {
    WerrorS("threadExec: thread is no longer running");
    if (ts) ts->lock.unlock();
    return TRUE;
  }
  ts->to_thread.push("x");
  ts->to_thread.push(expr);
  ts->to_cond.signal();
  ts->lock.unlock();
  result->rtyp = NONE;
  return FALSE;
}

BOOLEAN threadPoolExec(leftv result, leftv arg) {
  Command cmd("threadPoolExec", result, arg);
  ThreadPool *pool;
  cmd.check_argc(1, 2);
  int has_pool = cmd.nargs() == 2;
  if (has_pool) {
    cmd.check_arg(0, type_threadpool, "first argument must be a threadpool");
    cmd.check_init(0, "threadpool not initialized");
    pool = cmd.shared_arg<ThreadPool>(0);
  } else {
    pool = currentThreadPoolRef;
    if (!pool)
      return cmd.abort("no current threadpool");
  }
  if (cmd.ok()) {
    string expr = LinTree::to_string(has_pool ? arg->next : arg);
    Job* job = new ExecJob();
    job->args.push_back(expr);
    job->pool = pool;
    pool->broadcastJob(job);
  }
  return cmd.status();
}

BOOLEAN threadResult(leftv result, leftv arg) {
  if (wrong_num_args("threadResult", arg, 1))
    return TRUE;
  if (arg->Typ() != type_thread) {
    WerrorS("threadResult: argument is not a thread");
    return TRUE;
  }
  InterpreterThread *thread = *(InterpreterThread **)arg->Data();
  ThreadState *ts = thread->getThreadState();
  if (ts && ts->parent != pthread_self()) {
    WerrorS("threadResult: can only be called from parent thread");
    return TRUE;
  }
  if (ts) ts->lock.lock();
  if (!ts || !ts->running || !ts->active) {
    WerrorS("threadResult: thread is no longer running");
    if (ts) ts->lock.unlock();
    return TRUE;
  }
  while (ts->from_thread.empty()) {
    ts->from_cond.wait();
  }
  string expr = ts->from_thread.front();
  ts->from_thread.pop();
  ts->lock.unlock();
  leftv val = LinTree::from_string(expr);
  result->rtyp = val->Typ();
  result->data = val->Data();
  return FALSE;
}

BOOLEAN setSharedName(leftv result, leftv arg) {
  Command cmd("setSharedName", result, arg);
  cmd.check_argc(2);
  int type = cmd.argtype(0);
  cmd.check_init(0, "first argument is not initialized");
  if (type != type_job && type != type_trigger && type != type_threadpool) {
    cmd.report("first argument must be a job, trigger, or threadpool");
  }
  cmd.check_arg(1, STRING_CMD, "second argument must be a string");
  if (cmd.ok()) {
    SharedObject *obj = cmd.shared_arg<SharedObject>(0);
    name_lock.lock();
    obj->set_name((char *) cmd.arg(1));
    name_lock.unlock();
  }
  return cmd.status();
}

BOOLEAN getSharedName(leftv result, leftv arg) {
  Command cmd("getSharedName", result, arg);
  cmd.check_argc(1);
  int type = cmd.argtype(0);
  cmd.check_init(0, "first argument is not initialized");
  if (type != type_job && type != type_trigger && type != type_threadpool) {
    cmd.report("first argument must be a job, trigger, or threadpool");
  }
  if (cmd.ok()) {
    SharedObject *obj = cmd.shared_arg<SharedObject>(0);
    name_lock.lock();
    cmd.set_result(obj->get_name().c_str());
    name_lock.unlock();
  }
  return cmd.status();
}

}

using namespace LibThread;


extern "C" int mod_init(SModulFunctions *fn)
{
  const char *libname = currPack->libname;
  if (!libname) libname = "";
  master_lock.lock();
  if (!thread_state)
    thread_state = new ThreadState[MAX_THREADS];
  makeSharedType(type_atomic_table, "atomic_table");
  makeSharedType(type_atomic_list, "atomic_list");
  makeSharedType(type_shared_table, "shared_table");
  makeSharedType(type_shared_list, "shared_list");
  makeSharedType(type_channel, "channel");
  makeSharedType(type_syncvar, "syncvar");
  makeSharedType(type_region, "region");
  makeSharedType(type_thread, "thread");
  makeSharedType(type_threadpool, "threadpool");
  makeSharedType(type_job, "job");
  makeSharedType(type_trigger, "trigger");
  makeRegionlockType(type_regionlock, "regionlock");

  fn->iiAddCproc(libname, "putTable", FALSE, putTable);
  fn->iiAddCproc(libname, "getTable", FALSE, getTable);
  fn->iiAddCproc(libname, "inTable", FALSE, inTable);
  fn->iiAddCproc(libname, "putList", FALSE, putList);
  fn->iiAddCproc(libname, "getList", FALSE, getList);
  fn->iiAddCproc(libname, "lockRegion", FALSE, lockRegion);
  fn->iiAddCproc(libname, "regionLock", FALSE, regionLock);
  fn->iiAddCproc(libname, "unlockRegion", FALSE, unlockRegion);
  fn->iiAddCproc(libname, "sendChannel", FALSE, sendChannel);
  fn->iiAddCproc(libname, "receiveChannel", FALSE, receiveChannel);
  fn->iiAddCproc(libname, "statChannel", FALSE, statChannel);
  fn->iiAddCproc(libname, "writeSyncVar", FALSE, writeSyncVar);
  fn->iiAddCproc(libname, "updateSyncVar", FALSE, updateSyncVar);
  fn->iiAddCproc(libname, "readSyncVar", FALSE, readSyncVar);
  fn->iiAddCproc(libname, "statSyncVar", FALSE, statSyncVar);

  fn->iiAddCproc(libname, "makeAtomicTable", FALSE, makeAtomicTable);
  fn->iiAddCproc(libname, "makeAtomicList", FALSE, makeAtomicList);
  fn->iiAddCproc(libname, "makeSharedTable", FALSE, makeSharedTable);
  fn->iiAddCproc(libname, "makeSharedList", FALSE, makeSharedList);
  fn->iiAddCproc(libname, "makeChannel", FALSE, makeChannel);
  fn->iiAddCproc(libname, "makeSyncVar", FALSE, makeSyncVar);
  fn->iiAddCproc(libname, "makeRegion", FALSE, makeRegion);
  fn->iiAddCproc(libname, "findSharedObject", FALSE, findSharedObject);
  fn->iiAddCproc(libname, "bindSharedObject", FALSE, bindSharedObject);
  fn->iiAddCproc(libname, "typeSharedObject", FALSE, typeSharedObject);

  fn->iiAddCproc(libname, "createThread", FALSE, createThread);
  fn->iiAddCproc(libname, "joinThread", FALSE, joinThread);
  fn->iiAddCproc(libname, "createThreadPool", FALSE, createThreadPool);
  fn->iiAddCproc(libname, "createThreadPoolSet", FALSE, createThreadPoolSet);
#if 0
  fn->iiAddCproc(libname, "adjoinThreadPool", FALSE, adjoinThreadPool);
  fn->iiAddCproc(libname, "getAdjoinedThreadPools", FALSE, getAdjoinedThreadPools);
#endif
  fn->iiAddCproc(libname, "closeThreadPool", FALSE, closeThreadPool);
  fn->iiAddCproc(libname, "getThreadPoolWorkers", FALSE, getThreadPoolWorkers);
  fn->iiAddCproc(libname, "setThreadPoolWorkers", FALSE, setThreadPoolWorkers);
  fn->iiAddCproc(libname, "getThreadPoolConcurrency", FALSE, getThreadPoolConcurrency);
  fn->iiAddCproc(libname, "setThreadPoolConcurrency", FALSE, setThreadPoolConcurrency);
  fn->iiAddCproc(libname, "currentThreadPool", FALSE, currentThreadPool);
  fn->iiAddCproc(libname, "setCurrentThreadPool", FALSE, setCurrentThreadPool);
  fn->iiAddCproc(libname, "threadPoolExec", FALSE, threadPoolExec);
  fn->iiAddCproc(libname, "threadID", FALSE, threadID);
  fn->iiAddCproc(libname, "mainThread", FALSE, mainThread);
  fn->iiAddCproc(libname, "threadEval", FALSE, threadEval);
  fn->iiAddCproc(libname, "threadExec", FALSE, threadExec);
  fn->iiAddCproc(libname, "threadResult", FALSE, threadResult);
  fn->iiAddCproc(libname, "createJob", FALSE, createJob);
  fn->iiAddCproc(libname, "currentJob", FALSE, currentJob);
  fn->iiAddCproc(libname, "setSharedName", FALSE, setSharedName);
  fn->iiAddCproc(libname, "getSharedName", FALSE, getSharedName);
  fn->iiAddCproc(libname, "startJob", FALSE, startJob);
  fn->iiAddCproc(libname, "waitJob", FALSE, waitJob);
  fn->iiAddCproc(libname, "cancelJob", FALSE, cancelJob);
  fn->iiAddCproc(libname, "jobCancelled", FALSE, jobCancelled);
  fn->iiAddCproc(libname, "scheduleJob", FALSE, scheduleJob);
  fn->iiAddCproc(libname, "scheduleJobs", FALSE, scheduleJob);
  fn->iiAddCproc(libname, "createTrigger", FALSE, createTrigger);
  fn->iiAddCproc(libname, "updateTrigger", FALSE, updateTrigger);
  fn->iiAddCproc(libname, "testTrigger", FALSE, testTrigger);
  fn->iiAddCproc(libname, "chainTrigger", FALSE, chainTrigger);

  LinTree::init();
  master_lock.unlock();

  return MAX_TOK;
}
