/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

future.cpp
-----------------------------------------------------------------------------*/

#include "queuing.h"
#include <time.h>  // time(), ctime()
#include <new>   // set_new_handler()

#if TURBOC
  #include <conio.h> // clrscr(), gotoxy()
#endif

// i don't want this fn surfaced to the application, so its proto decl is here
// rather than in "queuing.h".  The fn is defined in "random.cpp" and called
// only from "future.cpp".

    void ResetRNstream (void);



/*=============================================================================
Static data members of class Future:
-----------------------------------------------------------------------------*/

    Future *   Future::future_ptr        = NULL;      // new
    Lspecies   Future::event_list_type   = NO_FLAVOR; // orig int
    Vlist *    Future::event_list        = NULL;      // orig LIST
    Boolean    Future::remove_duplicates = NO;        // orig int
    Boolean    Future::ranmark           = NO;        // orig int
    Boolean    Future::snapshot_flag     = OFF;       // orig int, set by trace_visual()
    int        Future::rn_stream         = 1;         // orig decld static in init_simpack()
    int        Future::arrivals          = 0;         // orig int
    int        Future::departures        = 0;         // orig int
    int        Future::tokens_in_system  = 0;         // orig int
    int        Future::current_event     = 0;         // orig int
    double     Future::current_time      = 0.0;       // orig float
    double     Future::last_event_time   = 0.0;       // orig float uninit, set by next_event(), possible undef
    double     Future::total_token_time  = 0.0;       // orig float
    Event      Future::executing_event;               // new
    Ioattr *   Future::cin_attr          = NULL;      // new
    Ioattr *   Future::cout_attr         = NULL;      // new
    Boolean    Future::interactive       = NO;        // was #defined (sort of)



/*=============================================================================
Constructor
-----------------------------------------------------------------------------*/

Future::Future (
                Lspecies fff,
                void (*GeneratorFn) (),
                char * fn_descr,
                SchedOrExec s_or_e,
                Boolean want_snapshots,
                Boolean inter_activ,
                Boolean rmv_dup,
                Boolean reseed
               )
  {
  if (future_ptr) ErrXit (9061, "future_ptr not NULL");
  set_new_handler (DynAllocFail);
  future_ptr = this;

  long flags     = cin.flags();
  int  width     = cin.width();
  int  precision = cin.precision();
  cin_attr = new Ioattr (flags, width, precision);

  flags     = cout.flags();
  width     = cout.width();
  precision = cout.precision();
  cout_attr = new Ioattr (flags, width, precision);

  time_t ttt;
  time(&ttt);
  cout << "Future constructed: " << ctime(&ttt); // \n built into stg
  cout. flush();

  rn_stream = 1;
  /// did NOT initialize token_list array (see notyet.001)
  event_list_type = fff;
  remove_duplicates = rmv_dup;
  ranmark = reseed;
  event_list = NULL;
  snapshot_flag = want_snapshots; // default is NO = ON DEMAND only
  interactive = inter_activ; // default is NO
  last_event_time = current_time = total_token_time = 0.0;
  arrivals = departures = tokens_in_system = 0;
  event_list = Vlist::Constructor (event_list_type);
  event_list->SetType(event_list_type);

  // if rn_stream == 0, return global value strm, initially 1.
  // elseif rn_stream <> 0 and out of range, then error.
  // else set global strm to rn_stream & return new value of strm.

  rn_stream = stream (rn_stream);

  // this next part is included so that one may run multiple passes with
  // different data structures for FEL but generate same random number stream
  // for easy comparison.  To use this, call constructor with reseed = YES, or
  // call init_simpack with flags or'd with RESEED.

  if (ranmark)
    ResetRNstream ();
  else
    rn_stream++;

  if (rn_stream > 15)
    rn_stream = 1;

  if (GeneratorFn)
    {
    switch (s_or_e)
      {
      case SCHEDULE_THIS_FN:
        {
        Token customer;
        Schedule (GeneratorFn, 0.0, customer, fn_descr);
        break;
        }
      case EXECUTE_THIS_FN:
        {
        Token customer;
        Schedule (GeneratorFn, 0.0, customer, fn_descr);
        Boolean done = NO;
        Estatus es;
        while ( ! done)
          {
          es = Dispatch();
          if (es.rtncod != ES_OK) { es.ES_DisplayReturnCode(); done = YES; }
          }
        break;
        }
      case NO_FN:
        break;
      default:
        ErrXit (9062, "Bad s_or_e in Future constructor");
      } // end switch
    } // end if GeneratorFn is non-null
  } // end fn



/*=============================================================================
These are alternative ways to turn several switches on or off, only valid AFTER
constructor is called; otherwise, values would be overwritten by constructor.

It's a matter of style whether to set the switches with the constructor or with
the fns below.  Some might not want umpteen parameters in the constructor, or
they might not want to fill in the intervening unneeded parameters with default
values, just to get positionally to where the later parameters appear.

You may also confuse parameters in the constructor -- there are 4 Booleans in a
row.  Using the functions below, there's no chance of confusion.

Each of these function names is overloaded.  The version which has no argument
returns the present setting of the capability (ON or OFF).
-----------------------------------------------------------------------------*/

void Future::RmvDup (Boolean OnOff)
  {
  if ( ! future_ptr) ErrXit (9063, "No future");
  remove_duplicates = OnOff;
  cout << "Remove-duplicates is " << (remove_duplicates ? "YES" : "NO") << '\n';
  }

Boolean Future::RmvDup ()
  {
  if ( ! future_ptr) ErrXit (9064, "No future");
  return remove_duplicates;
  }


void Future::Reseed (Boolean OnOff)
  {
  if ( ! future_ptr) ErrXit (9065, "No future");
  ranmark = OnOff;
  if (ranmark)
    ResetRNstream ();
  else
    rn_stream++;

  if (rn_stream > 15)
    rn_stream = 1;
  }

Boolean Future::Reseed ()
  {
  if ( ! future_ptr) ErrXit (9066, "No future");
  return ranmark;
  }

/*=============================================================================
There are two versions of SnapshotFlag() fn:  1-argument version SETS flag to
whatever the parameter is.  No-argument version REPORTS current state of flag.
-----------------------------------------------------------------------------*/

void Future::SnapshotFlag (Boolean OnOff)
  {
  if ( ! future_ptr) ErrXit (9067, "No future");
  snapshot_flag = OnOff;
  cout << "Snapshots " << (snapshot_flag ? "ALWAYS ON" : "ON DEMAND") << '\n';
  }

Boolean Future::SnapshotFlag ()
  {
  if ( ! future_ptr) ErrXit (9068, "No future");
  return snapshot_flag;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Future::Interactive (Boolean OnOff)
  {
  if ( ! future_ptr) ErrXit (9069, "No future");
  interactive = OnOff;
  #if UNIX
    if (interactive) initscr(); // initialize screens
  #endif
  cout << "Interactive mode is " << (interactive ? "ENABLED" : "DISABLED") << '\n';
  }

Boolean Future::Interactive ()
  {
  if ( ! future_ptr) ErrXit (9070, "No future");
  return interactive;
  }


/*=============================================================================
Destructor:  report statistics.  then reset static variables for a possible
nother run to follow.
-----------------------------------------------------------------------------*/

Future::~Future ()
  {
  if ( ! future_ptr) ErrXit (9071, "No future");
  cout << "WARNING: future destructor not supported in this release\n";
  ReportStats ();
  FELdrain ();
  delete cin_attr;   /// may want to set attributes back to defaults here first
  delete cout_attr;
  delete event_list; // virtual fn to delete FEL
  future_ptr        = NULL;
  event_list        = NULL;
  event_list_type   = NO_FLAVOR;
  remove_duplicates = NO;
  ranmark           = NO;
  snapshot_flag     = OFF;
  rn_stream         = 1;
  arrivals          = 0;
  departures        = 0;
  tokens_in_system  = 0;
  current_event     = 0;
  current_time      = 0.0;
  last_event_time   = 0.0;
  total_token_time  = 0.0;

  printf("destrutor\n");   
  /// think about the Future::Event static mbr: should it be cleared somehow?
  }




/*=============================================================================
Schedule an event = insert event into FEL.  i overloaded the fn name here.  The
first two methods are public, and the third is protected.  The third method is
only called from the first two.

The first version schedules an event-id, which is a subrange of integer.  Altho
this is primarily for compatibility with the old simpack API, it can also be
used by a C++ application pgmr to generate an event-id mode FEL.

The second version schedules an event-routine.  This is a new-style method,
which generates an event-routine mode FEL.

The third version is the protected common code, called ONLY by both the first
and second versions to get the insertion into the fel done, etc.

Trick:  overload constructor for class Event, so it builds the two kinds of
Event for us, while hiding the detail.
-----------------------------------------------------------------------------*/

void Future::Schedule (int event_id, double inter_time, Token tkn)
  {
  if ( ! future_ptr) ErrXit (9072, "No future");
  Event eee (event_id);
  Schedule (eee, inter_time, tkn);
  }

void Future::Schedule (void (*fn) (), double inter_time, Token tkn,
                       char * fn_descr)
  {
  if ( ! future_ptr) ErrXit (9073, "No future");
  Event eee (fn, fn_descr);
  Schedule (eee, inter_time, tkn);
  }

// potected static member method.
void Future::Schedule (Event eee, double inter_time, Token tkn)
  {
  eee.Time (current_time + inter_time);
  (Token &) eee = tkn; // token within event gets copy of tkn

  /// did NOT implement (see notyet.002):
  /* If in remove-duplicates mode then do not schedule this item if it */
  /* is already in the future event list.                              */

  int tkn_id = tkn.Id();
  if (snapshot_flag)
    FutMsg ("BEFORE SCHEDULE", eee.FnDescr(), inter_time, tkn_id);

  event_list->Insert (eee, TIME_KEY);

  if (snapshot_flag)
    FutMsg ("AFTER SCHEDULE", eee.FnDescr(), inter_time, tkn_id);

  } // end fn



/*=============================================================================
Dispatch removes the first event from the FEL and executes it.  This works ONLY
when FEL events specify event routines (and NOT when FEL events specify event
id's).  This is part of the move toward encapsulation, whose most visible
manifestation is the elimination of the "case" statement in the application
code.
-----------------------------------------------------------------------------*/

Estatus Future::Dispatch ()
  {
  if ( ! future_ptr) ErrXit (9074, "No future");
  Estatus es;

  if (event_list->Empty() )
    {
    es.rtncod = ES_NO_EVENT;
    return es;
    }

  executing_event = event_list->Remove (FROM_FRONT);
  void (*func) () = executing_event.fn;
  if (func == NULL)
    {
    es.rtncod = ES_NO_FN;
    return es;
    }

  strncpy(es.descr, executing_event.FnDescr(), ES_STG_LEN);
  es.descr [ES_STG_LEN] = 0;

  es.now = current_time = executing_event.Time();
  total_token_time += tokens_in_system * (current_time - last_event_time);
  last_event_time = current_time;

  (*func) ();

  es.rtncod = ES_OK;
  return es;
  }



/*=============================================================================
ExecutingEvent() is protected, the other 3 are public.  All return info
about the executing event.  The idea is that Event is not public.  Information
in Estatus comes some from event, and some from elsewhere, thus providing a
degree of freedom to allow Event to change while keeping Estatus the same or
consistent.
-----------------------------------------------------------------------------*/

Event Future::ExecutingEvent ()
  {
  if ( ! future_ptr) ErrXit (9075, "No future");
  return executing_event;
  }

Estatus Future::CurrentEvent ()
  {
  if ( ! future_ptr) ErrXit (9075, "No future");
  Estatus es;
  es.rtncod = ES_OK;
  es.now = current_time;
  es.fn = executing_event.fn;
  es.event_id = executing_event.EventId();
  strncpy(es.descr, executing_event.FnDescr(), ES_STG_LEN);
  es.descr[ES_STG_LEN] = 0;
  es.token = (Token &) executing_event;
  return es;
  }

int Future::CurrentEventId ()
  {
  if ( ! future_ptr) ErrXit (9076, "No future");
  return current_event;
  }

Token Future::CurrentToken ()
  {
  if ( ! future_ptr) ErrXit (9077, "No future");
  Token tkn = (Token &) executing_event;
  return tkn;
  }



void Future::FELinsert (Event eee, int where)
  {
  if ( ! future_ptr) ErrXit (9078, "No future");
  event_list->Insert (eee, where);
  }



Event Future::FELremove (int where, int tkn_id)
  {
  if ( ! future_ptr) ErrXit (9079, "No future");
  return event_list->Remove (where, tkn_id);
  }



void Future::FELdrain ()
  {
  cout << "Draining future event list containing "
       << event_list->Size () << " items\n";
  while ( ! FELempty () )
    event_list->Remove (FROM_FRONT);
  }



/*=============================================================================
In the new-style NextEvent(), we don't confine ourselves to only event-id FEL;
we also support event-routine FEL as well.

It's up to the application pgmr to choose which kind of FEL to use.

i prefer event-routine mode for new code, and we MUST support
event-id mode for compatibility with simpack c-language API.
-----------------------------------------------------------------------------*/

Estatus Future::NextEvent ()
  {
  if ( ! future_ptr) ErrXit (9080, "No future");

  if (snapshot_flag) Snapshot ("BEFORE NEXT_EVENT");

  Estatus es;
  int token_id = -1;

  if (event_list->Empty () )
    {
    cout << "\nI1027 Event list is empty\n";
    es.rtncod = ES_NO_EVENT;
    }
  else
    {
    executing_event = event_list->Remove (FROM_FRONT);

    es.fn       = executing_event.fn;
    es.event_id = executing_event.EventId();
    strncpy(es.descr, executing_event.FnDescr(), ES_STG_LEN);
    es.descr [ES_STG_LEN] = 0;
    es.token    = (Token &) executing_event;
    token_id = es.token.Id();
    es.rtncod   = ES_OK;

    }

  es.now = current_time = executing_event.Time();
  total_token_time += tokens_in_system * (current_time - last_event_time);
  last_event_time = current_time;

  if (snapshot_flag)
    FutMsg ("AFTER NEXT_EVENT", current_event, token_id);

  return es;
  } // end fn



double Future::SimTime ()
  {
  if ( ! future_ptr) ErrXit (9081, "No future");
  return current_time;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

int Future::UpdateArrivals ()
  {
  if ( ! future_ptr) ErrXit (9082, "No future");
  tokens_in_system++;
  return ++arrivals;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

int Future::UpdateDepartures()
  {
  if ( ! future_ptr) ErrXit (9083, "No future");
  tokens_in_system--;
  return ++departures;
  }



/*=============================================================================
Called from Snapshot() with mode=1 & from trace_eventlist() with mode=2.
The reason this fn is in class Future is because the object ptr, which
disambiguates the derived class to which the FEL belongs, is a mbr of Future.
The code for the Display() fn in the various derived classes is in linked.cpp,
heap.cpp, etc.
-----------------------------------------------------------------------------*/

void Future::Display (char * title)
  {
  if ( ! future_ptr) ErrXit (9084, "No future");
  if ( ! event_list) ErrXit (9085, "NULL event_list");

  cout. setf(ios::fixed, ios::floatfield);  cout. setf(ios::showpoint);
  cout. precision(1);

  cout << "-------------------------------------------------------------\n";
  cout << "Simulation snapshot at time: " << setw(5) << current_time << '\n';
  cout << "Title: " << title << ".\n";
  cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
  cout << "Arrivals:"         << setw(18) << arrivals    << "    "
       << "Remove duplicates? " << (remove_duplicates ? "Yes" : "No" ) << '\n';
  cout << "Departures: "      << setw(15) << departures << "    "
       << "Snapshots " << (snapshot_flag ? "Always On" : "On Demand") << '\n';
  cout << "Tokens in system:" << setw(10) << tokens_in_system << "    "
       << "Ranmark: "        << (ranmark ? "On " : "Off") << '\n';
  cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
  DisplayFEL(1); // virtual fn of abstract base class Vlist
  cout << "------------------------------------------------------------\n";
  //ResetIOattribs ();
  }



void Future::DisplayFEL (int mode)
  {
  if ( ! future_ptr) ErrXit (9086, "No future");
  if ( ! event_list) ErrXit (9087, "NULL event_list");
  cout << "FEL is " << event_list->ListType () << ' ';
  event_list->Display(mode); // virtual fn of abstract base class Vlist
  }



/*===========================================================================
Makes use of virtual fn to determine size of FEL in whatever derived class
it may be.  See vlist.h.
---------------------------------------------------------------------------*/

int Future::EventListSize ()
  {
  if ( ! future_ptr) ErrXit (9088, "No future");
  if ( ! event_list) ErrXit (9089, "NULL event_list");
  return event_list->Size ();
  }


Boolean Future::FELempty ()
  {
  if ( ! future_ptr) ErrXit (9090, "No future");
  if ( ! event_list) ErrXit (9091, "NULL event_list");
  return event_list->Empty ();
  }



/*=============================================================================
If the underlying remove fails, you get back an Event with "nothing" in it,
meaning just as the Event default constructor would initialize it, except that
the event_id field has the value NOT_FOUND.  It's up to the caller to check
after return to see if the event_id integer field in the returned event has
the value NOT_FOUND, in which case it means the rest of the Event data is not
meaningful.  If the event_id is NOT_FOUND or the event_routine is NULL, then
we set the es rtncod to NOT_FOUND.
-----------------------------------------------------------------------------*/

Estatus Future::CancelEvent (int event_id)
  {
  if ( ! future_ptr) ErrXit (9092, "No future");
  if (snapshot_flag) FutMsg ("BEFORE CANCEL_EVENT  Event", event_id);
  executing_event = event_list->Remove (MATCHING_EVENT_ID, event_id);
  int token_id = executing_event.Id();
  if (snapshot_flag) FutMsg ("AFTER CANCEL_EVENT", event_id, token_id);
  Estatus es = CurrentEvent();
  if (executing_event.EventId() == NOT_FOUND)
    es.rtncod = NOT_FOUND;
  return es;
  }



Estatus Future::CancelEvent (void (* fn) () )
  {
  if ( ! future_ptr) ErrXit (9093, "No future");
  if (snapshot_flag) Snapshot ("BEFORE CANCEL_EVENT");
  executing_event = event_list->Remove (MATCHING_EVENT_ROUTINE, 0, fn);
  char * fn_descr = executing_event.FnDescr();
  if (snapshot_flag) FutMsg ("AFTER CANCEL_EVENT", fn_descr);
  Estatus es = CurrentEvent();
  if (executing_event.fn == NULL)
    es.rtncod = NOT_FOUND;
  return es;
  }



/*===========================================================================
If the token is not found then executing_event won't have anything in it.
In particular, its token id won't match the one sought.  In dat case,
return NOT_FOUND in the es rtncod.
---------------------------------------------------------------------------*/

Estatus Future::CancelToken (int tkn_id)
  {
  if ( ! future_ptr) ErrXit (9094, "No future");
  if (snapshot_flag)
    FutMsg ("BEFORE CANCEL_TOKEN  Token", tkn_id);

  executing_event = event_list->Remove (MATCHING_TOKEN_ID, tkn_id);

  if (snapshot_flag)
    FutMsg ("AFTER CANCEL_TOKEN", executing_event.EventId(), tkn_id);

  Estatus es = CurrentEvent();
  if (executing_event.Id() != tkn_id)
    es.rtncod = NOT_FOUND;
  return es;
  }

double Future::getUtilization()
{
	return Facility::Stats(1) / current_time;
}

/*=============================================================================
Called from within Future destructor.  Also called from within the old-style
report_stats().  It's a public method, so can also be called directly from
outside the class.  Remember it's a static method (no object).
-----------------------------------------------------------------------------*/

void Future::ReportStats()
  {
  if ( ! future_ptr) ErrXit (9095, "No future");

  cout << '\n';
  cout << "+---------------------------+\n";
  cout << "| SimPack SIMULATION REPORT |\n";
  cout << "+---------------------------+\n";
  cout << '\n';

  double total_sim_time = current_time;
  cout. setf(ios::showpoint);
  cout. setf(ios::fixed, ios::floatfield);
  int old_precision = cout. precision(6);
  cout. width(9);
  cout << "Total Simulation Time: " << total_sim_time << '\n';
  cout << "Total System Arrivals: " << arrivals       << '\n';
  cout << "Total System Departures: " << departures    << '\n';

  cout << '\n';
  cout << "System Wide Statistics\n";
  cout << "----------------------\n";

  double total_busy_time = Facility::Stats (1);
  cout << "System Utilization: ";
  cout. precision(1);
  if (total_sim_time > 0.0)
    {
    double total_utilization = total_busy_time / total_sim_time;
    double percentage = 100.0 * total_utilization;
    cout << percentage << "%\n";
    }
  else
    cout << "unavailable\n";

  cout. precision(6);
  cout << "Arrival Rate: ";
  if (total_sim_time > 0.0)
    {
    double arrival_rate = double (arrivals / total_sim_time);
    cout << arrival_rate;
    }
  else
    cout << "unavailable";

  double throughput = 0.0;
  cout << ", Throughput: ";
  if (total_sim_time > 0.0)
    {
    throughput = double (departures / total_sim_time);
    cout << throughput << '\n';
    }
  else
    cout << "unavailable\n";


  cout << "Mean Service Time per Token: ";
  if (departures > 0)
    {
    double mean_service_time = total_busy_time / departures;
    cout << mean_service_time << '\n';
    }
  else
    cout << "unavailable\n";

  double mean_num_tokens;
  cout << "Mean # of Tokens in System: ";
  if (total_sim_time > 0.0)
    {
    mean_num_tokens = total_token_time / total_sim_time;
    cout << mean_num_tokens;
    }
  else
    cout << "not available";

  cout << "    Largest FEL size was " << event_list->HiWtrMark() << "\n";

  cout << "Mean Residence Time for each Token: ";
  if (throughput > 0.0)
    {
    double mean_residence_time = mean_num_tokens / throughput;
    cout << mean_residence_time << '\n';
    }
  else
    cout << "not available\n";

  cout << '\n';
  Facility::Stats (2);
  cout. precision (old_precision);
  } // end fn



/*=============================================================================
-----------------------------------------------------------------------------*/

void Future::ResetIOattribs ()
  {
  /*cin.      flags( cin_attr->Flags    () );
  cin.      width( cin_attr->Width    () );
  cin.  precision( cin_attr->Precision() );

  cout.     flags(cout_attr->Flags    () );
  cout.     width(cout_attr->Width    () );
  cout. precision(cout_attr->Precision() );*/

  }



/*=============================================================================
Based on trace_update()
-----------------------------------------------------------------------------*/

void Future::Snapshot (char * msg)
  {
  if ( ! future_ptr) ErrXit (9096, "No future");
  if (interactive) // Refresh screen
    {
    #if UNIX
      clear();
      refresh();
      move(0,0);
    #elif TURBOC
      clrscr ();         // proto in conio
      gotoxy (0,0);      // proto in conio
    #endif
    }

  Display (msg);                        // Show some parameters and the FEL.
  if (interactive) GetCmd ();           // Optional prompt
  Facility::FlistDisplay (interactive); // Show all facilities.

  ResetIOattribs ();
  } // end fn



/*=============================================================================
-----------------------------------------------------------------------------*/

char Future::GetCmd ()
  {
  cout << "-- [Enter] to proceed, X to exit, G to go, S to Skip -- ";
  char bfr [TINY_BFR_SIZ+1];
  int ii;
  for (ii=0;ii<=TINY_BFR_SIZ;ii++) bfr[ii]=0;
  cin. width(TINY_BFR_SIZ);
  cin. getline(bfr, TINY_BFR_SIZ);
  cout << '\n';
  char chr = bfr[0] & 0337;
  switch (chr)
    {
    case 'G': interactive = OFF; break;
    case 'X':
      #if UNIX
        endwin();
      #endif
      exit(0);
    }
  return chr; // not used in Future display but IS used in Facility display
  }



/*=============================================================================
Several variants of the function exist.  The compiler distinguishes them based
on their parameter list.  (This is the idea of "name mangling" in c++).
-----------------------------------------------------------------------------*/

void Future::FutMsg (char * msg, char * fn_desc, double intrvl, int tkn_id)
  {
  char fig [BFR_SIZ+1];
  int ii;
  for (ii=0; ii <= BFR_SIZ; ii++) fig [ii] = 0;
  ostrstream figstrm (fig, BFR_SIZ);
  figstrm << msg << " Event '" << fn_desc << "'"
                << "  Inter-Time " << setw(5) << intrvl
                << "  Token " << tkn_id;
  Snapshot (fig);
  }

void Future::FutMsg (char * msg, int event_id, int tkn_id)
  {
  char fig [BFR_SIZ+1];
  int ii;
  for (ii=0; ii <= BFR_SIZ; ii++) fig [ii] = 0;
  ostrstream figstrm (fig, BFR_SIZ);
  figstrm << msg << "  Event " << setw(2) << event_id
                 << "  Token " << setw(3) << tkn_id;
  Snapshot (fig);
  }

void Future::FutMsg (char * msg, int event_or_token_id)
  {
  char fig [BFR_SIZ+1];
  int ii;
  for (ii=0; ii <= BFR_SIZ; ii++) fig [ii] = 0;
  ostrstream figstrm (fig, BFR_SIZ);
  figstrm << msg << setw(3) << event_or_token_id;
  Snapshot (fig);
  }

void Future::FutMsg (char * msg, char * fn_desc)
  {
  char fig [BFR_SIZ+1];
  int ii;
  for (ii=0; ii <= BFR_SIZ; ii++) fig [ii] = 0;
  ostrstream figstrm (fig, BFR_SIZ);
  figstrm << msg << ' ' << fn_desc;
  Snapshot (fig);
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

// end future.cpp

