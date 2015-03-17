/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

util.cpp
-----------------------------------------------------------------------------*/


#include "queuing.h"


/*=============================================================================
-----------------------------------------------------------------------------*/

void DynAllocFail ()
  {
  cout << "\nFATAL ERROR #9000 Dynamic allocation failure\n";
  exit(9000);
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void ErrXit (int errnum, char * stg1)
  {
  if (errnum > 0) // can exit without error # prefix if prefix is 0.
    cout << "FATAL ERROR #" << setw(4) << errnum << ' ';
  cout << stg1 << '\n';
  exit(errnum);
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

Token ConvertTOKENtoToken (TOKEN old_style_tkn)
  {
  Token new_style_tkn;
  int k_old = MAX_NUM_ATTR;
  int k_new = MAX_NUM_RATTR;
  if (k_new < k_old)
    ErrXit (9171, "Can't convert: old TOKEN bigger than new Token");
  int ii;
  for (ii=0; ii < MAX_NUM_ATTR; ii++)
    new_style_tkn.SetTokenRattr (ii, old_style_tkn.attr[ii]);
  int tkn_id = int (old_style_tkn.attr [0]);
  new_style_tkn.Id (tkn_id);
  return new_style_tkn;
  }


/*=============================================================================
-----------------------------------------------------------------------------*/

// end util.cpp

