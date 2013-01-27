/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

flist.h
-----------------------------------------------------------------------------*/



/*=============================================================================
Facilities are doubly-linked into a list, to facilitate snapshots and
statistics.
-----------------------------------------------------------------------------*/

class Flist
  {
  public:
    Flist ();
  protected:
    void Append (class Facility * fff);
    void Unlink (class Facility * fff);
    class Facility * front;
    class Facility * rear;
    friend class Facility;      // facility can access flist members
  }; // end class


// end flist.h

