/*=============================================== =============================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

vlist.h
-------------------------------------------------------------------------------
Vlist is an abstract base class:  there will never be an object of this class;
however, there WILL be objects of its derived classes.  An object of one of the
derived classes serves as future event list (FEL) for the simulation.  Other
objects of various derived classes (eg, Linked) serve other purposes (eg,
facility queue).

When code outside the class calls a member fn of the base class, such calls are
forwarded to the method of the appropriate derived class, permitting the caller
to do a generic insert for example, without knowing whether the insert will be
into a linked list, a heap, etc.  This is extensible code.

Vlist's static member method Construct hides the detail of selecting a
derived type based on an enumeration type value (an Lspecies), invoking the
constructor of the selected derived class, and returning a generic ptr to this
object.

Neither Turbo C++ nor unix CC (cfront) complained but unix g++ did complain
"warning: class Linked has virtual functions but non-virtual destructor"
when base class destructor was not specified (hence not virtual).  So i
declared a vlist base class destructor in order to declare it virtual.
See Borland's "The World of C++", chapter 19, p.112, regarding virtual
destructors, and implying that (at least sometimes) they are necessary for
proper cleanup.

Another portability issue:  neither Turbo C++ nor unix CC (cfront) complained
but unix g++ generated an error "redeclaration of default argument 2" for the
declaration of Heap::Insert().  Turns out that g++ did not like parameter
default value specified in a derived class being different from default value
in base class.
-----------------------------------------------------------------------------*/



// Enumeration values guide generalized insertion and removal fns, to maintain
// specific discipline.  For example, AT_FRONT and FROM_FRONT form a (LIFO)
// stack; AT_REAR and FROM_FRONT form a (FIFO) Queue.

    enum InsertWhere {TIME_KEY, AHEAD_PRIORITY_KEY, BEHIND_PRIORITY_KEY,
                      AT_FRONT, AT_REAR};
    //Mario
    enum RemoveWhere {FROM_FRONT, MATCHING_TOKEN_ID, MATCHING_EVENT_ID,
                      MATCHING_EVENT_ROUTINE, MATCHING_CLASS};
    //



/// Fn prototype, may be able to elim by judicious rearrangement of .h files.
/// Leave that for some other time.

     void ErrXit (int, char *);



/*=============================================================================
Abstract base class for extensible code.  If there's no method in a derived
class for a particular fn, then the base class fn will execute, producing a
"Not implemented" msg.
-----------------------------------------------------------------------------*/

class Vlist
  {
  public:

    static Vlist * Constructor (Lspecies fel_type);
    void SetType (Lspecies typ) {i_am = typ; }
    int Size (void) {return size; }
    Boolean Empty (void) {return (size < 1); }
    int HiWtrMark (void) {return hi_wtr_mark; }
    char * ListType (void);                    // eg, "Heap"
    char * InsertionPoint (InsertWhere here);  // eg, "AT_REAR"

    virtual void  Insert (Event item, int where)
      {cout << "I2423 Not implemented\n"; if (where>item.Id()) ; }
    virtual Event Remove (int where, int id = -1, void (*func) () = NULL)
      {cout << "I2424 Not implemented\n"; if (where-id||func) ; return 0;}
    virtual void Display (int mode = 1)
      {cout << "I2425 Not implemented\n"; if (mode) ; }
    virtual void Display (char * descr)
      {cout << "I2426 Not implemented\n"; if (descr) ; }
    //Mario
    virtual Event RemoveSel (int where, int classe, int max) {return 0;}
    //
    virtual ~Vlist () {}

  protected: // methods

    void CannotRemoveFromEmpty (int where);
    void BoxEdge (int width);
    void ShowSizeEtc (double now);
    int SetBoxWidth (double now);

  protected: // data

    Lspecies i_am;
    int size;
    int hi_wtr_mark; // largest value of size in this run

  };



/*=============================================================================
Derived class Linked
-----------------------------------------------------------------------------*/

class Linked : public Vlist
  {
  public:

    // A list node consists of an Event and a pointer.  In that sense, Lnode is a
    // derived class of base class Event.
    struct Lnode : public Event
      {
      Lnode () : Event () { next = NULL; } // constructor
      Lnode * next; // data
      }; // end struct Lnode

    Linked () { front = rear = NULL; size = hi_wtr_mark = 0; }
    ~Linked (); // return nodes (if any) to freepool
    void    Insert  (Event item, int where);
    Event Remove (int where, int token_or_event_id = -1, void (*func)() = NULL);
    void    Display (int mode = 1); // modes are 1 and 2, dflt is 1
    void    Display (char * descr); // descr, eg, "FEL" or "Facility 4"
    //Mario
    Event RemoveSel (int where, int classe, int max);
    //

  protected: // data

    Lnode * front, * rear;

  };



/*=============================================================================
Derived class Heap
-----------------------------------------------------------------------------*/

#if TURBOC
  const int STANDARD_HEAP_CAPACITY = 100; // heap size
#else
  const int STANDARD_HEAP_CAPACITY = 50000;
#endif

class Heap : public Vlist
  {
  public:

    Heap (int cap = STANDARD_HEAP_CAPACITY);
    void Insert (Event item, int where_ignored);
    Event Remove (int where, int token_or_event_id = -1, void (*func)() = NULL);

    void  Display (int mode = 1);
    void    Display (char * descr); // descr, eg, "FEL" or "Facility 4"

  protected: // methods

    void HeapSwap (int ii, int jj);
    void HeapDownAdjust (int parent);
    void HeapUpAdjust (int child);

  protected: // data

    int capacity;
    Event * elms; // dyn-allocd array of events

  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Leftist : public Vlist
  {
  public:

    enum SearchResult {TURN_LEFT, TURN_RIGHT, IN_THIS_NODE,
                       NOT_IN_THIS_SUBTREE = -1};

    struct Lnode : public Event
      {
      public:

        Lnode () : Event () { lson = rson = NULL; dist = 1; } // constructor
        void Picture (int level, char * stg);
        static int Dista (Lnode * ptr) { if (ptr) return ptr->dist; return 0; }
        static void Clear (Lnode * ptr);
        int dist;             // data
        SearchResult flag;
        Lnode * lson, * rson;

      }; // end struct

    Leftist (Lnode * rrr = NULL);
    void Clear (void);
    void  Insert (Event item, int where);
    Event Remove (int where, int id = -1, void (*func) () = NULL);
    void Display (int mode = 1);
    void Display (char * descr);
    static void Tester (void);

  protected: // methods

    void Merge (Leftist * tree1);
    Boolean Search (Lnode * ptr, int which, int id, void (*func)() = NULL);
    Event ExtractAndAdjust (int value, Lnode * ptr);
    static void TestEvent (void);

  protected: // data

    Lnode * root;

  };



/*=============================================================================
Derived class Calendar.
-----------------------------------------------------------------------------*/

#if TURBOC
  const long STANDARD_CALENDAR_CAPACITY = 64 - (16-1) + 99; // ??
  const long MAX_NBUCKETS = 32-1;
#else
  const long STANDARD_CALENDAR_CAPACITY = 65536 - (16384-1);
  const long MAX_NBUCKETS = 32768-1;
#endif

const int LARGEST_CALENDAR_SAMPLE = 25;

class Calendar : public Vlist
  {
  public:

    enum ResizeAction {FOLD, SPLIT};

    struct Lnode : public Event
      {
      Lnode () : Event () { next = NULL; } // constructor
      Lnode * next; // data
      }; // end struct Lnode

    Calendar (long cap = STANDARD_CALENDAR_CAPACITY,
              Boolean r_e = YES);
    void Insert (Event eee, int where);
    Event Remove (int where, int id = -1, void (*func) () = NULL);
    void Display (int mode = 1);

    static void Tester (void);

  protected: // methods

    void Setup (int qbase, int nbuck, double bwidth, double startpri);
    void ShowParms (void);
    void CalResize (ResizeAction r_a);
    double NewWidth (void);
    int ObtainSample (Event * sample_event_array, int sample_size);
    static void TestEvent (void);

  protected: // data

    long capacity;          // size of bucketspace
    Boolean resize_enable;
    Lnode * * buckets;      // array of ptrs, dyn allocd at constructor time
    Lnode * * actv_bukts;   // array of active bucket ptrs, subset of above
    long firstsub;
    long nbuckets;
    long split_threshold;   // size > split_threshold --> resize * 2
    long  fold_threshold;   // size < fold_threshold  --> resize * 1/2
    double cal_width;
    long   previous_bucket;
    double previous_key;
    double bucket_top;

  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Henrik : public Vlist
  {
  public:
    Henrik () {ErrXit (9191, "Not implemented");}
  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Inomial : public Vlist
  {
  public:
    Inomial () {ErrXit (9192, "Not implemented");}
  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Pagoda : public Vlist
  {
  public:
    Pagoda () {ErrXit (9193, "Not implemented");}
  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Pair : public Vlist
  {
  public:
    Pair () {ErrXit (9194, "Not implemented");}
  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Skewdn : public Vlist
  {
  public:
    Skewdn () {ErrXit (9195, "Not implemented");}
  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Skewup : public Vlist
  {
  public:
    Skewup () {ErrXit (9196, "Not implemented");}
  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Splay : public Vlist
  {
  public:
    Splay () {ErrXit (9197, "Not implemented");}
  };



/*=============================================================================
Derived class
-----------------------------------------------------------------------------*/

class Twolist : public Vlist
  {
  public:
    Twolist () {ErrXit (9198, "Not implemented");}
  };


/*=============================================================================
-----------------------------------------------------------------------------*/

// end vlist.h

