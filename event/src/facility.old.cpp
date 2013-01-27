/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

facility.cpp
-----------------------------------------------------------------------------*/

#include <iostream.h>
#include "queuing.h"


/*=============================================================================
Static data mbrs of class Facility.
-----------------------------------------------------------------------------*/

// Some are for compat simpack c-language api:

  Boolean Facility::need_to_initialize_pointers = YES;
  Facility * Facility::pointers [MAX_FACILITIES+1];
  Flist Facility::flist;

int     Facility::created = 0;
int     Facility::destroyed = 0;
Boolean Facility::show_idle = YES;



/*=============================================================================
Find an unused facility_id.  Allocate a facility object.
Associate the facility_id with the facility object.
Static member method.
Called by create_facility().
Povides compatibility with legacy facility code.
-----------------------------------------------------------------------------*/

int Facility::GetAnAvailable (char * Name, int nsrvrs)
  {
  // in case of old-style call, initialize pointers array if NOT alrdy done.
  // Idempotent, so ok to call multiple times.  Range 1..max wastes 1 facility
  // ptr (ptr[0]) but maintains compat w/old simpack.
  if (need_to_initialize_pointers)
    {
    int ii;
    for (ii=0; ii <= MAX_FACILITIES; ii++)
      pointers [ii] = NULL;
    need_to_initialize_pointers = NO;
    }

  int facility_id = MAX_FACILITIES+1;
  int ii;
  for (ii = 1; ii <= MAX_FACILITIES; ii++)
    if (pointers [ii] == NULL)
      {
      facility_id = ii; // found an available facility
      break;
      }
  if (facility_id > MAX_FACILITIES) ErrXit (9041, "Too many facilities");
  pointers [facility_id] = new Facility (Name, nsrvrs, facility_id);
  return facility_id;
  }



/*=============================================================================
Facility constructor.  # of servers defaults to 1 if not specified.
-----------------------------------------------------------------------------*/

Facility::Facility (char * ccc, int nnn, int fac_id)
  {
  if (nnn < 1) ErrXit (9042, "Bad nnn");
  total_servers = nnn;

  created++;

  // Serial # is internally meaningful (within simpack) only for facilities
  // created thru old_style create_facility() calls.  For facilities created
  // directly with constructor, fac_id defaults to -1, but can be specified
  // if it will be useful to the application to do so.  If default value is
  // given to constructor, it uses count of facilities created so far as serial
  // #.  You are welcome to mix & match create_facility() and constructor
  // calls, but if you do, do NOT rely on serial # to be unique.

  if (fac_id == -1)         // default is -1 if not specified in
    serialnumber = created; //  constructor call
  else                      // create_facility() specifies fac_id
    serialnumber = fac_id;  //  when calling constructor

  Boolean named = (ccc != NULL);
  char newname [MAX_FAC_NAME_LEN+1]; // temporary
  int len;
  if (named)
    len = strlen(ccc);
  else
    {
    int ii;
    for (ii=0; ii <= MAX_FAC_NAME_LEN; ii++) newname [ii] = 0;
    ostrstream stgstrm(newname, MAX_NAME_LEN);
    if (serialnumber == -1)
      stgstrm << "Facility " << this;
    else
      stgstrm << "Facility " << serialnumber;
    len = strlen(newname);
    ccc = newname;
    }

  // facility name of any length is ok, but only the 1st MAX_FAC_NAME_LEN
  // characters will be used.
  if (len > MAX_FAC_NAME_LEN)
    len = MAX_FAC_NAME_LEN;

  name = new char [len + 1];
  strncpy(name, ccc, len);
  name [len] = 0;

  status = FREE;
  busy_servers = 0;
  total_busy_time = 0.0;
  preemptions = 0;

  service = new Service (total_servers);

  fq = Vlist::Constructor (LINKED); // create the queue

  // Link the facility (the whole facility, not just its queue) into a
  // doubly-linked list of facilities.  This is needed for statistics-gathering
  // purposes, to be able to scan all facilities and report stats.

  succ = pred = NULL;
  flist.Append (this);

  }//end fn



/*=============================================================================
Facility destructor.
/// we really need
/// to redefine assignment operator for ANY class that contains dyn alloc'd
/// elms; otherwise, assignment followed by destructor, where destructor
/// deletes dyn alloc'd elms, will cause BOOM!
-----------------------------------------------------------------------------*/

Facility::~Facility()
  {
  cout << "WARNING: facility destructor not supported in this release\n";
  flist.Unlink (this);
  if (name) delete [] name;
  delete service;
  cout << "Draining facility queue containing " << fq->Size()
       << " items\n";
  while (fq->Size() > 0)
    fq->Remove (FROM_FRONT);
  destroyed++;
  }//end fn



/*=============================================================================
Remove all facilities linked together in the flist.  This is a protected
method, and at present not called from within the class, but is only called
by friend function end_simpack().  The purpose of using protected/friend is to
make this powerful potentially dangerous method out of the ooAPI.
-----------------------------------------------------------------------------*/

void Facility::RemoveAll ()
  {
  cout << "WARNING: facilities removal not supported in this release\n";
  Facility * fptr = flist.front;
  while (fptr)
    {
    delete fptr;
    fptr = fptr->succ;
    }
  flist.front = flist.rear = NULL;
  }



/*=============================================================================
Map facility_id to Facility ptr.
Static mbr method.
Called by c-language api fns request(), preempt(), release(), etc.
Povides compatibility with legacy facility code.
-----------------------------------------------------------------------------*/

Facility * Facility::GetPtr (int facility_id)
  {
  if (facility_id < 1 || facility_id > MAX_FACILITIES)
    {
    cout << "Facility #" << facility_id << '\n';
    ErrXit (9043, "Bad facility_id");
    }
  Facility * ptr = pointers [facility_id];
  if (ptr == NULL) ErrXit (9044, "NULL ptr");
  return ptr;
  }



/*=============================================================================
If facility has an available server, begin serving the token, and return FREE,
indicating that there WAS a free server available.  otherwise, build an event
from the token provided, and place that event in the facility's queue.  as for
what happens to that event later, see Release().
-----------------------------------------------------------------------------*/

FacStatus Facility::Request(Token tkn, int pri)
  {
  int tkn_id = tkn.Id();
  if (tkn_id == 0) ErrXit (9045, "Token id of zero is not permitted");

  // search for possible duplication of token id of a token alrdy rcvg svc
  // FindSrvr returns total_servers, a value out of range, if not found
  if (service->FindSrvr (tkn_id) < total_servers)
    ErrXit (9046,
      "Token with this id is already receiving service from this Facility");

  Boolean want_snapshot = Future::SnapshotFlag();
  if (want_snapshot) FacMsg ("BEFORE REQUEST", tkn_id, pri);
  double now = Future::SimTime();
  if (busy_servers < 1) start_busy_time = now;

  if (status == FREE) // search for an open server
    {
    int povider = service->FindIdle ();
    if (povider >= total_servers) ErrXit (9047, "Internal error");
    service->Engage (povider, tkn_id, pri);
    if (++busy_servers >= total_servers) status = BUSY;
    if (want_snapshot) FacMsg ("AFTER REQUEST", tkn_id, pri);
    return FREE;
    }

  // all servers in facility are busy, so facility is BUSY, queue Event
  Event evnt = Future::ExecutingEvent(); // gets event_id etc from here
  evnt.Time(now);
  evnt.Priority(pri);
  (Token &) evnt = tkn;
  fq->Insert (evnt, BEHIND_PRIORITY_KEY);
  if (want_snapshot) FacMsg ("AFTER_REQUEST", tkn_id, pri);
  return BUSY;
  } // end fn



/*=============================================================================
-----------------------------------------------------------------------------*/

FacStatus Facility::Preempt (Token tkn, int pri)
  {
  int tkn_id = tkn.Id();
  if (tkn_id == 0) ErrXit (9048, "Token id of zero is not permitted");

  // search for possible duplication of token id of a token alrdy rcvg svc
  // FindSrvr returns total_servers, a value out of range, if not found
  if (service->FindSrvr (tkn_id) < total_servers)
    ErrXit (9049,
      "Token with this id is already receiving service from this Facility");

  Boolean want_snapshot = Future::SnapshotFlag();
  if (want_snapshot) FacMsg ("BEFORE PREEMPT", tkn_id, pri);
  double now = Future::SimTime();
  if (busy_servers < 1) start_busy_time = now;

  //
  // CASE #1: there's an IDLE server, preempt works like Request()
  //

  if (status == FREE) // search for an open server
    {
    int povider = service->FindIdle ();
    if (povider >= total_servers) ErrXit (9050, "Internal error");
    service->Engage (povider, tkn_id, pri);
    if (++busy_servers >= total_servers) status = BUSY;
    if (want_snapshot) FacMsg ("AFTER PREEMPT", tkn_id, pri);
    return FREE;
    }

  //
  // CASE 2: All servers in facility are busy, so facility is BUSY.
  //         But our would-be pre-emptor does NOT outrank any token now
  //         receiving service, so NO pre-empt, and would-be pre-emptor
  //         goes to the facility's queue, just like Request().
  //
  int lo_pri_server = service->FindMinPriSrvr ();
  int min_pri = service->GetPri (lo_pri_server);
  if (pri <= min_pri)
    {
    Event evnt = Future::ExecutingEvent(); // gets event_id etc from here
    evnt.Time(now);
    evnt.Priority(pri);
    (Token &) evnt = tkn;
    fq->Insert (evnt, BEHIND_PRIORITY_KEY);
    if (want_snapshot) FacMsg ("AFTER_PREEMPT", tkn_id, pri);
    return BUSY;
    } // end no preempt

  //
  // CASE 3: Facility is full-up, and pre-emptor outranks at least one
  //         token now receiving service.  Evict a lowest-pri token and
  //         start serving the pre-emptor.
  //
  preemptions++;

  // save info about evicted token
  int preempted_tkn = service->GetTokenID (lo_pri_server);
  int preempted_tkn_pri = min_pri;

  // pre-emptor begins service:
  service->Engage (lo_pri_server, tkn_id, pri);

  // What happens to the evicted lo-pri token is interesting: assume there's
  // a release() scheduled in future for this token, find & remove same
  // from FEL.
  Event egg = Future::FELremove (MATCHING_TOKEN_ID, preempted_tkn);

  // We have the event in question.  Set its priority.  Calculate its
  // remaining service time, then negate it as special signal that this is a
  // time remaining not an ordinary time.
  egg.Priority (preempted_tkn_pri);
  egg.Time (now - egg.Time() ); // this value will be (better be) negative
  if (egg.Time() > 0.0) ErrXit (9051, "Positive time");

  // re-insert preempted token in facility queue - it should
  // be placed ahead of other tokens with same priority
  fq->Insert (egg, AHEAD_PRIORITY_KEY);

  if (want_snapshot) FacMsg ("AFTER-PREEMPT", tkn_id, pri);
  return FREE; // indicates pre-emptor has begun service
  } // end fn



/*=============================================================================
-----------------------------------------------------------------------------*/

void Facility::Release (int tkn_id)
  {
  Boolean want_snapshot = Future::SnapshotFlag();
  if (want_snapshot) FacMsg ("BEFORE RELEASE", tkn_id);

  if (tkn_id == 0) ErrXit (9052, "Zero token id is not permitted");
  if (busy_servers < 1) ErrXit (9053, "Facility has no busy server");
  int srvr_i = service->FindSrvr (tkn_id);
  if (srvr_i >= total_servers)
    {
    cout << "Time: " << Future::SimTime() << '\n';
    cout << "Facility " << serialnumber << " '" << name << "'\n";
    cout << "Token id: " << tkn_id << "\n";
    ErrXit (9054, "This facility not poviding service to this token");
    }
  service->Disengage (srvr_i);

  double now = Future::SimTime();
  status = FREE;
  if (--busy_servers < 1)
    {
    if (start_busy_time == MINUS_INFINITY) ErrXit (9055, "Internal error");
    total_busy_time += now - start_busy_time;
    start_busy_time = MINUS_INFINITY; /// to see if it matters
    }
  if (fq->Empty() ) // no one waiting in this facility's queue
    {
    if (want_snapshot) FacMsg ("AFTER RELEASE", tkn_id);
    return;
    }

  Event eee = fq->Remove (FROM_FRONT);
  if (eee.Time() >= 0.0) // "normal" NON-preempted token
    {
    eee.Time(now);
    Future::FELinsert (eee, TIME_KEY); // used to be add_front_list
    if (want_snapshot) FacMsg ("AFTER_RELEASE", tkn_id);
    return;
    }

  // This guy was preempted - put the completion event back on event
  // list with a new time to reflect its wait in the queue after being
  // preempted.  This event must be scheduled to release
  // a server of the facility.  Recall that event time here is negative,
  // and that its absolute value is the remaining service time required, as
  // calculated by Preempt() when it evicted this guy.  Subtracting this
  // value in effect adds this interval to the present time, forming the
  // new service-completion time for this guy.

  eee.Time(now - eee.Time() );
  Future::FELinsert (eee, TIME_KEY);
  status = BUSY; // logic err? probably not see notbug.001 (if q then all busy)
  busy_servers++;
  start_busy_time = now;
  service->Engage (srvr_i, eee.Id(), eee.Priority() );
  if (want_snapshot) FacMsg ("AFTER-RELEASE", tkn_id);
  } // end fn



/*=============================================================================
Cannot be inline because Vlist's methods are unknown in facility.h.
-----------------------------------------------------------------------------*/

int Facility::FQlength ()
  {
  return fq->Size ();
  }



/*=============================================================================
Cannot be inline because Vlist's methods are unknown in facility.h.
-----------------------------------------------------------------------------*/

Boolean Facility::FQempty ()
  {
  return fq->Empty ();
  }



/*=============================================================================
Gather facility statistics, usually at end of run
-----------------------------------------------------------------------------*/

double Facility::Stats (int mode)
  {
  Facility * fptr = flist.front;
  double now = Future::SimTime();
  double total_sim_time = now;

  switch (mode)
    {
    case 1:
      {
      double busy_time = 0.0;
      int numfac = 0;
      while (fptr)
        {
        numfac++;   // count facilities used

        // update busy time when gathering statistics from a facility
        // with server(s) in use.

        if(fptr->busy_servers > 0)
          {
          fptr->total_busy_time += now - fptr->start_busy_time;
          fptr->start_busy_time = now;
          cout << "NOTE: facility " << fptr->serialnumber
            << " '" << fptr->name << "' has "
            << fptr->busy_servers << " busy server(s)\n";
          }
        busy_time += fptr->total_busy_time;
        fptr = fptr->succ;
        } // end while
      if (numfac > 0)
        busy_time /= double(numfac);
      else
        {
        cout << "NOTE: Facility statistics were not calculated "
             << "because no facilities exist.\n";
        busy_time = 0.0;
        }
      return busy_time;
      }

    case 2:
      {
      cout << "Facility Statistics\n";
      cout << "-------------------\n";

      cout. setf(ios::fixed, ios::floatfield);
      cout. setf(ios::showpoint);
      int old_precision = cout. precision(1);

      double utilization = 0.0;
      double idle = 0.0;
      while (fptr)
        {
        cout << "F " << fptr->serialnumber << " (" << fptr->name << "): ";
        if (total_sim_time > 0)
          {
          utilization = 100.0 * fptr->total_busy_time / total_sim_time;
          idle = 100.0 * (total_sim_time - fptr->total_busy_time)
            / total_sim_time;
          cout << "Idle: " << setw(4) << idle << "%, ";
          cout << "Util: " << utilization << "%, ";
          }
        else
          cout << "Idle:  N/A %, Util: N/A %, ";

        cout << "Preemptions: " << fptr->preemptions
          << ", LongestQ: " << fptr->fq->HiWtrMark() << "\n";
        fptr = fptr->succ;
        } // end while
      cout. precision (old_precision);
      break;
      }

    default: ErrXit (9056, "Bad mode");
    } // end switch

  return 0.0;
  } // end fn



/*=============================================================================
Static fn to display ALL facilities.  The flist doubly-links all facilities.
Traverse flist, for each facility in flist, display info about the facility,
its servers, and its queue.
-----------------------------------------------------------------------------*/

void Facility::FlistDisplay (Boolean interactive)
  {
  Boolean was_interactive = interactive;
  char chr = 0;
  Facility * fptr;
  for (fptr = flist.front; fptr; fptr = fptr->succ)
    {
    if (interactive)
      interactive = (chr != 'G' && chr != 'S');
    chr = fptr->FacDisplay (interactive);
    }

  // in case user skipped thru the display, catch and hold at the end:
  if (was_interactive && chr == 'S')
    chr = Future::GetCmd (); // optional smart prompt
  } // end fn



/*=============================================================================
-----------------------------------------------------------------------------*/

char Facility::FacDisplay (Boolean interactive)
  {
  if ( ! show_idle && busy_servers < 1 && FQlength() < 1) return 0;

  cout << "## FACILITY " << setw(2) << serialnumber
    << " (" << name << "): "
    << setw(2) << total_servers << " server(s), "
    << setw(2) << busy_servers << " busy.\n";
  cout << "Server detail: ";
  service->Display ();
  cout << '\n';
  fq->Display (name); // calls a method in derived class, eg, linked
  char chr = 0;
  if (interactive)
    chr = Future::GetCmd (); // optional smart prompt
  return chr;
  }



/*=============================================================================
Potected mbr methods stg is eg:  "BEFORE REQUEST", "AFTER PREEMPT", etc.
Several variants of the function exist.  The compiler distinguishes them based
on their parameter list.  (This is the idea of "name mangling" in c++).
-----------------------------------------------------------------------------*/

void Facility::FacMsg (char * stg, int tkn, int pri)
  {
  char fig [BFR_SIZ+1];
  int ii;
  for (ii=0; ii <= BFR_SIZ; ii++) fig[ii] = 0;
  ostrstream figstrm (fig, BFR_SIZ);
  figstrm << stg << "  Facility " << setw(2) << serialnumber
                 << "  Token "    << setw(3) << tkn
                 << "  Priority " << setw(2) << pri;
  Future::Snapshot (fig);
  }

void Facility::FacMsg (char * stg, int tkn)
  {
  char fig [BFR_SIZ+1];
  int ii;
  for (ii=0; ii <= BFR_SIZ; ii++) fig[ii] = 0;
  ostrstream figstrm (fig, BFR_SIZ);
  figstrm << stg << "  Facility " << setw(2) << serialnumber
                 << "  Token " << setw(3) << tkn;
  Future::Snapshot (fig);
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

// end facility.cpp
