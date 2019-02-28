# Building the library

Run:

        ./configure && make && make install

Use:

        ./configure --singular=/path/to/singular

instead if you want to use this with a different Singular installation.

The "make install" command will install `systhreads.so` and `systhreads.lib`
in the MOD and LIB directories.

# Usage

The library can be used by loading it with:

        LIB "systhreads.lib";

# Threads

Threads can be created with `createThread()`. Once no longer in use,
thread resources should be disposed of with `joinThread()`. It is
possible to evaluate expressions in an interpreter with `threadEval()`
and retrieve the result with `threadResult()`.

Example:

        thread t = createThread();
        threadEval(t, quote("foo" + "bar"));
        threadResult(t);
        joinThread(t);

If no result is needed, `threadExec(th, expr)` can be used instead, as
can be the variants `threadExecString(th, string)` and
`threadExecFile(th, filename)`.

To load a library into a thread, `threadLoadLib(th, libname)` is available.

A unique numeric per-thread id is available via `threadID()`. To check
if the program is executing the main thread, the `mainThread()` function
can be used; it'll return `1` for the main thread and `0` for all other
threads.

# Shared objects

This library primarily provides a number of types to share data
between threads, such as channels or atomic hash tables. We call
instances of these types "shared objects".

Shared objects can contain basic Singular values, such as integers,
strings, lists, numbers (note: not all types of numbers are currently
supported), rings, polynomials, and ideals (note that not all Singular
types are supported yet). They can also contain references to other
shared objects. When basic singular values are stored in a shared
object, a copy of that value is created (if necessary, along with any
ring it is based on). Shared objects that are stored in other shared
objects are stored as references and are not copied.

# Channels

Channels are created via `makeChannel(uri)`. You can use
`sendChannel(ch, val)` to send values to a channel and
`receiveChannel(ch)` to read them from the channel.

        channel ch = makeChannel("channel:example");
        sendChannel(ch, 314);
        receiveChannel(ch);

Also, `statChannel(ch)` returns the number of elements in the channel.
Note that this is an estimate, as new elements can concurrently be
written to or existing elements retrieved from the channel.

# Syncronization Variables

Synchronization Variables are created via `makeSyncVar(uri)`. You can
use `writeSyncVar(sv, val)` to write a value to a synchronization
variable and `readSyncVar(sv)` to read from it. Note that writing a
synchronization variable more than once results in an error and reading
from a synchronization variable that has not yet been written to will
block the underlying thread.

Example:

        syncvar sv = makeSyncVar("syncvar:example");
        writeSyncVar(sv, list(1, 2, 3));
        readSyncVar(sv);

Like all shared objects, synchronization variables can contain
other shared objects. This is particularly useful to make them
globally available across all threads.

Example:

        syncvar channel_var = makeSyncVar("syncvar:channel");
        writeSyncVar(channel_var, makeChannel("channel:main"));
        channel ch = readSyncVar(channel_var);
        sendChannel(ch, "test");
        readChannel(ch);

# Atomic Tables

Atomic hash tables exist to store values that can concurrently been
accessed by multiple threads. An atomic table is created via
`makeAtomicTable(uri)`; it can be written to with `putTable(table, key,
value)`, read with `getTable(table, key)`, and you can check whether
a table holds a key with `inTable(table, key)`.

Example:

        atomic_table table = makeAtomicTable("table:example");
        putTable(table, "foo", 314);
        inTable(table, "foo");
        inTable(table, "bar");
        getTable(table, "foo");

# Atomic Lists

Atomic lists work like atomic hash tables, but are indexed by integers
rather than strings. They are created via `makeAtomicList(uri)`, are
written with `putList(list, index, value)`, and read with `getList(list,
index)`. Atomic lists resize automatically if a value is entered at an
index beyond the end of the list.

Example:

        atomic_list list = makeAtomicList("list:example");
        putList(list, 3, 2718)
        getList(list, 3);

# Regions

Regions are individually lockable regions of memory that can contain
other objects, namely shared hash tables and lists. These tables and
lists work like atomic tables and lists, but can only be accessed when
the region is locked.

Example:

        region r = makeRegion("region:example");
        lockRegion(r);
        shared_table table = makeSharedTable(r, "table");
        shared_list list = makeSharedList(r, "list");
        putTable(table, "foo", 314);
        getTable(table, "foo");
        putList(list, 1, 2718);
        getList(list, 1);
        unlockRegion(r);

Attempting to use list and table operations while the region is not
locked by the current thread will result in an error.

# Region locks

To make management of region locking easier, there is a regionlock
type. This can store the result of a `lockRegion(r)` call and will
automatically unlock when it goes out of scope.

Example:

        proc testGlobals(region r, string key) {
          regionlock lk = regionLock(r);
          shared_table globals = makeSharedTable(r, "globals");
          return(inTable(globals, key));
        }
