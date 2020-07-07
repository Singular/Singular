#ifndef _LIBTHREAD_THREADPOOL_H
#define _LIBTHREAD_THREADPOOL_H

namespace LibThread {
  class ThreadPool;
  class Job;
  // thread pools
  ThreadPool *createThreadPool(int threads, int prioThreads = 0);
  void closeThreadPool(ThreadPool *pool, bool wait);
  ThreadPool *getCurrentThreadPool();
  // job creation
  Job *createJob(void (*func)(leftv result, leftv arg));
  Job *createJob(void (*func)(long ndeps, Job **deps));
  // job status
  Job *getCurrentJob();
  bool getJobCancelled();
  bool getJobCancelled(Job *job);
  void addJobArgs(Job *job, leftv arg);
  void setJobData(Job *job, void *data);
  void *getJobData(Job *job);
  leftv getJobResult(Job *job);
  const char *getJobName();
  void setJobName(const char *);
  // Job scheduling
  Job *startJob(ThreadPool *pool, Job *job, leftv arg);
  Job *startJob(ThreadPool *pool, Job *job);
//   Job *scheduleJob(ThreadPool *pool, Job *job, long ndeps, Job **deps);
  void cancelJob(Job *job);
  void waitJob(Job *job);
  // reference counting
  void release(Job *job);
  void release(ThreadPool *pool);
  void retain(Job *job);
  void retain(ThreadPool *pool);
}

#endif
