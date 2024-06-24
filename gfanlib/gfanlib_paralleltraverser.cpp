// Parallel abstract graph traverser. Contributed by Bjarne Knudsen.

#include <iostream>
#include <vector>
#include <deque>
#include <thread>
#include <condition_variable>
//#include <chrono>
#include <iomanip>

#include "gfanlib_paralleltraverser.h"

/*
  TODO:

  - Consider giving JobTransfers an affinity to a specific thread.

  - Consider if the amount of entries to be copied in the stack for
    deep searches can be reduced.

  - Consider making a more compact stack for limited in and out edge
    counts.
 */

using namespace std;

namespace gfan{

// This struct holds information about one step of a traversal.
struct TraverseState {
  // The number of next edges
  int  next_count;

  // The index of the next edge that was followed
  int  next_index;

  // The index of the previous edge to get back
  int  prev_index;

  TraverseState( int  next_count,
                 int  next_index,
                 int  prev_index )
  {
    this->next_count  =  next_count;
    this->next_index  =  next_index;
    this->prev_index  =  prev_index;
  }
};


void  traverse_simple_recursive( Traverser*  traverser )
{
  int  count  =  traverser->getEdgeCountNext();

  traverser->collectInfo();

  for (int i = 0; i < count; i++) {
    int  prev_index  =  traverser->moveToNext(i, true);

    if (prev_index == 0) {
      // Only traverse each state once, so do it for the zero'th
      // parent.
      traverse_simple_recursive(traverser);
    }

    traverser->moveToPrev(prev_index);
  }
}


// This function creates a stack that represents the full job to be
// done. Info is also collected for the start state.
vector<TraverseState>*  create_first_job_stack( Traverser*  traverser )
{
  vector<TraverseState>*  stack  =  new vector<TraverseState>();

  stack->push_back(TraverseState(traverser->getEdgeCountNext(), -1, -1));

  traverser->collectInfo();

  return stack;
}


void  traverse_simple_stack( Traverser*  traverser )
{
  // The stack holds information about what we have done so far
  vector<TraverseState>*  stack  =  create_first_job_stack(traverser);

  while (!stack->empty()) {
    stack->back().next_index++;

    TraverseState  state  =  stack->back();

    if (state.next_index == state.next_count || traverser->aborting /* Added by Anders */) {
      if (state.prev_index != -1) {
        traverser->moveToPrev(state.prev_index);
      }
      stack->pop_back();
    }
    else {
      int  prev_index  =  traverser->moveToNext(state.next_index, true);

      if (prev_index == 0) {
        traverser->collectInfo();
        stack->push_back(TraverseState(traverser->getEdgeCountNext(), -1, prev_index));
      }
      else {
        traverser->moveToPrev(prev_index);
      }
    }
  }

  delete stack;
}


void  traverse_simple( Traverser*  traverser )
{
  traverse_simple_stack(traverser);
}


// A Job holds a traverser and a stack representing the job. The stack
// is changed along with the traverser, so it represents the state of
// the traverser.
class Job {
  Traverser*  traverser;

  vector<TraverseState>*  stack;

  // The lowest index in the stack where there is some unfinished
  // work. -1 if there is no unfinished work.
  int  first_split;

  // For a given starting point, find the first index of the stack
  // with unfinished work. -1 if there is no unfinished work.
  static int  find_first_split( vector<TraverseState>*  stack,
                                int                     start )
  {
    auto  it = stack->begin();

    it += start;
    while (it != stack->end() && it->next_index == it->next_count - 1) {
      start++;
      it++;
    }

    if (it == stack->end()) {
      return -1;
    }
    else if (it->next_index == -1 && it->next_count == 1) {
      return -1;
    }

    return  start;
  }


public:
        bool aborting;                                                                                                                // Added by Anders
  // Create a new Job. if first_split is not set (or it is -2), the
  // first split will be found.
  Job( vector<TraverseState>*  stack = new vector<TraverseState>(),
       int                     first_split = -2 )
  :aborting(false)                                                                                                                // Added by Anders
  {
    if (first_split == -2) {
      first_split = find_first_split(stack, 0);
    }

    this->stack        =  stack;
    this->first_split  =  first_split;
  }


  ~Job( void )
  {
    delete stack;
  }


  // Set the traverser and move it to the start of this job based on
  // its last job.
  void  setTraverser( Traverser*  traverser,
                      Job*        last_job )
  {
    vector<TraverseState>*  last_stack  =  last_job->stack;
    unsigned int            i;

    this->traverser = traverser;

    // Find the first state where the stacks differ:
    for (i = 0; i < last_stack->size() && i < stack->size(); i++) {
      if ((*stack)[i].next_index != (*last_stack)[i].next_index) {
        break;
      }
    }

    if (i > 0) {
      // TODO: why is this needed?
      i--;
    }

    // roll back to the division point
    while (last_stack->size() > i + 1) {
      traverser->moveToPrev(last_stack->back().prev_index);
      last_stack->pop_back();
    }
    if (!last_stack->empty()) {
      last_stack->pop_back();
    }

    // go forward so the traverser represents the new job
    for (; i < stack->size() - 1; i++) {
      traverser->moveToNext((*stack)[i].next_index, false);
    }
  }


  // This function does some work and returns false when there is no
  // more work. Otherwise does at least step_count steps and returns
  // true when there is a subjob available.
  bool  step( int  step_count )
  {
    int  steps  =  0;

    // keep going if there is no subjob available
    while (steps < step_count || first_split == -1) {
      if (stack->empty()) {
        return false;
      }

      stack->back().next_index++;

      TraverseState  state  =  stack->back();

      if (state.next_index == state.next_count || aborting /* Added by Anders */) {
        if (state.prev_index != -1) {
          traverser->moveToPrev(state.prev_index);
        }
        stack->pop_back();

        if (first_split == -1) {
          // There is no more work to do in the previous states
          return false;
        }
      }
      else {
        if ((int) stack->size() - 1 == first_split) {
          if (state.next_index == state.next_count - 1) {
            first_split = -1;
          }
        }

        int  prev_index  =  traverser->moveToNext(state.next_index, true);
        aborting=traverser->aborting;                                                /* Added by Anders */
        steps++;

        if (prev_index == 0) {
          int  count  =  traverser->getEdgeCountNext();

          traverser->collectInfo();

          if (first_split == -1 && count > 1) {
            first_split = stack->size();
          }

          stack->push_back(TraverseState(count, -1, prev_index));
        }
        else {
          traverser->moveToPrev(prev_index);
        }
      }
    }

    return  true;
  }


  // Get a new subjob of the current job and adjust the current job so
  // it does not overlap with the new subjob. first_split may not be
  // -1 when calling this function.
  Job*  getSubjob( void )
  {
    // We can assume that first_split >= 0.
    auto  it = stack->begin();

    it += first_split + 1;
    // this new job will do the rest
    vector<TraverseState>*  new_stack  =  new vector<TraverseState>(stack->begin(), it);
    it--;

    // limit the existing job
    it->next_count = it->next_index + 1;

    Job* new_job =  new Job(new_stack, find_first_split(new_stack, first_split));

    first_split = find_first_split(stack, first_split);

    return new_job;
  }


  void  print( void )
  {
    cout << "--cc-nn-pp----------" << endl;
    for(vector<TraverseState>::const_iterator state=stack->begin();state!=stack->end();state++){
      //    for (TraverseState state : *stack) {
      cout << "  " << setw(2) << state->next_count << " " << setw(2) << state->next_index
           << " " << setw(2) << state->prev_index << endl;
    }
    cout << "--------------------" << endl;
  }
};


// This class is used to safely transfer a job from one thread to another
class JobTransfer {
  bool  is_set;

  Job*  job;

  mutex  mtx;

  condition_variable  cond;

public:
  JobTransfer( void )
  {
    is_set  =  false;
  }


  void  setJob( Job*  job )
  {
    mtx.lock();

    this->job  =  job;
    is_set     =  true;

    // notify should be done after unlock according to
    // http://en.cppreference.com/w/cpp/thread/condition_variable/notify_one
    // but doing so seems to cause a deadlock with 8 threads using
    // SubsetTraversers of size 16 on my laptop
    cond.notify_one();
    mtx.unlock();
  }


  Job*  getJob( void )
  {
    unique_lock<mutex>  lock(mtx);

    while (!is_set) {
      cond.wait(lock);
    }

    lock.unlock();

    return job;
  }
};


class JobCentral;

// A struct for holding all the information needed by a thread.
struct ThreadContext {
  JobCentral*  central;

  Traverser*  traverser;

  int  step_count;

  ThreadContext( JobCentral*  central,
                 Traverser*   traverser,
                 int          step_count )
  {
    this->central     =  central;
    this->traverser   =  traverser;
    this->step_count  =  step_count;
  }
};


void  work( ThreadContext*  context );


// This class is used running the whole threaded traversal and for
// exchanging jobs between threads
class JobCentral {
  ThreadContext**  contexts;

  int  context_count;

  int  step_count;

  Job*  first_job;

  mutex  mtx;

  // This queue is used for transferring jobs between threads. When a
  // thread requests a job, an empty transfer is put in this queue. It
  // will then be picked up by a thread that has a subjob
  // available. The subjob is given to the JobTransfer and is then
  // received by the thread needing it.
  deque<JobTransfer*>*  transfers;

public:
        bool aborting;                                                                        // Added by Anders
  // step_count is the number of algorithm steps taken between
  // possible job transfers. This value should be high (e.g. 100) if
  // the traverser is very fast. If the traverser is slow, step_count
  // should be one.
  JobCentral( Traverser**  traversers,
              int          count,
              int          step_count )
  :aborting(false)                                                                        // Added by Anders
  {
    context_count  =  count;
    contexts       =  new ThreadContext*[count];

    for (int i = 0; i < count; i++) {
      contexts[i] = new ThreadContext(this, traversers[i], step_count);
    }

    transfers = new deque<JobTransfer*>();
  }


  ~JobCentral( void )
  {
    delete transfers;
    for (int i = 0; i < context_count; i++) {
      delete contexts[i];
    }
    delete[] contexts;
  }


  // Run a job in multiple threads. This function is usually just
  // called once with a job representing everything to be done.
  void  runJob( Job*  job )
  {
    thread**  thr  = new thread*[context_count];

    this->first_job  =  job;

    for (int i = 0; i < context_count; i++) {
      thr[i] = new thread(work, contexts[i]);
    }

    for (int i = 0; i < context_count; i++) {
      thr[i]->join();
      delete thr[i];
    }

    delete[] thr;
  }


  bool  hasTransfer( void )
  {
        return !transfers->empty();
  }


  // If no threads are requesting jobs, the return value will be
  // NULL. Otherwise a JobTransfer will be returned ready for
  // receiving a new job.
  JobTransfer*  getTransfer( void )
  {
    JobTransfer*  transfer  =  NULL;

    mtx.lock();

    if (!transfers->empty()) {
      transfer = transfers->back();
      transfers->pop_back();
    }

    mtx.unlock();

    return  transfer;
  }


  // Request a job from another thread. The return value is NULL if all
  // jobs are done.
  Job*  getJob( void )
  {
    Job*  job  =  NULL;

    mtx.lock();

    if (first_job != NULL) {
      job        =  first_job;
      first_job  =  NULL;
    }
    else if ((int) transfers->size() < context_count - 1) {
      JobTransfer*  transfer =  new JobTransfer();

      transfers->push_front(transfer);

      mtx.unlock();

      job =  transfer->getJob();

      delete transfer;

      // return now because the mutex is already unlocked
      return job;
    }
    else {
      // We are fully done

      for(deque<JobTransfer*>::const_iterator tr=transfers->begin();tr!=transfers->end();tr++){
        //      for (JobTransfer*  tr : *transfers) {
        (*tr)->setJob(NULL);
      }
    }

    mtx.unlock();

    return job;
  }
};


// Do the actual work
void  work( ThreadContext*  context )
{
  Traverser*   traverser   =  context->traverser;
  JobCentral*  central     =  context->central;
  Job*         job;
  Job*         last_job   =  new Job();

  while ((job = central->getJob()) != NULL) {
    job->setTraverser(traverser, last_job);

    int  step_count  =  central->hasTransfer() ? 1 : context->step_count;

    if(central->aborting)job->aborting=true;                        // Added by Anders

    while (job->step(step_count)) {
      if(job->aborting)central->aborting=true;                        // Added by Anders
      JobTransfer*  transfer  =  central->getTransfer();

      if (transfer != NULL) {
        transfer->setJob(job->getSubjob());
      }

      step_count  =  central->hasTransfer() ? 1 : context->step_count;
    }

    delete last_job;
    last_job = job;
  }

  delete last_job;
}


// Do the actual work
void  work2( ThreadContext*  context )
{
  Traverser*   traverser   =  context->traverser;
  JobCentral*  central     =  context->central;
  int          step_count  =  context->step_count;
  Job*         job;
  Job*         last_job   =  new Job();

  while ((job = central->getJob()) != NULL) {
    job->setTraverser(traverser, last_job);

    while (job->step(step_count)) {
      JobTransfer*  transfer  =  central->getTransfer();

      if (transfer != NULL) {
        transfer->setJob(job->getSubjob());
      }
    }

    delete last_job;
    last_job = job;
  }

  delete last_job;
}


void  traverse_threaded( Traverser**  traversers,
                         int          count,
                         int          step_count )
{
  JobCentral*  central  =  new JobCentral(traversers, count, step_count);

  central->runJob(new Job(create_first_job_stack(traversers[0])));

  delete central;
}
}
