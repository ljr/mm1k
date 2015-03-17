/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

future.h
-----------------------------------------------------------------------------*/


enum SchedOrExec {SCHEDULE_THIS_FN, EXECUTE_THIS_FN, NO_FN};
enum RunMode {BATCH, INTERACTIVE}; // in SimPack, values were reverse

enum Lspecies {LINKED, HEAP, CALENDAR, LEFTIST, HENRIK, INOMIAL,
  PAGODA, PAIR, SKEWDN, SKEWUP, SPLAY, TWOLIST, NO_FLAVOR};


/*=============================================================================
-----------------------------------------------------------------------------*/

class Future // there's at most 1 object of this class in existence at a time
  {
  friend class Facility;
  friend void end_simpack (void);

  public:
	// ljr
	static double getUtilization();
  public://methods/////////////////////////////////////////////////////////////
  //                                                                         //
  // These methods may be called from outside the class.                     //
  //                                                                         //
  /////////////////////////////////////////////////////////////////////////////

  Future ( // constructor
          Lspecies FEL_type = LINKED,
          void (*event_routine) () = NULL,
          char const * event_routine_description = "",
          SchedOrExec schedule_or_execute = SCHEDULE_THIS_FN,
          Boolean want_snapshots = NO,
          Boolean inter_activ = NO,
          Boolean rmv_dup = NO,
          Boolean reseed = NO
         );

  ~Future(); // destructor is unsupported in this release

  // Alternate way (instead of in constructor) to turn four switches on or
  // off.  If called without parameter, each fn returns present value of
  // switch without changing it.

    static void    RmvDup (Boolean OnOff);
    static Boolean RmvDup (void);

    static void    Reseed (Boolean OnOff);
    static Boolean Reseed (void);

    static void    SnapshotFlag (Boolean OnOff);
    static Boolean SnapshotFlag (void);

    static void    Interactive  (Boolean OnOff);
    static Boolean Interactive  (void);

  // The following 2 scheduling methods are equivalent.  Select between them
  // based on whether you characterize events of your simulation by event_id
  // or event_routine.

    static void Schedule (int  event_id,
         double time_interval, Token tkn);

    static void Schedule (void (*event_routine)(),
         double time_interval, Token tkn, char const * event_routine_descr = "");

  // There are 3 ways to cause event occurrence in sim++.  The automatic
  // method is done with the constructor above.  The remaining 2 occurrence
  // methods are below:  The 1st is semiautomatic; the 2nd is manual.

    static Estatus Dispatch  (void);
    static Estatus NextEvent (void);

  // The fol 3 fns are related but different: the first returns
  // the most recently executed event; the second returns the
  // Token inside the aforementioned Event; the third returns the
  // event_id of the aforementioned Event.

    static Estatus CurrentEvent   (void);
    static Token   CurrentToken   (void);
    static int     CurrentEventId (void);

  // The following group relates to the future event list (FEL):

    static Lspecies FELspecies        (void) {return event_list_type; }
    static int      EventListSize     (void);
    static Boolean  FELempty          (void);
    static void     FELdrain          (void);

  // Ways to get rid of future events before they occur:

    static Estatus CancelEvent       (void (* event_routine) () );
    static Estatus CancelEvent       (int event_id);
    static Estatus CancelToken       (int token_id);

  // Return the present simulation time:

    static double  SimTime           (void);

  // Recording arrivals and departures.  Essential to maintaining
  // meaningful simulation statistics:

    static int     UpdateArrivals    (void);
    static int     UpdateDepartures  (void);

  // These methods give you information, mostly about structures and
  // what's in them.  Snapshot() provides detailed output before and
  // after every important operation.  Snapshot() calls Display()
  // and then calls a method to display facilities.  Display() shows
  // some parameter settings for class Future, then calls DisplayFEL().
  // DisplayFEL() displays the future event list.  All these are public
  // methods, so you may use any of them.  Typically you use Snapshot().
  // ReportStats() provides overall simulation statistics, usually at end
  // of run, but you may get periodic statistics if you like.

    static void    Snapshot          (char const * msg);
    static void    Display           (char const * title);
    static void    DisplayFEL        (int mode = 1);
    static void    ReportStats       (void);


  protected://methods//////////////////////////////////////////////////////////
  //                                                                         //
  // Protected method members are called only by other method members of the //
  // class, or friends.                                                      //
  //                                                                         //
  /////////////////////////////////////////////////////////////////////////////

  // This is the common code for Schedule(), called ONLY from the 2 public
  // versions of above; one for event_id, the other for event_routine.

    static void Schedule (Event eee, double t, Token cus);

  // Called from Snapshot to do output:

    static void FutMsg (char const * msg, char * fn_desc, double intrvl, int tkn_id);
    static void FutMsg (char const * msg, int event_id, int tkn_id);
    static void FutMsg (char const * msg, int event_or_token_id);
    static void FutMsg (char const * msg, char * fn_desc);

  // These operations on future event list (FEL) are called from within the
  // class and from friend class Facility.

    static void    FELinsert         (Event eee, int where);
    static Event   FELremove         (int where, int tkn_id);

  // Called from friend Facility.

    static Event ExecutingEvent      (void);

  // Called from Snapshot() and from Facility display fn:

    static char    GetCmd            (void);

  // Restores x_flags and precision to their values when future was created.

    static void    ResetIOattribs    (void);



  protected://data members/////////////////////////////////////////////////////
  //                                                                         //
  // Direct access to data members is permitted within the class or by       //
  // friends only.                                                           //
  //                                                                         //
  /////////////////////////////////////////////////////////////////////////////

    static Future *       future_ptr;
    static Lspecies       event_list_type;
    static class Vlist *  event_list;
    static Boolean        remove_duplicates;
    static Boolean        ranmark;               // generate same rn stream
    static Boolean        snapshot_flag;         // formerly trace_flag
    static int            rn_stream;
    static int            arrivals;
    static int            departures;
    static int            tokens_in_system;
    static int            current_event;
    static double         current_time;
    static double         last_event_time;
    static double         total_token_time;
    static Event          executing_event;
    static class Ioattr * cin_attr;
    static class Ioattr * cout_attr;
    static Boolean        interactive;

  }; // end class


/*=============================================================================
-----------------------------------------------------------------------------*/

class Ioattr
  {
  public:
    Ioattr (long fff, int www, int ppp)
      { flags=fff; width=www; precision=ppp; }
    long Flags     () {return flags;     }
    int  Width     () {return width;     }
    int  Precision () {return precision; }
  protected:
    long flags;
    int width;
    int precision;
  };

// end future.h

