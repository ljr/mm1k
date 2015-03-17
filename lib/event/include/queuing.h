/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

queuing.h
-------------------------------------------------------------------------------
User includes this as the "one and only" sim++ header.  It supports the new
sim++ ooAPI (object oriented API) by including class definitions.  It also
supports complete compatibility with simpack c-language programs, in place of
simpack's queuing.h, by including the cAPI prototype declarations.
-----------------------------------------------------------------------------*/

// always include the following system header files:

    #include <iostream>  // cin, cout, etc
    #include <iomanip>   // manipulators (see tc++ug p192)
    #include <fstream>   // ifstream, etc
    #include <strstream> // ostrstream
    //#include <sstream>   // basic_istringstream, basic_ostringstream
    #include <stdlib.h>    // exit()
    #include <string.h>    // strcmp()
    #include <stdio.h>     // printf(), scanf()

using namespace std;

// defines

    #define do_until_break for (;;)

// typedefs & enums:

    typedef unsigned char Boolean;

    enum YesNo {NO, YES};
    enum Switch {OFF, ON};
    enum TrueFalse {FALSE, TRUE};
    const int NOT_FOUND    =  -1; // in Event
    const int MAX_NAME_LEN = 100; // in Facility
    const int ES_STG_LEN   =  24; // in Estatus and Event
    const int BFR_SIZ      = 222; // in Facility & Future
    const int TINY_BFR_SIZ =  16;

// Class dfns:

    const int MAX_NUM_ATTR = 5; // for old style token, for compatibility mode
    struct TOKEN // old style token
      {
      float attr [MAX_NUM_ATTR];
      };

    #include "token.h"
    #include "flist.h"   // class Flist = facility-doubly-linked-list
    #include "facility.h"
    #include "event.h"   // class Event derived fr base class Token
    #include "future.h"  // class Future
    #include "vlist.h"   // base class Vlist, derived classes Linked, Heap, etc
                         // Linked contains Lnode *, Lnode is derived fr Event
//
// Fn prototype declarations for NON-API fns:
//
    void DynAllocFail(void); // registered by call to set_new_handler()
    void ErrXit(int errnum, char const * description);
    Token ConvertTOKENtoToken (TOKEN old_style_tkn);

// SimPack-compatible c-language API:

    #include "capi.h"

/*=============================================================================
-----------------------------------------------------------------------------*/

// end queuing.h

