# Userspace Threading

Note: priorities are not yet fully supported.

# Threadpools

The function `createThreadPool()` is used to create a new threadpool:

    threadpool pool = createThreadPool(int nthreads);

The `nthreads` arguments defines how many worker threads the threadpool
should have (and thus the maximum level of parallelism that the threadpool
allows for). The value zero for `nthreads` is special; it will not actually
create any separate threads, but create a pseudo threadpool that is
subordinate to the current thread. It is useful for testing and debugging
and can also be used to emulate user-space threads in single-threaded
Singular.

A threadpool can be shut down with the `closeThreadPool()` function:

    int remaining = closeThreadPool(threadpool pool[, int wait]);

The `pool` argument is the threadpool that is to be shutdown. The pool
will be shutdown immediately after all threads finish executing their
current jobs. If the optional argument `wait` is non-zero,
`closeThreadPool()` will wait with shutting down the pool until there
are no remaining jobs that can be executed anymore, either because there
are none left or because all remaining jobs are not ready.

It returns an integer that describes the remaining number of jobs
remaining in the threadpool.

The current threadpool can be set and queried via the
`setCurrentThreadPool()` and `currentThreadPool()` functions:

    setCurrentThreadPool(threadpool pool);
    threadpool pool = currentThreadPool();

Setting the current threadpool allows the threadpool argument for
the `startJob()` and `scheduleJob()` functions to be omitted (see
below). While executing a job, the current threadpool is automatically
set without requiring external action.

# Threadpool Initialization

Threadpools can be initialized with any of the following functions that
requests them to (respectively) load a library, execute a quoted
expression, execute the contents of a file, execute the contents of a
string, or call a function:

    threadPoolsLoadLib(threadpool pool, string lib);
    threadPoolExec(threadpool pool, def quote_expr);
    threadPoolExecFile(threadpool pool, string file);
    threadPoolExecString(threadpool pool, string str);
    threadPoolExecFunc(threadpool pool, string funcname);

For a threadpool, this initialization request is passed on to all
threads in the pool. Initialization happens after any currently running
jobs have been processed. At program startup, this occurs immediately.

# Creating Jobs

A job is a descriptor for a piece of work that a thread is meant to
perform. The basic job is 

    job j = createJob(string func[, def arg1, ..., def argn]);

Here, `func` is the name of the function to be executed. This name
can include a package name and `"Current"` for the current package.
Examples are `"foo"`, `"Somepackage::foo"`, or `"Current::foo"`.

The arguments `arg1` through `argn` are any arguments to be passed
along. The argument list *can* be partial; one does not have to supply
any arguments at job creation, but the missing arguments have to be
supplied at startup (see below).

Examples:

    proc add(int x, int y) { return (x+y); }

    job adder = createJob("add");
    job increment = createJob("add", 1);
    job add2and2 = createJob("add", 2, 2);

In this example, three different jobs are created. The first one,
`adder`, will still require two arguments in order to be run. The
second one, `increment`, requires one additional argument. And the
`add2and2` job does not require any more arguments.

Jobs can also be created from other jobs by pasing in a job argument
instead of a function name as the first argument:

    job j2 = createJob(job j[, def arg1, ..., def argn]);

Any arguments are appended to the jobs existing argument list. Note that
the original job will not be modified: the call to `createJob` will
first create a copy, then add the arguments to the copy.

Finally, one can also supply a quoted expression to `createJob()`:

    createJob(def quote_expr);

Example:

    job j = createJob(quote(factorial(1000)));

Within the kernel, additionally C/C++ jobs can be created by supplying
a function pointer (and optional arguments). These jobs can be returned

For debugging purposes, a job can be given a descriptive name with the
`nameJob()` function:

    nameJob(job j, string name);

This has no effect on execution, but can be useful for debugging. The
name can be queried with the `getJobName()` function:

    string name = getJobName(job j);

If no name is explicitly assigned to a job, the name defaults to the
function name or (for quoted expression) to the fixed string
`"quote(...)"`.

# Running jobs

The simplest way to execute a job is with the `startJob()` function:

    job j2 = startJob([threadpool pool, [int prio,]]
        job|string j[, def arg1, ..., def argn]);

The first argument is the optional threadpool on which to execute the
job; if absent, it defaults to the current threadpool. It is followed
(optionally) by a priority argument, which defaults to zero. Higher
numbers indicate a higher priority. High priority jobs will preempt
low priority jobs; note that this can lead to low priority jobs being
starved if there is a constant influx of high priority jobs.

The next argument is the actual job. For simple jobs, a string
describing a function name can be supplied instead, which follows
the same conventions as the first argument for `createJob()`. It
allows bypassing the `createJob()` step for simple jobs.

Finally, the remaining arguments are taken to be the remaining
arguments for the job. They are appended to any arguments provided
during `createJob()`.

For convenience, `startJob()` will also return the job it was passed
in (or the job it created if a function name was supplied).

A job scheduled by `startJob()` will be executed as soon as a worker
thread becomes available to run it.

Example:

    proc add(int x, int y) { return (x+y); }

    threadpool pool = createThreadPool(4);
    job inc = createJob("add", 1);
    startJob(pool, inc, 1);

The result of a job can be queried with `waitJob()`:

    def result = waitJob(job j);

Note that a job need not return a result; if the underlying function
does not return a value, the job will not have a result. One can still
call `waitJob()` on the job, but it likewise will not return a value.

Example:

    proc add(int x, int y) { return (x+y); }

    threadpool pool = createThreadPool(4);
    job inc = createJob("add", 1);
    startJob(pool, inc, 1);
    int result = waitJob(inc);

A job's execution can be cancelled with `cancelJob()`:

    cancelJob(job j);

If the job has not been executed by a worker thread yet, it is removed
from the scheduler. If it has been executed already, the function will
not do anything. If it is being executed, nothing will be done, either.
However, a flag will be set for the job and the job can use the function
`jobCancelled()` to find out if cancellation has been requested and
abort early.

Example:

    if (jobCancelled()) { return (); }

More interestingly, the execution of jobs can be made contingent on
the completion of other jobs or on triggers.

    scheduleJobs([threadpool pool, [int prio,]]
      list|job|string jobs[,
      list|job|trigger dep1, ..., list|job|trigger depn]);

The first two arguments (`pool` and `prio`) are the same as for
`startJob()` and are again both optional. The third argument is either a
job, a function name (with the same semantics as for `startJob()`) or a
list of jobs.

The remaining arguments list dependencies. The jobs will not be executed
until all dependencies are met, i.e. the underlying tasks are completed
or the triggers have been fully activated.

If any of the jobs or triggers `dep1` through `depn` return results,
then those will be appended as arguments to the scheduled jobs.

Example:

    proc add(int x, int y) { return (x+y); }

    threadpool pool = createThreadPool(4);
    job j1 = startJob("add", 1, 2);
    job j2 = startJob("add", 3, 4);
    job total = createJob("add");
    scheduleJob(total, j1, j2);
    int sum = waitJob(total);

# Triggers

Triggers allow the programmer to create more complex interactions
between jobs.

    trigger trig = createTrigger([threadpool pool,]
      string type[, def arg1, ..., argn]);

We currently support three types of triggers, accumulators, counters, and
sets.

    trigger trig = createTrigger("acc", int n);
    trigger trig = createTrigger("count", int n);
    trigger trig = createTrigger("set", int n);

An accumulator trigger will accumulate a set of results. It will fire
when `n` results have been added; any accumulated results will be passed
as an additional list argument to triggered jobs.

A counter trigger will simply count up and fire when it has been activated `n`
times. A set trigger represents the set `{1..n}` and will fire when all
elements in the set have been added at least once. Neither of the last
two will pass any additional arguments to triggered jobs.

Adding to a trigger can be done with `updateTrigger()`.

    updateTrigger(trigger trig[, def arg]);

Triggers can also be activated when a job finishes or another trigger
fires via the `chainTrigger()` function:

    chainTrigger(trigger trig, job|trigger dep);

The state of a trigger can be tested explicitly with `testTrigger()`,
which returns 1 or 0, depending on whether it has fired or not.

    int success = testTrigger(trigger trig);

Because it is often useful to discard any remaining jobs when a trigger
fires, the `cancelOnTrigger()` function allows to automate this.

    cancelOnTrigger(trigger trig, job|list arg1, ..., job|list argn);

The function takes an arbitrary number of jobs or lists of jobs in addition
to a trigger argument. When the trigger fires, all those jobs will be
cancelled if they haven't finished yet.

A simple use of triggers is to wait for the first job in a list to be
triggered.
