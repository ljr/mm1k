/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

facility.h
-----------------------------------------------------------------------------*/

/*=============================================================================
capacities for facilities.  facility capacity limit applies ONLY to calls to
c-language API fn create_facility().  Limit does NOT apply to facilities
created with the c++ API (Facility constructor).
-----------------------------------------------------------------------------*/

const int MAX_FACILITIES = 1000; // max number of facilities


enum FacStatus {FREE, BUSY};
const int MAX_FAC_NAME_LEN = 64;
const double MINUS_INFINITY = -1.0e77; // for debug during development

/*=============================================================================
-----------------------------------------------------------------------------*/

class Facility
  {
  public:

    Facility (char * name = "", int n_srvrs = 1, int fac_id = -1);
    ~Facility();  // destructor unlinks facility from flist
                  // but is unsupported in this release
    FacStatus Request (Token tkn, int pri = 0);
    FacStatus Preempt (Token tkn, int pri = 0);
    void Release (int tkn_id);
    static double Stats (int mode); // modes are 1 and 2
    void Trace (void);
    double    InuseTime (void) {return total_busy_time; }
    FacStatus Status    (void) {return status; }
    int       FQlength  (void);
    Boolean   FQempty   (void);

    //called fr Future::Snapshot() & trace_facility()
    static void FlistDisplay (Boolean interactive = NO);
    char FacDisplay (Boolean interactive = NO);

    // compatibility mode fn, called from c-language api fn create_facility():
    static int GetAnAvailable (char * Name, int number_of_servers);

    // compatibility mode fn, called from c-lang api fns request() & release():
    static Facility * GetPtr (int facility_id);

    static void    ShowIdle (Boolean yn) {show_idle = yn; }
    static Boolean ShowIdle (void)       {return show_idle; }

  protected: // methods

    void FacMsg (char * stg, int tkn_id, int pri);
    void FacMsg (char * stg, int tkn_id);
    static void RemoveAll (void);

  protected: // data

    static Boolean need_to_initialize_pointers;
    static Facility * pointers [MAX_FACILITIES+1];
    static int created, destroyed;
    static Boolean show_idle; // used in conjunction with displays

    int serialnumber;
    class Vlist * fq; // facility's priority queue
    char * name;
    FacStatus status;
    int busy_servers;
    double total_busy_time;
    double start_busy_time;
    int preemptions;
    int total_servers;
    class Service * service;
    Facility * succ;          // facilities are doubly-linked into a list,
    Facility * pred;          // traversed for snapshots & statistics.
    static class Flist flist; // Here is that list.
    friend class Flist;       // flist can access Facility members
    friend  void end_simpack (void); // end_simpack() can access Facility mbrs

  }; // end class



/*=============================================================================
-----------------------------------------------------------------------------*/

class Service
  {
  public:
    Service (int ns);
    ~Service (); // free dyn alloc'd array
    int FindIdle (); // returns server # that is idle, or num_srvrs if none idle
    int FindSrvr (int tkn_id);
    int FindMinPriSrvr (void);
    void Engage (int srvr_i, int tkn_id, int pri);
    void Disengage (int srvr_i);
    int GetPri (int srvr_i);
    int GetTokenID (int srvr_i);
    void Display (void);

  protected:

    int num_srvrs;
    class Srvrunit * srvrunit; // ptr to dyn allocd array of server units
  };

/*=============================================================================
-----------------------------------------------------------------------------*/

class Srvrunit
  {
  public:
    Srvrunit () {tkn_id = FREE; pri = 0; }

  protected:

    friend class Service;   // Service can access Srvrunit members
    int tkn_id;
    int pri;
  };

/*=============================================================================
-----------------------------------------------------------------------------*/

// end facility.h

