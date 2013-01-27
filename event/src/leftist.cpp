/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

leftist.cpp
-------------------------------------------------------------------------------
This class, Leftist, is one of a dozen or so derived classes of abstract base
class Vlist.  Class dfn in vlist.h.
-----------------------------------------------------------------------------*/


#include "queuing.h"



/*=============================================================================
Constructor.
-----------------------------------------------------------------------------*/

Leftist::Leftist (Lnode * rrr)
  {
  i_am = LEFTIST;
  root = rrr;
  if (root)
    size = hi_wtr_mark = 1;
  else
    size = hi_wtr_mark = 0;
  }



/*=============================================================================
Package new node into a sapling (a little tree).  Merge will do the work.
-----------------------------------------------------------------------------*/

void Leftist::Insert (Event item, int where)
  {
  if (where != TIME_KEY) ErrXit (9111, "bad where");
  Lnode * new_node = new Lnode();
  (Event &) (* new_node) = item;
  Leftist * new_tree = new Leftist (new_node);
  Merge (new_tree);
  size ++;
  if (size > hi_wtr_mark) hi_wtr_mark = size;
  }



/*=============================================================================
Potected fn.  Merge "this" with tree2.  Merged tree will be "this".  Sitcan
tree #2.
-----------------------------------------------------------------------------*/

void Leftist::Merge (Leftist * tree2)
  {
  Lnode * ptr1 = root;
  Lnode * ptr2 = tree2->root;
  if ( ! ptr1 && ! ptr2) { delete tree2; return; }

  /*  merge the two trees, inserting upward right pointers  */

  Lnode * rrr = NULL;
  int dist1;
  do_until_break
    {
    if (ptr2 == NULL) {              dist1 = ptr1->dist; break; }
    if (ptr1 == NULL) { ptr1 = ptr2; dist1 = ptr1->dist; break; }

    if (ptr1->Time() <= ptr2->Time())
      { Lnode * t = ptr1->rson; ptr1->rson = rrr; rrr = ptr1; ptr1 = t; }
    else
      { Lnode * t = ptr2->rson; ptr2->rson = rrr; rrr = ptr2; ptr2 = t; }
    } // end while

  /*  convert the tree to normal form  */

  int distance;
  while (rrr)
    {
    if (rrr->lson == NULL) distance = 0;
    else distance = rrr->lson->dist;
    ptr2 = rrr->rson;
    if (distance < dist1)
      {
      dist1 = distance + 1;
      rrr->rson = rrr->lson;
      rrr->lson = ptr1;
      }
    else
      {
      dist1++;
      rrr->rson = ptr1;
      }
    rrr->dist = dist1;
    ptr1 = rrr;
    rrr = ptr2;
    } // end while
  root = ptr1;
  delete tree2;
  }



/*=============================================================================
Depending on where, removes root or removes node whose token matches povided
value or func.  Matching the token id supports CancelToken() & Preempt().
-----------------------------------------------------------------------------*/

Event Leftist::Remove (int where, int value, void (*func) () )
  {
  Event eee;
  if (Empty () )
    {
    CannotRemoveFromEmpty (where);
    eee.EventId (NOT_FOUND);
    return eee;
    }
  if ( ! root) ErrXit (9112, "NULL root");

  Boolean from_front = NO;
  switch (where)
    {
    case FROM_FRONT:
      from_front = YES; break;
    case MATCHING_TOKEN_ID:
    case MATCHING_EVENT_ID:
    case MATCHING_EVENT_ROUTINE:
      Search (root, where, value, func);
      if (root->flag == IN_THIS_NODE)
        { from_front = YES; break; }
      eee = ExtractAndAdjust (value, root);
      size--;
      return eee;
    default: ErrXit (9113, "Bad where");
    } // end switch

  if (from_front)
    {
    eee = (Event &) (* root);
    Lnode * rptr = root->rson;
    Lnode * sitcan = root;
    root = root->lson;
    delete sitcan;
    int sz = --size;

    if ( ! root)   // no left child --> no merge
      root = rptr;
    else           // no right child --> no merge
    if (rptr)
      {
      Leftist * new_tree = new Leftist (rptr);
      Merge (new_tree);
      }

    size = sz;
    }
  return eee;
  }



/*=============================================================================
Potected mbr method.  Locate a node matching a key in a leftist tree.
Ultimately sets a flag saying to turn left or right to reach it.
-----------------------------------------------------------------------------*/

Boolean Leftist::Search (Lnode * ptr, int which, int id, void (*func)() )
  {
  if ( ! ptr)                          return NO;

  Boolean is_it_here = NO;
  switch (which)
    {
    case MATCHING_TOKEN_ID:      is_it_here = (ptr->Id()      == id);   break;
    case MATCHING_EVENT_ID:      is_it_here = (ptr->EventId() == id);   break;
    case MATCHING_EVENT_ROUTINE: is_it_here = (ptr->fn        == func); break;
    default: ErrXit (9114, "Bad which");
    }

  if (is_it_here)
    { ptr->flag = IN_THIS_NODE;        return YES; }

  if (Search (ptr->lson, which, id, func) == YES)
    { ptr->flag = TURN_LEFT;           return YES; }

  if (Search (ptr->rson, which, id, func) == YES)
    { ptr->flag = TURN_RIGHT;          return YES; }

      ptr->flag = NOT_IN_THIS_SUBTREE; return NO;
  }



/*=============================================================================
Potected member method.  Handles the process of adjusting a tree when an
arbitrary node OTHER THAN ROOT is removed.  First, it adjusts the tree below
it.  Then, it swaps children if necessary according to the distances.  Finally,
it sets its own distance, based on children.  It's done this way because of the
need to fix up a trailing ptr:  parent of removed node gets a change in child.
-----------------------------------------------------------------------------*/

Event Leftist::ExtractAndAdjust (int value, Lnode * ptr)
  {
  if ( ! ptr) ErrXit (9115, "NULL ptr");
  Event eee;
  switch (ptr->flag)
    {
    case NOT_IN_THIS_SUBTREE:
      cout << "I5915 Search failed\n";
      eee.EventId (ES_NO_EVENT);
      return eee;
    case IN_THIS_NODE:
      ErrXit (9116, "IN_THIS_NODE");
    case TURN_LEFT:
      {
      if ( ! ptr->lson) ErrXit (9117, "NULL ptr->lson");
      if (ptr->lson->flag == IN_THIS_NODE)
        {
        Leftist * new_tree = new Leftist (ptr->lson);
        eee = new_tree->Remove (FROM_FRONT);
        ptr->lson = new_tree->root;
        delete new_tree;
        }
      else
        eee = ExtractAndAdjust (value, ptr->lson);

      if (Lnode::Dista (ptr->lson) < Lnode::Dista (ptr->rson) )
        {
        Lnode * temp = ptr->rson;
        ptr->rson = ptr->lson;
        ptr->lson = temp;
        }
      break;
      }

    case TURN_RIGHT: // same idea, but on right child no swapping necessary
      {
      if ( ! ptr->rson) ErrXit (9118, "NULL ptr->rson");
      if (ptr->rson->flag == IN_THIS_NODE)
        {
        Leftist * new_tree = new Leftist (ptr->rson);
        eee = new_tree->Remove (FROM_FRONT);
        ptr->rson = new_tree->root;
        delete new_tree;
        }
      else
        eee = ExtractAndAdjust (value, ptr->rson);
      break;
      }
    default: ErrXit (9119, "Bad ptr->flag");
    } // end switch

  ptr->dist = Lnode::Dista (ptr->rson) + 1;
  return eee;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Leftist::Display (int mode)
  {
  if (mode) ;

  double now = Future::SimTime();
  ShowSizeEtc (now);
  if (root) root->Picture (1, "");
  }



/*============================================================================
Potected recursive mbr method.  visit root, traverse lson, traverse rson.
----------------------------------------------------------------------------*/

void Leftist::Lnode::Picture (int level, char * stg)
  {

  int ii;
  for (ii=1; ii < level; ii++) cout << stg;
  cout << "Token: " << Id() << ", "
       << "Time: " << time << ", "
       << "Event: ";
  char egg [TINY_BFR_SIZ+1];
  if (event_id >= 0)
    cout << event_id;
  else
    {
    strncpy(egg, fn_descr, TINY_BFR_SIZ); egg [TINY_BFR_SIZ] = 0;
    cout << "'" << egg << "'";
    }
  cout << "\n";

  if (lson)
    lson->Picture (level+1, "'' ");
  if (rson)
    rson->Picture (level+1, "`` ");
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Leftist::Display (char * descr)
  {
  if (descr) ;
  cout << "I1085 Not implemented\n";
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Leftist::Tester ()
  {
  cout << "Welcome to Leftist::Tester\n";
  init_simpack(LEFTIST);
  Future::Interactive (YES);
  Future::SnapshotFlag (ON);
  Token cow;  char descr [2]; strcpy(descr, "@");
  int arf;
  for (arf=0; arf < 16; arf++)
    {
    cow.Id (arf); descr [0]++;
    Future::Schedule (TestEvent, arf, cow, descr);
    }
  Estatus es;
  cout << '\n';
  Future::CancelToken (7);
  cout << '\n';
  do_until_break
    {
    es = Future::NextEvent();
    if (es.rtncod != ES_OK) break;
    cout << es.descr;
    }
  cout << "\nHave a nice day\n";
  exit(0);
  }



/*=============================================================================
static protected method, used with Tester above, just povides address, not
called.
-----------------------------------------------------------------------------*/

void Leftist::TestEvent (void)
  {
  cout << "Leftist test event routine\n";
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Leftist::Clear ()
  {
  if ( ! root) return;

  Lnode::Clear (root->lson);
  Lnode::Clear (root->rson);
  delete root;
  }




/*=============================================================================
static so it works with NULL.
-----------------------------------------------------------------------------*/

void Leftist::Lnode::Clear (Lnode * ptr)
  {
  if ( ! ptr) return;
  Clear (ptr->lson);
  Clear (ptr->rson);
  }




/*=============================================================================
-----------------------------------------------------------------------------*/

// end leftist.cpp

