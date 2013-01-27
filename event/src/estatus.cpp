/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

estatus.cpp
-----------------------------------------------------------------------------*/

#include "queuing.h"

/*=============================================================================
Returns a string.  String is static literal.
-----------------------------------------------------------------------------*/

char * Estatus::ReturnCodeString ()
  {
  switch (rtncod)
    {
    case ES_OK       : return "ES_OK";
    case ES_NO_EVENT : return "ES_NO_EVENT";
    case ES_NO_FN    : return "ES_NO_FN";
    case ES_UNKNOWN  : return "ES_UNKNOWN";
    }
  return "?";
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Estatus::ES_DisplayReturnCode ()
  {
  cout << "------------------------------------------\n";
  cout << "In Event Status Block:\n";
  cout << " return code: " << ReturnCodeString() << '\n';
  cout << "------------------------------------------\n";
  }

/*=============================================================================
-----------------------------------------------------------------------------*/

void Estatus::ES_Display ()
  {
  cout << "-----------------------------------------------------------\n";
  cout << "Event Status Block:\n";
  cout << " event routine description: '" << descr << "'\n";
  cout << " return code: " << ReturnCodeString() << '\n';
  cout << " time: " << now << '\n';
  cout << "-----------------------------------------------------------\n";
  }


// end estatus.cpp

