/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

event.cpp
******************************************************************************/

#include "queuing.h"

// Static variables:
    unsigned Event::created = 0;
    unsigned Event::destroyed = 0;

/******************************************************************************
Constructor
-------------------------------------------------------------------------------
All parameters are optional.  See event.h.
******************************************************************************/

Event::Event (int et, double ttt, int ppp)

//  : Token () // first invoke constructor of base class

  {
  serialnumber = ++created;
  time = ttt;
  event_id = et;
  priority = ppp;
  fn = NULL;
  fn_descr [0] = 0;

  }//end fn

Event::Event (void (*func) (), double ttt, int ppp, char * fds)
  {
  serialnumber = ++created;
  time = ttt;
  event_id = NOT_FOUND;
  priority = ppp;
  fn = func;

  if (fds)
    {
    strncpy(fn_descr, fds, ES_STG_LEN);
    fn_descr [ES_STG_LEN] = 0;
    }
  else
    fn_descr [0] = 0;

  }



Event::Event (void (*func) (), char * fds)
  {
  serialnumber = ++created;
  time = 0.0;
  event_id = NOT_FOUND;
  priority = 0;
  fn = func;

  if (fds)
    {
    strncpy(fn_descr, fds, ES_STG_LEN);
    fn_descr [ES_STG_LEN] = 0;
    }
  else
    fn_descr [0] = 0;

  }



/******************************************************************************
Destructor
******************************************************************************/

Event::~Event ()
  {
  destroyed++;
  }//end fn

// end event.cpp

