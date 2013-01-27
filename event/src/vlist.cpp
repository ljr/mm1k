/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

vlist.cpp
-----------------------------------------------------------------------------*/

// A great reference is Borland's World of C++, Lesson 20, VPET2.CPP:
// A menagerie of pets, (C) 1991 Borland


#include "queuing.h"


/*=============================================================================
-----------------------------------------------------------------------------*/

Vlist * Vlist::Constructor (Lspecies fel_type)
  {
  switch (fel_type)
    {
    case LINKED:   return new Linked();
    case HEAP:     return new Heap ();
    case CALENDAR: return new Calendar(150, YES);
    case HENRIK:   return new Henrik();
    case INOMIAL:  return new Inomial();
    case LEFTIST:  return new Leftist();
    case PAGODA:   return new Pagoda();
    case PAIR:     return new Pair();
    case SKEWDN:   return new Skewdn();
    case SKEWUP:   return new Skewup();
    case SPLAY:    return new Splay();
    case TWOLIST:  return new Twolist();
    default:       ErrXit (9181, "Bad event list type");
    }
  return NULL; // pacify compiler
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

char * Vlist::ListType ()
  {
  switch (i_am)
    {
    case LINKED  : return "Linked";
    case HEAP    : return "Heap";
    case LEFTIST : return "Leftist";
    case CALENDAR: return "Calendar";
    }
  return "?";
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

char * Vlist::InsertionPoint (InsertWhere here)
  {
  switch (here)
    {
    case TIME_KEY           : return "TIME_KEY";
    case AHEAD_PRIORITY_KEY : return "AHEAD_PRIORITY_KEY";
    case BEHIND_PRIORITY_KEY: return "BEHIND_PRIORITY_KEY";
    case AT_FRONT           : return "AT_FRONT";
    case AT_REAR            : return "AT_REAR";
    }
  return "?";
  }



/*=============================================================================
potected mbr method.
-----------------------------------------------------------------------------*/

void Vlist::CannotRemoveFromEmpty (int where)
  {
  switch (where)
    {
    case FROM_FRONT:
      ErrXit (9182, "Empty list");
    case MATCHING_TOKEN_ID:
    case MATCHING_EVENT_ID:
    case MATCHING_EVENT_ROUTINE:
    case MATCHING_CLASS:
      return;
    }
  ErrXit (9183, "bad where");
  }


/*=============================================================================
potected mbr method.
-----------------------------------------------------------------------------*/

void Vlist::BoxEdge (int width)
  {
  cout << "  +";
  int ii;
  for (ii=0; ii < width; ii++) cout << '-';
  cout << '+';
  }



/*=============================================================================
potected mbr method.
-----------------------------------------------------------------------------*/

void Vlist::ShowSizeEtc (double now)
  {
  cout. setf(ios::fixed, ios::floatfield);
  cout. setf(ios::showpoint);
  cout. precision(1);
  cout << " contains " << size << " elements "
       << "at time: " << now << '\n'
       << "It has been as large as " << hi_wtr_mark << " elements so far.\n";
  }



/*=============================================================================
potected mbr method.
-----------------------------------------------------------------------------*/

#include <math.h> // pow()

int Vlist::SetBoxWidth (double now)
  {
  int ii;
  for (ii = 3; ii < 12; ii++)
  if (now < pow(10,ii) )
    break;
  return ii + 2;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

// end vlist.cpp

