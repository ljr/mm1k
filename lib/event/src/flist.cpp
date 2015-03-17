/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

flist.cpp
-----------------------------------------------------------------------------*/


#include "queuing.h"


/*=============================================================================
-----------------------------------------------------------------------------*/

Flist::Flist ()
  {
  front = NULL;
  rear = NULL;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Flist::Append (Facility * fff)
  {
  if (rear == NULL)
    front = rear = fff;
  else
    {
    rear->succ = fff;
    fff->pred = rear;
    rear = fff;
    }
  }



/*=============================================================================
Remove a facility from the doubly-linked facility list.  This is called from
the Facility destructor.
-----------------------------------------------------------------------------*/

void Flist::Unlink (Facility * fff)
  {
  if (fff->succ)
    fff->succ->pred = fff->pred;
  else
    rear = fff->pred;

  if (fff->pred)
    fff->pred->succ = fff->succ;
  else
    front = fff->succ;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

// end flist.cpp

