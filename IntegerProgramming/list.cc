// list.cc

// implementation of class list and class list iterator

#ifndef LIST_CC
#define LIST_CC

#include "list.h"

/////////////////////////////////////////////////////////////////////////////
///////////////////////// class list ////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////// constructors and destructor /////////////////////////////

list::list()
{

#ifdef DL_LIST

  element* begin_dummy=new element;

#endif  // DL_LIST

  element* end_dummy=new element;

#ifdef DL_LIST

  begin_dummy->previous=NULL;
  begin_dummy->next=end_dummy;
  begin_dummy->entry=NULL;
  begin_dummy->done=FALSE;
  begin_dummy->head_reduced=FALSE;
  end_dummy->previous=begin_dummy;

#endif  // DL_LIST

  end_dummy->next=NULL;
  end_dummy->entry=NULL;
  end_dummy->done=TRUE;
  end_dummy->head_reduced=TRUE;

#ifdef DL_LIST

  start=begin_dummy;

#endif  // DL_LIST

#ifdef SL_LIST

  start=end_dummy;

#endif  // SL_LIST

}

// The dummy elements have the following functions:
// - The end_dummy guarantees that the deletion method of the simply linked
//   list works: Deletion is done by copying the next element to the actual
//   position and then deleting the original, see below for an explaination.
//   This would cause problems when deleting the last element; then the
//   next-pointer of the preceeding element would reference freed memory
//   (it cannot be manipulated, is unknown). So the end_dummy as an
//   artificial last element avoids this problem.
// - With the described deletion method for a simply linked list, the start
//   pointer of a list has never to be changed by the list_iterator class
//   (as long as the list is not empty; but with the end_dummy, the list
//   never becomes empty). So a list iterator must never know on which list
//   he operates on.
//   Doubly linked lists use another, more secure deletion method (which
//   really deletes the actual element). Therefore, the comfort of a start
//   pointer which has never to be manipulated by the iterators must be
//   reached in another way: This is the purpose of the begin_dummy, an
//   artificial first element which is referenced by the start pointer and
//   never changed.
// - With the dummy elements, we do need to distinguish the cases of inserting
//   or deleting at the beginning or the end of a list.
// - Furthermore, the end_dummy is a stopper for Buchberger's algorithm:
//   By setting all flags to TRUE, we can eliminate many calls of the
//   is_at_end()-function of the list iterator class.
// The dummy elements should never be deleted (the dletion method is not
// secure)!




list::list(const list& l)
{

#ifdef DL_LIST

  element* begin_dummy=new element;

#endif  // DL_LIST

  element* end_dummy=new element;

#ifdef DL_LIST

  begin_dummy->previous=NULL;
  begin_dummy->next=end_dummy;
  begin_dummy->entry=NULL;
  begin_dummy->done=FALSE;
  begin_dummy->head_reduced=FALSE;

  end_dummy->previous=NULL;

#endif  // DL_LIST

  end_dummy->next=NULL;
  end_dummy->entry=NULL;
  end_dummy->done=TRUE;
  end_dummy->head_reduced=TRUE;

#ifdef DL_LIST

  start=begin_dummy;
  element *iter=(l.start)->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element* iter=l.start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: list::list(const list&):\ntry to construct a list from"
      " a corrupt one; empty list created"<<endl;
    return;
  }

  while((iter->next)!=NULL)
    // end_dummy not reached
  {
    copy_insert(*(iter->entry));
    iter=iter->next;
  }

}




list::~list()
{

#ifdef DL_LIST

  element *iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  while(iter->next!=NULL)
    // delete non-dummy elements
  {
    element* aux=iter;
    iter=iter->next;
    delete aux->entry;
    delete aux;
  }

  // end_dummy reached, delete it
  delete iter;

#ifdef DL_LIST

  // delete begin_dummy
  delete start;

#endif  // DL_LIST

}




///////////////////// inserting ////////////////////////////////////////////

// For a better overview, the code for the simply and for the doubly linked
// list is separated (except from the "copy-insert"-routines).




#ifdef DL_LIST




list& list::insert(binomial& bin)
{
  // insert at the beginning (after the begin_dummy)

  // initialize element
  element* aux=new element;
  aux->entry=&bin;
  aux->done=FALSE;
  aux->head_reduced=FALSE;

  // set pointers
  aux->next=start->next;
  aux->next->previous=aux;
  aux->previous=start;
  start->next=aux;

  return(*this);
}




list& list::_insert(binomial& bin)
{
  // insert at the beginning

  // initialize element
  element* aux=new element;
  aux->entry=&bin;

  // set pointers
  aux->next=start->next;
  aux->next->previous=aux;
  aux->previous=start;
  start->next=aux;

  return(*this);
}




list& list::ordered_insert(binomial& bin, const term_ordering& w)
{

  // search for the place to insert
  element* iter=start->next;
  while(iter->entry!=NULL)
    // end_dummy not reached
  {
    if(w.compare(*(iter->entry),bin)<=0)
      // bin is bigger in term ordering then the referenced binomial
      iter=iter->next;
    else
      break;
  }

  // now bin is smaller in term ordering then the referenced binomial
  // or the referenced binomial is the end_dummy

  // initialize element
  element*aux=new element;
  aux->entry=&bin;
  aux->done=FALSE;
  aux->head_reduced=FALSE;

  // set pointers
  aux->previous=iter->previous;
  aux->previous->next=aux;
  aux->next=iter;
  iter->previous=aux;

  return *this;
}




list& list::_ordered_insert(binomial& bin, const term_ordering& w)
{
  // search for the place to insert
  element* iter=start->next;
  while(iter->entry!=NULL)
    // end_dummy not reached
  {
    if(w.compare(*(iter->entry),bin)<=0)
      // bin is bigger in term ordering then the referenced binomial
      iter=iter->next;
    else
      break;
  }

  // now bin is smaller in term ordering then the referenced binomial
  // or the referenced binomial is the end_dummy

  // initialize element
  element*aux=new element;
  aux->entry=&bin;

  // set pointers
  aux->previous=iter->previous;
  aux->previous->next=aux;
  aux->next=iter;
  iter->previous=aux;

  return *this;
}




#endif  // DL_LIST




#ifdef SL_LIST




list& list::insert(binomial& bin)
{
  // insert at the beginning

  // initialize element
  element* aux=new element;
  aux->entry=&bin;
  aux->done=FALSE;
  aux->head_reduced=FALSE;

  // set pointers
  aux->next=start;
  start=aux;

  return(*this);
}




list& list::_insert(binomial& bin)
{
  // insert at the beginning

  // initialize element
  element* aux=new element;
  aux->entry=&bin;

  // set pointers
  aux->next=start;
  start=aux;

  return(*this);
}




list& list::ordered_insert(binomial& bin, const term_ordering& w)
{
  // search for the place to insert
  element* iter=start;
  while(iter->entry!=NULL)
    // end_dummy not reached
  {
    if(w.compare(*(iter->entry),bin)<=0)
      // bin is bigger in term ordering then the referenced binomial
      iter=iter->next;
    else
      break;
  }

  // now bin is smaller in term ordering then the referenced binomial
  // or the referenced binomial is the end_dummy

  // here we have to consider a special case
  if(iter==start)
    return insert(bin);

  // insert new element by first allocating a new list place behind the
  // referenced element, then moving the referenced element to that
  // new place...
  element*aux=new element;
  aux->entry=iter->entry;
  aux->done=iter->done;
  aux->head_reduced=iter->head_reduced;
  aux->next=iter->next;

  // .. and finally inserting bin at the old place
  // Remember that we cannot insert a new element between the referenced
  // element and its preceeding (we do not know the preceeding element)
  iter->entry=&bin;
  iter->done=FALSE;
  iter->head_reduced=FALSE;
  iter->next=aux;

  return *this;
}




list& list::_ordered_insert(binomial& bin, const term_ordering& w)
{
  // search for the place to insert
  element* iter=start;
  while(iter->entry!=NULL)
    // end_dummy not reached
  {
    if(w.compare(*(iter->entry),bin)<=0)
      // bin is bigger in term ordering then the referenced binomial
      iter=iter->next;
    else
      break;
  }

  // now bin is smaller in term ordering then the referenced binomial
  // or the referenced binomial is the end_dummy

  // here we have to consider a special case
  if(iter==start)
    return _insert(bin);

  // insert new element by first allocating a new list place behind the
  // referenced element, then moving the referenced element to that
  // new place...
  element*aux=new element;
  aux->entry=iter->entry;
  aux->next=iter->next;

  // .. and finally inserting bin at the old place
  // Remember that we cannot insert a new element between the referenced
  // element and its preceeding (we do not know the preceeding element)
  iter->entry=&bin;
  iter->next=aux;

  return *this;
}




#endif  // SL_LIST




// copy-insert routines

list& list::copy_insert(const binomial& bin)
{
  binomial* _bin=new binomial(bin);
  return insert(*_bin);
}


list& list::_copy_insert(const binomial& bin)
{
  binomial *_bin=new binomial(bin);
  return _insert(*_bin);
}


list& list::ordered_copy_insert(const binomial& bin, const term_ordering& w)
{
  binomial *_bin=new binomial(bin);
  return ordered_insert(*_bin,w);
}


list& list::_ordered_copy_insert(const binomial& bin, const term_ordering& w)
{
  binomial *_bin=new binomial(bin);
  return _ordered_insert(*_bin,w);
}




/////////////////////////// output //////////////////////////////////////////




void list::print() const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::print() const:\ncannot print corrupt list"
        <<endl;
    return;
  }

  while(iter->next!=NULL)
  {
    iter->entry->print();
    iter=iter->next;
  }
}




void list::ordered_print(const term_ordering& w) const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::print(const term_ordering&) const:\n"
      "cannot print corrupt list"<<endl;
    return;
  }

  list aux;

  while(iter->next!=NULL)
  {
    aux._ordered_insert(*(iter->entry),w);
    iter=iter->next;
  }

  aux.print();

  // delete aux, but only the element structs, not the binomials
  // (these are still stored in the actual list!)

#ifdef DL_LIST

  iter=aux.start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  iter=aux.start;

#endif

  while(iter->next!=NULL)
    // end_dummy not reached
  {
    element* aux2=iter->next;
    iter->next=iter->next->next;
    delete aux2;
  }

  // the dummy elements are deleted by the destructor

}




void list::print(FILE* output) const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::print(FILE*) const:\ncannot print corrupt "
      "list"<<endl;
    fprintf(output,"\nWARNING: void list::print(FILE*) const:\ncannot print "
            "corrupt list\n");
    return;
  }

  while(iter->next!=NULL)
  {
    iter->entry->print(output);
    iter=iter->next;
  }
}




void list::ordered_print(FILE* output, const term_ordering& w) const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::print(const term_ordering&) const:\n"
      "cannot print corrupt list"<<endl;
    fprintf(output,"\nWARNING: void list::print(const term_ordering&) const:\n"
      "cannot print corrupt list\n");
    return;
  }

  list aux;

  while(iter->next!=NULL)
  {
    aux._ordered_insert(*(iter->entry),w);
    iter=iter->next;
  }

  aux.print(output);

  // delete aux, but only the element structs, not the binomials
  // (these are still stored in the actual list!)

#ifdef DL_LIST

  iter=aux.start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  iter=aux.start;

#endif

  while(iter->next!=NULL)
    // end_dummy not reached
  {
    element* aux2=iter->next;
    iter->next=iter->next->next;
    delete aux2;
  }

  // the dummy elements are deleted by the destructor
}




void list::print(ofstream& output) const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::print(ofstream&) const:\n"
      "cannot print corrupt list"<<endl;
    output<<"\nWARNING: void list::print(oftream&) const:\ncannot print "
      "corrupt list"<<endl;
    return;
  }

  while(iter->next!=NULL)
  {
    iter->entry->print(output);
    iter=iter->next;
  }
}




void list::ordered_print(ofstream& output, const term_ordering& w) const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::ordered_print(const term_ordering&) const:\n"
      "cannot print corrupt list"<<endl;
    output<<"\nWARNING: void list::ordered_print(const term_ordering&) const:"
      "\n"
      "cannot print corrupt list\n"<<endl;
    return;
  }

  list aux;

  while(iter->next!=NULL)
  {
    aux._ordered_insert(*(iter->entry),w);
    iter=iter->next;
  }

  aux.print(output);

  // delete aux, but only the element structs, not the binomials
  // (these are still stored in the actual list!)

#ifdef DL_LIST

  iter=(aux.start)->next;

#endif  // DL_LIST

#ifdef SL_LIST


  iter=aux.start;

#endif


  while((iter->next)!=NULL)
  {
    element* aux1=iter->next;
    iter->next=iter->next->next;
    delete aux1;
  }

  // the dummy elements are deleted by the destructor
}




void list::format_print(ofstream& output) const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::print(ofstream&) const:\n"
      "cannot print corrupt list"<<endl;
    output<<"\nWARNING: void list::print(oftream&) const:\ncannot print "
      "corrupt list"<<endl;
    return;
  }

  while(iter->next!=NULL)
  {
    iter->entry->format_print(output);
    iter=iter->next;
  }
}




void list::ordered_format_print(ofstream& output, const term_ordering& w) const
{

#ifdef DL_LIST

  element* iter=start->next;

#endif  // DL_LIST

#ifdef SL_LIST

  element *iter=start;

#endif  // SL_LIST

  if(iter==NULL)
  {
    cerr<<"\nWARNING: void list::ordered_format_print(const term_ordering&) "
      "const:\n"
      "cannot print corrupt list"<<endl;
    output<<"\nWARNING: void list::ordered_format_print(const term_ordering&) "
      "const:\n"
      "cannot print corrupt list\n"<<endl;
    return;
  }

  list aux;

  while(iter->next!=NULL)
  {
    aux._ordered_insert(*(iter->entry),w);
    iter=iter->next;
  }

  aux.format_print(output);

  // delete aux, but only the element structs, not the binomials
  // (these are still stored in the actual list!)

#ifdef DL_LIST

  iter=(aux.start)->next;

#endif  // DL_LIST

#ifdef SL_LIST


  iter=aux.start;

#endif


  while((iter->next)!=NULL)
  {
    element* aux1=iter->next;
    iter->next=iter->next->next;
    delete aux1;
  }

  // the dummy elements are deleted by the destructor
}





/////////////////////////////////////////////////////////////////////////////
////////////////////// class list_iterator //////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// implementation of class list_iterator
// Most of these function can be inlined. I have tried this and not improved
// the performance. Perhaps the compiler does this automatically...




///////////////////////// constructors and destructor ///////////////////////




list_iterator::list_iterator()
{
  actual=NULL;
}



list_iterator::list_iterator(list& l)
{

#ifdef DL_LIST

  actual=(l.start)->next;

#endif // DL_LIST

#ifdef SL_LIST

  actual=l.start;

#endif // SL_LIST

}



list_iterator::list_iterator(list_iterator& iter)
{
  actual=iter.actual;
}



list_iterator::~list_iterator()
{
}




/////////////////// object information /////////////////////////////////////




BOOLEAN list_iterator::element_is_marked_done() const
{
  return(actual->done);
}


BOOLEAN list_iterator::element_is_marked_head_reduced() const
{
  return(actual->head_reduced);
}


BOOLEAN list_iterator::is_at_end() const
{
  if(actual==NULL)
  // actual references no list
    return(TRUE);

  if(actual->next==NULL)
  // actual references dummy element
    return(TRUE);

  // actual references a real element
  return(FALSE);
}




////////////////////////// assignment ///////////////////////////////////////




list_iterator& list_iterator::set_to_list(const list& l)
{

#ifdef DL_LIST

  actual=(l.start)->next;

#endif  // DL_LIST

#ifdef SL_LIST

  actual=l.start;

#endif  // SL_LIST

  return *this;
}



list_iterator& list_iterator::operator=(const list_iterator& iter)
{
  if((&iter)!=this)
    actual=iter.actual;
  return *this;
}



list_iterator& list_iterator::next()
{
  actual=actual->next;
  return *this;
}




/////////////////// comparison ////////////////////////////////////////////




int list_iterator::operator==(const list_iterator& iter) const
{
  return(actual==iter.actual);
}


int list_iterator::operator!=(const list_iterator& iter) const
{
  return(!(actual==iter.actual));
}


int list_iterator::next_is(const list_iterator& iter) const
{
  return ((actual->next)==iter.actual);
}




////////////// manipulation of list elements //////////////////////////////

// For a better overview, the code of the delete- and extract-routine is
// separated for simply and doubly linked lists.




binomial& list_iterator::get_element()
{
  return(*(actual->entry));
}




#ifdef DL_LIST




list_iterator& list_iterator::delete_element()
{
  binomial* aux1=actual->entry;
  element*  aux2=actual;

  actual->previous->next=actual->next;
  actual->next->previous=actual->previous;

  actual=actual->next;

  delete aux1;
  delete aux2;
  return *this;
}




list_iterator& list_iterator::extract_element()
{
  element* aux=actual;

  actual->previous->next=actual->next;
  actual->next->previous=actual->previous;

  actual=actual->next;

  delete aux;
  return *this;
}




#endif  // DL_LIST




#ifdef SL_LIST

// When deleting or extracting an element of a simply linked list, the
// next-pointer of the previous element cannot be manipulated (is unkonwn!).
// So deletion must be done by copying the next element to the actual position
// and then deleting the original. Notice that only pointers are copies, never
// binomials.




list_iterator& list_iterator::delete_element()
{
  binomial* aux1=actual->entry;
  element* aux2=actual->next;

  actual->done=actual->next->done;
  actual->head_reduced=actual->next->head_reduced;
  actual->entry=actual->next->entry;
  actual->next=actual->next->next;

  delete aux1;
  delete aux2;

  return *this;
}




list_iterator& list_iterator::extract_element()
{
  element* aux=actual->next;

  actual->done=actual->next->done;
  actual->head_reduced=actual->next->head_reduced;
  actual->entry=actual->next->entry;
  actual->next=actual->next->next;

  delete aux;
  return *this;
}
#endif  // SL_LIST

list_iterator& list_iterator::mark_element_done()
{
  actual->done=TRUE;
  return *this;
}



list_iterator& list_iterator::mark_element_undone()
{
  actual->done=FALSE;
  return *this;
}



list_iterator& list_iterator::mark_element_head_reduced()
{
  actual->head_reduced=TRUE;
  return *this;
}



list_iterator& list_iterator::mark_element_head_unreduced()
{
  actual->head_reduced=FALSE;
  return *this;
}
#endif  // LIST_CC
