/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

capifac.cpp
-------------------------------------------------------------------------------
simpack c-language application progrmamming interface
Part 2: facility-related.
-----------------------------------------------------------------------------*/


#include "queuing.h"


/************************************************************************/
/*                                                                      */
/* create_facility:                                                     */
/*      Setup a facility with a given number of servers.                */
/*                                                                      */
/************************************************************************/
int create_facility (char * Name, int nsrvrs)
  {
  int facility_id = Facility::GetAnAvailable (Name, nsrvrs);
  return facility_id;
  }



/************************************************************************/
/*                                                                      */
/* request:                                                             */
/*      Request the use of a given queue.                               */
/*                                                                      */
/************************************************************************/
int request (int facility_id, TOKEN old_style_tkn, int pri)
  {
  Token new_style_tkn = ConvertTOKENtoToken (old_style_tkn);
  Facility * ptr = Facility::GetPtr (facility_id);
  FacStatus status = ptr->Request (new_style_tkn, pri);
  return int (status);
  } // end fn



/************************************************************************/
/*                                                                      */
/* preempt:                                                             */
/*      Seize control of the given queue for the given token.           */
/*                                                                      */
/************************************************************************/
int preempt (int facility_id, TOKEN old_style_tkn, int pri)
  {
  Token new_style_tkn = ConvertTOKENtoToken (old_style_tkn);
  Facility * ptr = Facility::GetPtr (facility_id);
  FacStatus rtncod = ptr->Preempt (new_style_tkn, pri);
  return int (rtncod);
  }



/************************************************************************/
/*                                                                      */
/* release:                                                             */
/*      Release server to waiting tokens (if any)                       */
/*                                                                      */
/************************************************************************/
int release (int facility_id, TOKEN old_style_tkn)
  {
  Facility * ptr = Facility::GetPtr (facility_id);
  int tkn_id = int (old_style_tkn.attr[0]); // ID is all we need from token
  ptr->Release (tkn_id);
  return 0;
  }



/************************************************************************/
/*                                                                      */
/* busy_time:                                                           */
/*      Provide total accumulated busy time for a facility.             */
/*                                                                      */
/************************************************************************/
float busy_time(int facility_id)
  {
  Facility * ptr = Facility::GetPtr (facility_id);
  double value = ptr->InuseTime ();
  return float (value);
  }



/************************************************************************/
/*                                                                      */
/* status:                                                              */
/*      Return facility status.                                         */
/*                                                                      */
/************************************************************************/
int status (int facility_id)
  {
  Facility * ptr = Facility::GetPtr (facility_id);
  FacStatus rtncod = ptr->Status ();
  return int (rtncod);
  }



/************************************************************************/
/*                                                                      */
/* facility_size:                                                       */
/*      Return facility queue size.                                     */
/*                                                                      */
/************************************************************************/
int facility_size (int facility_id)
  {
  Facility * ptr = Facility::GetPtr (facility_id);
  int value = ptr->FQlength ();
  return value;
  }



/************************************************************************/
/*                                                                      */
/* trace_facility:                                                      */
/*      Output information about the given facility.                    */
/*                                                                      */
/************************************************************************/
int trace_facility (int facility_id)
  {
  Facility * ptr = Facility::GetPtr (facility_id);
  ptr->FacDisplay (Future::Interactive());
  return 0;
  }


// end capifac.cpp

