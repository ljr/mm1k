/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

service.cpp
-----------------------------------------------------------------------------*/


#include "queuing.h"


/*=============================================================================
Max # of servers is limited only by available resources.
-----------------------------------------------------------------------------*/

Service::Service (int ns)
  {
  if (ns < 1) ErrXit (9141, "Bad ns");
  num_srvrs = ns;
  srvrunit = new Srvrunit [num_srvrs];
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

Service::~Service ()
  {
  delete [] srvrunit;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

int Service::FindIdle ()
  {
  int povider = num_srvrs;
  int ii;
  for (ii = 0; ii < num_srvrs; ii++)
    if (srvrunit[ii].tkn_id == FREE)
      { povider = ii; break; }
  return povider;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

int Service::FindSrvr (int tkn_id)
  {
  int povider = num_srvrs;
  int ii;
  for (ii = 0; ii < num_srvrs; ii++)
    if (srvrunit[ii].tkn_id == tkn_id)
      { povider = ii; break; }
  return povider;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Service::Engage (int srvr_i, int tkn_id, int pri)
  {
  if (srvr_i < 0 || srvr_i >=num_srvrs) ErrXit (9142, "Bad srvr_i");
  srvrunit [srvr_i] . tkn_id = tkn_id;
  srvrunit [srvr_i] . pri    = pri;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Service::Disengage (int srvr_i)
  {
  if (srvr_i < 0 || srvr_i >= num_srvrs) ErrXit (9143, "Bad srvr_i");
  srvrunit [srvr_i] . tkn_id = FREE;
  srvrunit [srvr_i] . pri    = 0;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

int Service::FindMinPriSrvr ()
  {
  int lo_pri_srvr = 0;
  int min_pri = srvrunit [lo_pri_srvr] . pri;
  int ii;
  for (ii = 1; ii < num_srvrs; ii++)
    {
    if (srvrunit [ii] . tkn_id == FREE) continue;
    if (srvrunit [ii] . pri < min_pri)
      {
      lo_pri_srvr = ii;
      min_pri = srvrunit [lo_pri_srvr] . pri;
      }
    }
  return lo_pri_srvr;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

int Service::GetPri (int srvr_i)
  {
  if (srvr_i < 0 || srvr_i >= num_srvrs) ErrXit (9144, "Bad srvr_i");
  return srvrunit [srvr_i] . pri;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

int Service::GetTokenID (int srvr_i)
  {
  if (srvr_i < 0 || srvr_i >= num_srvrs) ErrXit (9145, "Bad srvr_i");
  return srvrunit [srvr_i] . tkn_id;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Service::Display ()
  {
  int srvr_i;
  for (srvr_i = 0; srvr_i < num_srvrs; srvr_i++)
     cout << '(' << setw(2) << srvr_i << ')'
          << " TK " << setw(3) << GetTokenID (srvr_i)
          << " PR " << setw(2) << GetPri     (srvr_i) << ' ';
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

// end service.cpp

