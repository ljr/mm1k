/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

heap.cpp
-------------------------------------------------------------------------------
This class, Heap, is one of a dozen or so derived classes of abstract base
class Vlist.  Class dfn in vlist.h.
-----------------------------------------------------------------------------*/


#include "queuing.h"


/*=============================================================================
-----------------------------------------------------------------------------*/

Heap::Heap (int cap)
  {
  size = hi_wtr_mark = 0;
  capacity = cap;
  elms = new Event [capacity + 1];
  cout << "Constructed Heap, capacity " << capacity << ".\n";
  }



/*=============================================================================
Put the given item in the heap structure.  Never uses element [0].
-----------------------------------------------------------------------------*/

void Heap::Insert (Event new_elm, int where)
  {
  if (where != TIME_KEY)
    {
    Future::Display ("Error in Heap::Insert()");
    InsertWhere here = InsertWhere (where);
    cout << "Heap supports Insert on TIME_KEY but not on "
         << InsertionPoint (here) << '\n';
    ErrXit (9101, "bad where");
    }
  size++;
  if (size > capacity) ErrXit (9102, "Heap full");
  if (size > hi_wtr_mark) hi_wtr_mark = size;

  elms [size] = new_elm;
  if (size > 1)
    {
    int child = size;
    int parent = child / 2;
    while ( (elms[parent].Time() > elms[child].Time() )  &&  (child > 1) )
      {
      HeapSwap (parent, child);
      child = parent;
      if (child > 1) parent = child / 2;
      } // end while
    } // end if
  } // end fn



/*=============================================================================
Heaps are best at delete-min: remove element 1 & fix up the heap.  But we also
support delete-arbitrary, to remove a pacific item from heap.
-----------------------------------------------------------------------------*/

Event Heap::Remove (int where, int token_or_event_id, void (*func) () )
  {
  Event eee;
  if (Empty () )
    {
    CannotRemoveFromEmpty (where);
    eee.EventId (NOT_FOUND);
    return eee;
    }

  int ii;
  int here = 0;
  switch (where)
    {
    case FROM_FRONT:
      here = 1;
      break;
    case MATCHING_TOKEN_ID:
      for (ii=1; here == 0 && ii <= size; ii++) // find key, could be anywhere
        if (elms[ii].Id() == token_or_event_id)
          here = ii;
      break; // end case
    case MATCHING_EVENT_ID:
      for (ii=1; here == 0 && ii <= size; ii++) // find key, could be anywhere
        if (elms[ii].EventId() == token_or_event_id)
          here = ii;
      break; // end case
    case MATCHING_EVENT_ROUTINE:
      for (ii=1; here == 0 && ii <= size; ii++) // find key, could be anywhere
        if (elms[ii].fn == func)
          here = ii;
      break; // end case
    default: ErrXit (9103, "bad where");
    }

  if (here == 0) ErrXit (9104, "Not found");
  eee = elms [here];          // copy out heap item being deleted
  elms [here] = elms [size--];       // fill empty slot, shrink heap
  if (here > 1) HeapUpAdjust (here); // re-estab heap invariant
  HeapDownAdjust (here);
  return eee;
  }


/*=============================================================================
potected mbr method
Adjust the heap going down from the given location.
loop copied from old heap_delete routine
-----------------------------------------------------------------------------*/

void Heap::HeapDownAdjust (int parent)
  {
  int child;
  for (;;)
    {
    if (2 * parent > size)
      break;
    else
      child = 2 * parent;
    if (child + 1 <= size)
      if (elms[child+1].Time() < elms[child].Time() )
        child++;
    if (elms[parent].Time() < elms[child].Time() )
      break;
    HeapSwap (parent, child);
    parent = child;
    } // end while
  } // end fn



/*=============================================================================
potected mbr fn
Adjust the heap going up from the given location.
-----------------------------------------------------------------------------*/

void Heap::HeapUpAdjust (int child)
  {
  int parent = child / 2;
  while (parent > 0)
    {
    if (elms[parent].Time() < elms[child].Time() )
      break;
    HeapSwap (parent, child);
    child = parent;
    parent /= 2;
    }
  }



/*=============================================================================
potected mbr fn
Switch the positions of two items in the heap.
-----------------------------------------------------------------------------*/

void Heap::HeapSwap (int ii, int jj)
  {
  if (ii == jj) return;
  Event temp = elms [ii];
  elms [ii] = elms [jj];
  elms [jj] = temp;
  }



/*=============================================================================
Output the contents of the heap.
-----------------------------------------------------------------------------*/

void Heap::Display (char * descr)
  {
  cout << "Display Heap '" << descr << "' queue";
  Display (1);
  cout << "\n------------------------------------------------------------\n";
  }

void Heap::Display (int mode)
  {
  if (mode) ;

  double now = Future::SimTime();
  ShowSizeEtc (now);
  if (Empty () ) return;

  int width = SetBoxWidth (now);

  int ii;
  for (ii=0; ii < 6; ii++) cout << ' ';
  int node;
  for (node = 1; node <= size; node++) BoxEdge (width);
  cout << "\nToken ";
  cout. precision(1);

  for (node = 1; node <= size; node++)
    cout << "  |" << setw(width) << elms[node].Id() << '|';

  cout << "\nTime  ";
  for (node = 1; node <= size; node++)
    {
    cout << "<=|" << setw(width) << elms[node].Time() << '|';
    }
  cout << "\nEvent ";

  char stg [TINY_BFR_SIZ+1];
  if (width >= TINY_BFR_SIZ) ErrXit (9105, "width too big");

  for (node = 1; node <= size; node++)
    {
    cout << "  |";
    int evnt_id = elms[node].EventId();
    if (evnt_id < 0)
      {
      strncpy(stg, elms[node].FnDescr(), width);
      stg [width] = 0;
      cout << stg;
      }
    else
      cout << setw(width) << evnt_id;
    cout << '|';
    }
  cout << "\n      ";
  for (node = 1; node <= size; node++) BoxEdge (width);
  cout << '\n';
  cout. flush();
  }

/*=============================================================================
-----------------------------------------------------------------------------*/

// end heap.cpp

