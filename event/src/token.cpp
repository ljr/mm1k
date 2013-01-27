/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

token.cpp
-------------------------------------------------------------------------------
Methods for class Token
******************************************************************************/


#include "queuing.h"


// Static data members of class Token:

  unsigned Token::created = 0;
  unsigned Token::destroyed = 0;


/******************************************************************************
Constructor
-------------------------------------------------------------------------------
 If token id is provided, use it.  if AUTO is provided as token id, use token
serial number as id.  Serial number is guaranteed unique, so if user doesn't
care what id is as long as it's unique, he can use serial number (AUTO).
Token ID may be given a value or have its value changed any time after
construction.  Assigning in constructor is just a convenience.
 A pointer is available for user to connect a structure of her choice to be
carried around with a token.  Set to NULL by default.  Can be specified at
token constructor time, or later.
******************************************************************************/

Token::Token (int ii, void * pp)
  {
  created++; // static mbr of class Token

  id = (ii == AUTO) ? created : ii;

  pBox = pp;

  unsigned jj;
  for (jj=0; jj < MAX_NUM_RATTR; jj++)
    {
    rattr [jj] = 0.0;
    hasrdata [jj] = NO;
    }

  for (jj=0; jj < MAX_NUM_IATTR; jj++)
    {
    iattr [jj] = 0;
    hasidata [jj] = NO;
    }
  }



/******************************************************************************
Destructor
******************************************************************************/

Token::~Token ()
  {
  destroyed++;
  }



/******************************************************************************
Set a particular attribute.
-------------------------------------------------------------------------------
Attribute is selected by index, which is range-checked before use. If out of
range program terminates.  Otherwise set the selected attribute to the
specified value.
******************************************************************************/

void Token::SetTokenRattr (unsigned ii, double vv)
  {
  if (ii >= MAX_NUM_RATTR) // array size defined in qing.h
    ErrXit (9151, "Index out of range");
  rattr [ii] = vv;
  hasrdata [ii] = YES;
  }



/******************************************************************************
Get attribute.
-------------------------------------------------------------------------------
Attribute is selected by index, which is bounds-checked before use. If out of
range program terminates.  Otherwise return the selected attribute value.
******************************************************************************/

double Token::TokenRattr (unsigned ii)
  {
  if (ii >= MAX_NUM_RATTR) // array size defined in qing.h
    ErrXit (9152, "Index out of range");
  if ( ! hasrdata [ii])
    ErrXit (9153, "Attempt to use an attribute that has no value");
  return rattr [ii];
  }



/******************************************************************************
Set a particular attribute.
-------------------------------------------------------------------------------
Attribute is selected by index, which is range-checked before use. If out of
range program terminates.  Otherwise set the selected attribute to the
specified value.
******************************************************************************/

void Token::SetTokenIattr (unsigned ii, int aa)
  {
  if (ii >= MAX_NUM_IATTR)
    ErrXit (9154, "Index out of range");
  iattr [ii] = aa;
  hasidata [ii] = YES;
  }



/******************************************************************************
Get attribute.
-------------------------------------------------------------------------------
Attribute is selected by index, which is bounds-checked before use. If out of
range program terminates.  Otherwise return the selected attribute value.
******************************************************************************/

int Token::TokenIattr (unsigned ii)
  {
  if (ii >= MAX_NUM_IATTR)
    ErrXit (9155, "Index out of range");
  if ( ! hasidata [ii])
    {
    cout << "Token id " << id << '\n';
    cout << "Attribute #" << ii << '\n';
    ErrXit (9156, "Attempted use of integer attribute that has no value");
    }
  return iattr [ii];
  }//end fn



/******************************************************************************
Get indication of whether a particular attribute has data or not.
-------------------------------------------------------------------------------
index is bounds-checked before use. If out of
range program terminates.  Otherwise return the selected boolean value.
******************************************************************************/

Boolean Token::HasRdataIn (unsigned ii)
  {
  if (ii >= MAX_NUM_RATTR)
    ErrXit (9157, "Index out of range");
  return hasrdata [ii];
  }



/******************************************************************************
Get indication of whether a particular attribute has data or not.
-------------------------------------------------------------------------------
index is bounds-checked before use. If out of
range program terminates.  Otherwise return the selected boolean value.
******************************************************************************/

Boolean Token::HasIdataIn (unsigned ii)
  {
  if (ii >= MAX_NUM_IATTR)
    ErrXit (9158, "Index out of range");
  return hasidata [ii];
  }

// end token.cpp

