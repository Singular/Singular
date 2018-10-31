// Parallel abstract graph traverser. Contributed by Bjarne Knudsen.

#ifndef __traverser_h__
#define __traverser_h__

/*
  This file defines a Traverser interface. A Traverser can be used for
  going through an underlying structure of states. At each state,
  there is a number of next states. Moving the traverser to a next
  state changes its internal state.

  When constructing a Traverser, it should be in the start state
  (i.e. it should have no previous states). The traversal is over when
  there are no more next states.

  The state space should be a directed acyclic graph, so several
  previous states to a given state are allowed. There may be any
  number of end points, but only one start point.

  When traversing in a multi threaded fashion, each Traverser receives
  some of the collected information. It is up to the user of this
  interface to join that information when the traversal is done.
*/
namespace gfan{

class Traverser
{
public:
        bool aborting;                                         // Added by Anders
        void abort(){aborting=true;}        // Added by Anders
        Traverser():aborting(false){}        // Added by Anders
  // Virtual destructor
  virtual ~Traverser( void ) {};

  // Get the number of next states.
  virtual int  getEdgeCountNext( void ) = 0;

  // The return value is the index for moving to the same previous
  // state again. The indexing of the previous state can be arbitrary,
  // but zero should be one of the previous index values. The
  // collect_info parameter will be true once for every edge in the
  // state graph during the traversal. This allows te traverser to
  // collect information along its edges.
  virtual int  moveToNext( int   index,
                           bool  collect_info ) = 0;

  // Go back to a previous state. Due to the return value of
  // moveToNext(), the state will be unchanged after calling
  // movetoPrev(moveToNext(index)) with a legal next index.
  virtual void  moveToPrev( int  index ) = 0;

  // This function will be called once for every state in a traversal.
  virtual void  collectInfo( void ) = 0;

  // Function for printing the state to cout for debug purposes.
  virtual void  printState( void ) = 0;
};

// Traverse a structure in a single threaded way. The traverser should
// be in the start state. The traverser may not be in the start state
// when this function returns.
void  traverse_simple( Traverser*  traverser );

// Traverse a structure using several traversers in several
// threads. The traversers should all be in the start state. Several
// traversers may go through the same state, but collectInfo() is only
// called once for each state. The traversers may not be in the start
// state when this function returns.
void  traverse_threaded( Traverser**  traverser,
                         int          count,
                         int          step_count );

}
#endif // __traverser_h__
