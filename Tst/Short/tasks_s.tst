LIB "tst.lib";
tst_init();

LIB "tasks.lib";

ring R = 0, (x,y), dp;
ideal I = x9y2+x10, x2y7-y8;
task t = createTask("std", list(I));
t;
killTask(t);

t = "std", list(I);
startTasks(t);
t;
killTask(t);
t;
getState(t);

task t1 = "std", list(I);
startTasks(t1);
waitAllTasks(t1);
task t2 = copyTask(t1);
killTask(t1);
t2;
getResult(t2);
killTask(t2);

t1 = "std", list(I);
t2 = "std", list(I);
compareTasks(t1, t2);
startTasks(t1);
waitAllTasks(t1);
t1 == t2;
killTask(t1);
killTask(t2);
ideal J = x;
task t3 = "std", list(I);
task t4 = "std", list(J);
t3 == t4;
killTask(t3);
killTask(t4);

printTask(t);
t = "std", list(I);
t;
startTasks(t);
waitAllTasks(t);
t;
killTask(t);

t1 = "std", list(I);
t2 = "slimgb", list(I);
startTasks(t1, t2);
waitAllTasks(t1, t2);
getResult(t1);
getResult(t2);
killTask(t1);
killTask(t2);

t = "std", list(I);
startTasks(t);
stopTask(t);
t;
killTask(t);

t1 = "std", list(I);
t2 = "slimgb", list(I);
startTasks(t1, t2);
waitTasks(list(t1, t2), 2);
getResult(t1);
getResult(t2);
killTask(t1);
killTask(t2);

t = "std", list(I);
startTasks(t);
waitAllTasks(t);
pollTask(t);
t;
getResult(t);
killTask(t);

t = "std", list(I);
getCommand(t);
killTask(t);

t = "std", list(I);
getArguments(t);
killTask(t);

t = "std", list(I);
startTasks(t);
waitAllTasks(t);
getResult(t);
killTask(t);

t = "std", list(I);
getState(t);
startTasks(t);
getState(t);
waitAllTasks(t);
getState(t);
killTask(t);
getState(t);

int sem = semaphore(1);
system("semaphore", "acquire", sem);
system("semaphore", "try_acquire", sem);
system("semaphore", "release", sem);
system("semaphore", "try_acquire", sem);

tst_status(1);$
