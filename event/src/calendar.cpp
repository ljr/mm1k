/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

calendar.cpp
-------------------------------------------------------------------------------
This class, Calendar, is one of a dozen or so derived classes of abstract base
class Vlist.  Class dfn in vlist.h.
-----------------------------------------------------------------------------*/

// Ported (and subsequently modified) from c-language version of simpack,
// which contained the following:

/*************************************************************************/
/**                                                                      */
/** Calendar Queueing Method for Future Event List Manipulation:         */
/**     As presented in the article by Randy Brown in Communications of  */
/** the ACM, Oct. 1988 Vol. 30 Num. 10.  Coded by Eric Callman 3/4/92    */
/**                                                                      */
/*************************************************************************/


#include "queuing.h"


/*=============================================================================
Constructor
 is based on linit and init in previous release of (c-language)
simpack.
-----------------------------------------------------------------------------*/

Calendar::Calendar (long cap, Boolean r_e)
  {
  if (cap < 1) ErrXit (9001, "Bad cap");
  capacity = cap;
  resize_enable = r_e;
  buckets = new Lnode * [capacity+1];
  int ii;
  for (ii = 0; ii <= capacity; ii++) buckets[ii] = NULL;
  Setup (0, 2, 1.0, 0.0);
  size = hi_wtr_mark = 0;
  }



/*=============================================================================
* Setup() WAS calendar_localinit(), which was called from calendar_init() and
  from calendar_resize().  See comment-header from old code below.
* calq array of size CALQSPACE was declared static and glocal in queuing.c
  Each element of that array was, in effect an Lnode.
* calendar used to be a ptr, initially set to the beginning of the calq array,
  subsequently addressed as the array itself, with subscript, and subject to
  change after a resize.  calendar pointed to a subrange of the calq array.
* previous_key used to be called lastprio
-----------------------------------------------------------------------------*/

/*************************************************************************/
/*                                                                       */
/* calendar_localinit:                                                   */
/*     This routine initializes a bucket array within the array calq[].  */
/* Calendar[0] is made equal to calq[qbase], the number of buckets is    */
/* nbuck, and startprio is the priority at which dequeing begins.  All   */
/* external variables except calresize_enable are initialized.           */
/*                                                                       */
/*************************************************************************/

void Calendar::Setup (int qbase, int nbuck, double bwidth, double startpri)
  {
  firstsub = qbase;   /* set position and size of new queue */
  nbuckets = nbuck;
  cal_width = bwidth;
  previous_key = startpri;  /* set up initial position */

  actv_bukts = buckets + qbase; /* initialize the new queue */
  int ii;
  for (ii = 0; ii < nbuckets; ii++) actv_bukts [ii] = NULL;

  if (cal_width == 0) ErrXit (9002, "cal_width is 0");
  long nnn = long ( double(previous_key) / cal_width); /* virtual bucket */
  previous_bucket = nnn % nbuckets;
  double d_nnn = double (nnn);
  bucket_top = (d_nnn+1.5) * cal_width; /* set up queue size change thresholds */
  double d_nbuckets = double (nbuckets);
  fold_threshold = long (d_nbuckets / 2.0) - 2L;
  if (fold_threshold < 0L) fold_threshold = 0L;
  split_threshold = 2L * nbuckets;
  ShowParms();
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Calendar::ShowParms ()
  {
  cout. setf(ios::fixed, ios::floatfield);
  cout. setf(ios::showpoint);
  cout. precision(1);
  cout << "*--------------------------- Calendar parameters -----------------------------*\n";
  cout << "Bucketspace capacity:" << setw(9) << capacity << "        ";
  cout << "Bucketspace address:" << setw(15) << buckets << '\n';

  cout << "Offset into bucketspace:" << setw(6) << firstsub << "        ";
  cout << "Active bucket array:" << setw(15) << actv_bukts << '\n';

  cout << "Resize enabled? " << setw(14) << (resize_enable ? "Y" : "N");
  cout << setw(25) << "# active buckets:"  << setw(18) << nbuckets << '\n';

  cout << "Split threshold:" << setw(14) << split_threshold << "        "
       << "Previous bucket:" << setw(19) << previous_bucket << '\n';

  cout << "Fold threshold:"  << setw(15) <<  fold_threshold << "        "
       << "Previous key:"    << setw(22) << previous_key << '\n';

  cout << "Calendar width: " << setw(14) << cal_width;
  cout << setw(19) << "Bucket top:" << setw(24) << bucket_top << '\n';
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

void Calendar::Insert (Event eee, int where)
  {
  if (where != TIME_KEY) ErrXit (9003, "bad where");
  Lnode * newnode = new Lnode ();
  (Event &) (* newnode) = eee;

  // calaculate bucket number to place new entry into

  if (cal_width == 0) ErrXit (9004, "cal_width is 0");
  long virtual_bkt = long ( newnode->Time() / cal_width );
  long bkt_i = virtual_bkt % nbuckets; /* find actual bucket */

  // grab head of list of events in that bucket
  Lnode * node_in_i = actv_bukts [bkt_i];

  // put in head of list if appropriate
  double newtime = newnode->Time();
  if ( (node_in_i == NULL) || (newtime < node_in_i->Time() ) )
    {
    actv_bukts [bkt_i] = newnode;
    newnode->next = node_in_i;
    }
  else // otherwise find the correct spot in list
    {
    Lnode * trailing_ptr = node_in_i;
    node_in_i = node_in_i->next;
    while ( node_in_i && (node_in_i->Time() <= newtime) )
      {
      trailing_ptr = node_in_i;
      node_in_i = node_in_i->next;
      }
    trailing_ptr->next = newnode;
    newnode->next = node_in_i;
    }

   size ++;
   if (size > hi_wtr_mark) hi_wtr_mark = size;

   if (size > split_threshold) // may or may not split -- see code in Resize()
      CalResize (SPLIT);  /* double the size of the calendar */
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

Event Calendar::Remove (int where, int id, void (*func) () )
  {
  Event eee;
  if (Empty () )
    {
    CannotRemoveFromEmpty (where);
    eee.EventId (NOT_FOUND);
    return eee;
    }

  switch (where)
    {
    case FROM_FRONT:
      {
      /* search for next entry */
      long ii = previous_bucket;  // starting here
      Lnode * node_in_i = NULL;
      Boolean found = NO;
      do_until_break
        {
        node_in_i = actv_bukts [ii];

        found = ( node_in_i && (node_in_i->Time() <= bucket_top) );
        if (found) break;

        ii++;                        // next active bucket
        if (ii >= nbuckets) ii = 0;  // with wrap
        bucket_top += cal_width;

        if (ii == previous_bucket)   // if we're back to where we started
          break;                     //  then it's time for direct search

        } // end while

      /* directly search for the event with lowest time */
      Boolean need_new_bucket_top = NO;
      if ( ! found)
        {
        // Have to run the loop below in a convoluted way because we can't just
        // initialize min to first element, as that may be null.  Instead, proceed
        // sequentially, & initialize from first non-null.
        long lo_guy = -1L;
        double lo_time = 9e99;
        for (ii = 0L; ii < nbuckets; ii++)
          {
          node_in_i = actv_bukts [ii];
          if (    (node_in_i != NULL)
               && ( (lo_guy == -1L) || (node_in_i->Time() < lo_time) ) )
            {
            lo_guy = ii;                  /* lowest entry so far */
            lo_time = node_in_i->Time();
            }
          }
        if (lo_guy < 0L) ErrXit (9005, "lo_guy never set");
        ii = lo_guy;
        node_in_i = actv_bukts [ii];
        need_new_bucket_top = YES;
        } // end if

      eee = (Event &) (* node_in_i);
      /* printf("extracting: %.1f(%d)\n",ent->time,ent->event); */
      actv_bukts [ii] = node_in_i->next;
      delete node_in_i;

      previous_bucket = ii;
      previous_key = eee.Time();

      if (need_new_bucket_top)
        bucket_top = double ( long (previous_key / cal_width) + 1L) * cal_width
                   + (0.5 * cal_width);
      break; // end case
      }

    case MATCHING_TOKEN_ID:
    case MATCHING_EVENT_ID:
    case MATCHING_EVENT_ROUTINE:
      {
      Lnode * node_in_i = NULL;
      Lnode * trailing_ptr = NULL;
      Boolean found = NO;
      Boolean first_one_in_bucket = NO;
      long ii;
      for (ii = 0L; (ii < nbuckets) && (! found); ii++)
        {
        node_in_i = actv_bukts [ii];
        if ( ! node_in_i) continue;

        switch (where)
          {
          case MATCHING_TOKEN_ID:      found = (node_in_i->Id()      == id  ); break;
          case MATCHING_EVENT_ID:      found = (node_in_i->EventId() == id  ); break;
          case MATCHING_EVENT_ROUTINE: found = (node_in_i->fn        == func);
          }
        if (found) { first_one_in_bucket = YES; break; }

        trailing_ptr = node_in_i;
        node_in_i = node_in_i->next;
        while (node_in_i)
          {
          switch (where)
            {
            case MATCHING_TOKEN_ID:      found = (node_in_i->Id()      == id  ); break;
            case MATCHING_EVENT_ID:      found = (node_in_i->EventId() == id  ); break;
            case MATCHING_EVENT_ROUTINE: found = (node_in_i->fn        == func);
            }
          if (found) break;
          trailing_ptr = node_in_i;
          node_in_i = node_in_i->next;
          } // end while
        } // end for

      if ( ! found)
         {
         cout << "I5916 Search failed\n";
         eee.EventId (ES_NO_EVENT);
         return eee;
         }
      eee = (Event &) (* node_in_i);
      ///previous_bucket = ii;
      ///previous_key = eee.Time();

      if (first_one_in_bucket)
        actv_bukts [ii] = node_in_i->next;
      else
        trailing_ptr->next = node_in_i->next;

      delete node_in_i;
      break; // end case
      }
    default:
      cout << "where = " << where << '\n';
      ErrXit (9006, "Bad where");
    } // end switch

  size--;
  if (size < fold_threshold)
    CalResize (FOLD);

  return eee;
  } // end fn



/*=============================================================================
-----------------------------------------------------------------------------*/

void Calendar::Display (int mode)
  {
  if (mode) ;
  double now = Future::SimTime();
  ShowSizeEtc (now);
  ShowParms ();
  cout. setf(ios::fixed, ios::floatfield);
  cout. setf(ios::showpoint);
  cout. precision(1);
  char egg [TINY_BFR_SIZ+1];
  long ii;
  for (ii = 0; ii < nbuckets; ii++)
    {
    Lnode * node_in_i = actv_bukts [ii];
    if ( ! node_in_i) continue;
    Boolean first = YES;
    while (node_in_i != NULL)
      {
      if (first)
        { cout << setw(6) << ii << ": "; first = NO; }
      else
        cout << "        ";
      cout << "[Token:" << setw(4) << node_in_i->Id()   << ",";
      cout << " Time:" << setw(10) << node_in_i->Time() << ",";
      cout << " Event:";
      int event_id = node_in_i->EventId();
      if (event_id >= 0)
        cout << event_id;
      else
        {
        strncpy(egg, node_in_i->FnDescr(), TINY_BFR_SIZ);
        egg [TINY_BFR_SIZ] = 0;
        cout << "'" << egg << "'";
        }
      cout << "]\n";
      node_in_i = node_in_i->next;
      }
    }
  }



/*=============================================================================
Header comment from legacy code from which CalResize() was ported appears
below:
-----------------------------------------------------------------------------*/

/*************************************************************************/
/*                                                                       */
/* calendar_resize:                                                      */
/*     This routine copies the queue into a calendar with the number of  */
/* buckets specified in the parameter newsize.  The new queue is in the  */
/* opposit end of calq from the original.                                */
/*                                                                       */
/*************************************************************************/

void Calendar::CalResize (ResizeAction resize_action)
  {
  if ( ! resize_enable) return;
  if (nbuckets > MAX_NBUCKETS) return;
  int newsize;

  switch (resize_action)
    {
    case SPLIT: newsize = nbuckets * 2L; break;
    case FOLD:  newsize = nbuckets / 2L; break;
    default: ErrXit (9007, "Bad resize_action");
    }

  double bwidth = NewWidth ();  /* find new bucket width */
  Lnode * * oldcalendar = actv_bukts;  /* save location & size of old calendar */
  long oldnbuckets = nbuckets;  /* for use when copying calendar */

  if (firstsub == 0)  /* initialize new calendar */
    Setup (capacity - newsize, newsize, bwidth, previous_key);
  else
    Setup (0                 , newsize, bwidth, previous_key);

  resize_enable = NO;
  long ii;
  for (ii = oldnbuckets - 1; ii >= 0; ii--)
    {    /* copy elements to new calendar */
    Lnode * node_in_i = oldcalendar [ii];
    while (node_in_i)
      { /* traverse linked list of elements */
      Event eee = (Event &) (* node_in_i);
      size--; /// i'm not proud of this, but it'll have to do for now
      Insert (eee, TIME_KEY); /* use insert routine for transfer */
      Lnode * sitcan = node_in_i;
      node_in_i = node_in_i->next;
      delete sitcan;
      }
    oldcalendar [ii] = NULL;
    }
  resize_enable = YES;
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

/*************************************************************************/
/*                                                                       */
/* new_cal_width:                                                        */
/*     This function returns the width that the buckets should have      */
/* based on a random sample of the queue so that there will be about 3   */
/* items in each bucket.                                                 */
/*                                                                       */
/*************************************************************************/

// Had to make event array global to elim the fol g++ warning:
//  cp -p calendar.cpp calendar.C
//  Compile methods of derived class Calendar of base class Vlist:
//  g++ -c -o calendar.o calendar.C
//  calendar.C: In method `double  Calendar::NewWidth ()':
//  calendar.C:462: warning: control reaches end of non-void function

  Event calendar_event_array [LARGEST_CALENDAR_SAMPLE];

double Calendar::NewWidth ()
  {
  if (size < 2L) return 1.0;

  long nsamples = size;
  if (size > 5L)
    nsamples = 5L + long ( float (size) / 10.0);
  if (nsamples > long (LARGEST_CALENDAR_SAMPLE) )
    nsamples = long (LARGEST_CALENDAR_SAMPLE);
  int sample_size = int (nsamples);

  Event * event_array = calendar_event_array; ///
  int actual_sample_count = ObtainSample (event_array, sample_size);
  if (actual_sample_count < 2) return 1.0;
  sample_size = actual_sample_count;
  double sum = 0.0;
  int ii;
  for (ii = 1; ii < sample_size; ii++)
    sum += event_array[ii].Time() - event_array[ii-1].Time();

  /// hmm ... isn't the above a telescoping sum?  if so, then its value is
  /// just the dif btwn the end pts.

  double average = sum / double (sample_size - 1);
  sum = 0.0;
  int jj = 0;
  for (ii = 1; ii < sample_size; ii++)
    {  // recalculate ignoring large separations
    double dif = event_array[ii].Time() - event_array[ii-1].Time();
    if (dif <= average * 2.0)
      {
      sum += dif;
      jj++;
      }
    }

  double newaverage = (jj < 1 ? average : sum / double (jj) * 3.0);

  if (newaverage < 1.0)
    newaverage = 1.0;

  return newaverage; // new width
  }



/*=============================================================================
This code replaces a former method which Remove()'d and re-Insert()'d elements.
The former method made the code quite complex to follow or debug; it was
expensive; and, it seemed to suffer from the "Mathieson bug", named after the
person who reported it (see correspondence thread 055).  Finally, i had trouble
getting the old code to work when porting it.

The code below fills in a sample array of events.  It returns the number of
events actually placed into the sample array, which possibly may be less than
the sample_size sought.  It's up to the caller to look at the return value &
decide what to do about it.
-----------------------------------------------------------------------------*/

int Calendar::ObtainSample (Event * sample_event_array, int sample_size)
  {
  if (sample_size > LARGEST_CALENDAR_SAMPLE)
    ErrXit (9008, "sample_size too big");
  Lnode * node_in_i = NULL;
  double bukt_top = bucket_top;
  long ii = previous_bucket;
  int collected = 0;
  do_until_break
    {
    node_in_i = actv_bukts [ii];

    while ( node_in_i && (node_in_i->Time() <= bukt_top) )
      {
      sample_event_array [collected++] = (Event &) (* node_in_i);
      if (collected >= sample_size) break; // out of while
      node_in_i = node_in_i->next;
      }
    if (collected >= sample_size) break;   // out of for

    ii++;                           // next active bucket
    if (ii >= nbuckets) ii = 0L;    // with wrap
    if (ii == previous_bucket)      // oops, back where we started -- done
      break;                        //  with loop, but sample is incomplete

    bukt_top += cal_width;

    } // end while

  return collected;
  }



/*=============================================================================
 Static fn for debug during development.
-----------------------------------------------------------------------------*/

void Calendar::Tester ()
  {
  cout << "Welcome to Calendar::Tester\n";
  init_simpack(CALENDAR);
  Future::SnapshotFlag (ON);
  Future::Interactive (YES);
  Token cow;  char descr [2]; strcpy(descr, "@");
  int arf;
  for (arf=0; arf < 16; arf++)
    {
    cow.Id (arf); descr [0]++;
    Future::Schedule (TestEvent, arf, cow, descr);
    }
  Estatus es;
  cout << '\n';
  Future::CancelToken (7);
  cout << '\n';
  do_until_break
    {
    es = Future::NextEvent();
    if (es.rtncod != ES_OK) break;
    cout << es.descr;
    }
  cout << "\nHave a nice day\n";
  exit(0);
  }



/*=============================================================================
static potected method, used with Tester above, just povides address, not
called.
-----------------------------------------------------------------------------*/

void Calendar::TestEvent (void)
  {
  cout << "Calendar test event routine\n";
  }



/*=============================================================================
-----------------------------------------------------------------------------*/

// end calendar.cpp


