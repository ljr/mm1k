/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

capifut.cpp
-------------------------------------------------------------------------------
simpack c-language application progrmamming interface
Part 1: future-related.
-----------------------------------------------------------------------------*/


#include "queuing.h"


/************************************************************************/
/*                                                                      */
/* init_simpack:                                                        */
/*      Initialize data structures and variables for simulation.        */
/*                                                                      */
/************************************************************************/
int init_simpack (int p1)
  {
  Lspecies fel_type = Lspecies (p1 & FEL_TYPE_MASK);
  Boolean rmv_dup     = p1 & REMOVE_DUP_MASK;
  Boolean reseed      = p1 & RESEED_MASK;
  new Future (fel_type, NULL, "", NO_FN, NO, NO, rmv_dup, reseed);
  cout. flush();
  return 0; // pacify compiler
  }



/************************************************************************/
/*                                                                      */
/* trace_visual:                                                        */
/*      Complete Graphical Trace of Queuing Simulation.                 */
/*                                                                      */
/************************************************************************/
int trace_visual (int on_or_off)
  {
  Future::SnapshotFlag (ON);
  Future::Interactive (on_or_off);
  return 0; // pacify compiler
  }



/************************************************************************/
/*                                                                      */
/* schedule:                                                            */
/* Schedule a given event in a given time.                              */
/*                                                                      */
/************************************************************************/
int schedule (int event_id, float inter_time, TOKEN old_style_tkn)
  {
  Token new_style_tkn = ConvertTOKENtoToken (old_style_tkn);
  Future::Schedule (event_id, inter_time, new_style_tkn);
  return 0;
  }



/************************************************************************/
/*                                                                      */
/* next_event:                                                          */
/* Cause an event to occur and return it in the given pointers.         */
/*                                                                      */
/************************************************************************/
int next_event (int * event_id, TOKEN * old_style_token_ptr)
  {
  Estatus es = Future::NextEvent ();
  (* event_id) = es.event_id;
  Token tkn    = es.token;

  switch (es.rtncod)
    {
    case ES_OK:
      {
      // copy contents of token to old style token
      int ii;
      for (ii=0; ii < MAX_NUM_ATTR; ii++)
        old_style_token_ptr->attr[ii] = tkn.TokenRattr (ii);
      break;
      }
    case ES_NO_EVENT:
      {
      cout << "I1521 Event list is empty\n";
      Future::ReportStats();
      break;
      }
    }

  // The next_event() in old simpack had a problem: if the FEL were ever
  // empty when next_event() were called, next_event() would blithely
  // return to the application, giving NO indication of error.  In my
  // experiments, i was reliably able to produce abnormal pgm termination
  // (segmentation fault on unix system) by creating this situation.  To
  // mitigate this problem for those who would continue to use the old-style
  // API, we send to stdout a msg that FEL was empty, generate
  // final stats for the run, and return with a return code that can
  // be tested to sense the condition, and permit the application pgm to
  // exit, or to do something else, as appropriate.  The old next_event() didn't
  // return anything, but its definition was a function that returns int.
  // I have exploited this to return something meaningful that can be used
  // in the empty-FEL situation just described.

  // An alternative was to error exit above, but in the spirit of C and C++,
  // i decided to err on the side of giving the application pgmr more rope
  // (i mean flexibility).

  // BTW, to create the empty FEL situation you count arrivals
  // and stop generating them after a certain number have been generated.
  // All remaining events then wend their way thru the system, and
  // if the last event departs before the "main while loop" exits on
  // sim_time > whatever, the FEL-empty situation occurs.

  return es.rtncod; // eg, ES_OK is 0 (see enum type in future.h)
  }



/************************************************************************/
/*                                                                      */
/* time:                                                                */
/*      Provide the current simulation time.                            */
/*                                                                      */
/************************************************************************/
float sim_time ()
  {
  return Future::SimTime();
  }



/************************************************************************/
/*                                                                      */
/* update_arrivals:                                                     */
/*      Update # of arrivals into a system.                             */
/*                                                                      */
/************************************************************************/
int update_arrivals ()
  {
  return Future::UpdateArrivals ();
  }



/************************************************************************/
/*                                                                      */
/* update_completions:                                                  */
/*      Update # of completions out of a system.                        */
/*                                                                      */
/************************************************************************/
int update_completions ()
  {
  return Future::UpdateDepartures ();
  }



/************************************************************************/
/*                                                                      */
/* trace_eventlist:                                                     */
/*      Output information about the eventlist.                         */
/*                                                                      */
/************************************************************************/
int trace_eventlist ()
  {
  Future::DisplayFEL (1); // mode 2 is for trace_eventlist() format
  return 0;
  }



/************************************************************************/
/*                                                                      */
/* print_heap:                                                          */
/*      Output the contents of the heap.                                */
/*                                                                      */
/************************************************************************/
int print_heap(void)
  {
  if (Future::FELspecies () != HEAP)
    ErrXit (9021, "FEL not HEAP");
  Future::DisplayFEL (1);
  return 0;
  }



/************************************************************************/
/*                                                                      */
/* trace_eventlist_size:                                                */
/*      Output the size of the event list.                              */
/*                                                                      */
/************************************************************************/
int trace_eventlist_size ()
  {
  return Future::EventListSize ();
  }



/************************************************************************/
/*                                                                      */
/* cancel_event:                                                        */
/*      Remove the given event from the queue.                          */
/*                                                                      */
/************************************************************************/
int cancel_event (int event_id)
  {
  Estatus es = Future::CancelEvent (event_id);
  int event_id_returned = es.event_id;
  if (event_id_returned != event_id) return NOT_FOUND;
  return es.token.Id();
  }



/************************************************************************/
/*                                                                      */
/* cancel_token:                                                        */
/*      Remove the given token from consideration.                      */
/*                                                                      */
/************************************************************************/
int cancel_token (TOKEN old_style_token)
  {
  int tkn_id = int (old_style_token.attr [0]);
  Estatus es = Future::CancelToken (tkn_id);
  return es.event_id;
  }



/************************************************************************/
/*                                                                      */
/* report_stats:                                                        */
/*      Output all relavent simulation statistics.                      */
/*                                                                      */
/************************************************************************/
int report_stats ()
  {
  Future::ReportStats ();
  return 0; // pacify compiler
  }



void end_simpack()
  {
  cout << "WARNING: end_simpack() not supported in this release\n";
  Future * future_ptr = Future::future_ptr;
  if ( ! future_ptr) ErrXit (9031, "NULL future");
  delete future_ptr;
  Facility::RemoveAll ();
  }



// end capifut.cpp

