/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

event.h
-------------------------------------------------------------------------------
Dfn of class Event
Event is a derived class, base class is Token.
Tokens are known to application, but Events are known only within SimPack.

Estatus is a structure which returns information about an Event, plus some
otherinfo, to the simulation application.
-----------------------------------------------------------------------------*/


/*=============================================================================
-----------------------------------------------------------------------------*/

class Event : public Token
  {
  public: // methods

    // Constructors: some parameters optional, defaults as shown.
    Event(int et=0,      double ttt=0.0, int ppp=0);
    Event(void (*ff) (), double ttt=0.0, int ppp=0, char * fn_descr = "");
    Event(void (*ff) (), char * fn_descr);

    ~Event(); // destructor

    // Inline methods to examine member data:
    double Time     (void)         { return time;      }
    int    EventId  (void)         { return event_id;  }
    int    Priority (void)         { return priority;  }
    void * Routine  (void)         { return (void *) fn; }

    // Inline methods to change member data:
    void   Time     (double tt)    { time     = tt;    }
    void   EventId  (int    ee)    { event_id = ee;    }
    void   Priority (int    pp)    { priority = pp;    }
    void   Routine  (void(*ff)() ) { fn = ff;          }
    char * FnDescr  (void)         { return fn_descr;  }

  protected: // data

    static unsigned created, destroyed;
    unsigned serialnumber;

    double time;
    int priority;
    int event_id;                 // old style event identifier
    char fn_descr [ES_STG_LEN+1]; // optional

  public: // data

    void (*fn) ();                // new style event-routine

  }; // end class Event


/*=============================================================================
-----------------------------------------------------------------------------*/

enum EstatusRtnCodes
  {
  ES_OK,
  ES_NO_EVENT = -1, // future event list was empty
  ES_NO_FN    = -2, // action not specified in event
  ES_UNKNOWN  = -3  // set by constructor
  };

struct Estatus
  {
  Estatus () {rtncod = ES_UNKNOWN; now = 0.0;
              fn = NULL; event_id = -1; descr [0] = 0; }

  char * ReturnCodeString (void);
  void   ES_DisplayReturnCode (void);
  void   ES_Display (void);

  int rtncod;
  double now;
  void (* fn) ();
  int event_id;
  char descr [ES_STG_LEN+1];
  Token token;
  };


// end event.h

