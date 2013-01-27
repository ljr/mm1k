/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

token.h
-----------------------------------------------------------------------------
Dfn of class Token.  See also token.cpp.
****************************************************************************/

// symbolic name
const int AUTO = -1;
const int MAX_NUM_RATTR = 5;
const int MAX_NUM_IATTR = 3;

class Token
  {
  private:
    static unsigned created, destroyed;
  protected:
    int id; // may be provided by user or gen'd automatically
    double rattr[MAX_NUM_RATTR]; // real attributes
    Boolean hasrdata[MAX_NUM_RATTR];
    int iattr[MAX_NUM_IATTR]; // integer attributes
    Boolean hasidata[MAX_NUM_IATTR];
    void * pBox; // ptr to optional appl structure
  public:
  // if id == AUTO, use serialnumber as id.  Appl may attach any optional
  // structure it wishes.
  Token(int ii = AUTO, void * pp = NULL);

  ~Token(); // Destructor.

  int  Id (void)   {return id; }
  void Id (int ii) {id = ii; }

  // Return the value of a data member:
  void * Pbox () { return pBox; }
  Boolean HasRdataIn (unsigned ii); // does real attribute # ii have data?
  double TokenRattr (unsigned ii);  // value of real attribute # ii
  Boolean HasIdataIn (unsigned ii); // does integer attribute # ii have data?
  int TokenIattr (unsigned ii);     // value of integer attribute # ii


  // Token never looks at * pp but application presumably does.
  void SetPbox (void * pp) { pBox = pp; } ;

  // Set real attribute ii to value vv.
  void SetTokenRattr (unsigned ii, double vv);

  // Set integer attribute ii to value vv.
  void SetTokenIattr (unsigned ii, int vv);

  }; // end class

/****************************** End of file ********************************/

