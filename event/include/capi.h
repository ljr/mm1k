/*=============================================================================

   Sim++ version 1.0 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

capi.h
-------------------------------------------------------------------------------
This is the legacy c-language simpack application programming interface.

Many of these fns had no prototype declarations.  Hence the C-language default
was taken, which is:  fn w/o prototype decl is presumed to be "int f (void);".
For compatibility, i preserved this convention, altho in most cases, nothing
gets returned in the legacy code.  To pacify the compiler under such
circumstances, i usually return 0, but in several cases i've used the
opportunity to pass back something meaningful.  See for example next_event(),
update_arrivals(), and update_completions().
-----------------------------------------------------------------------------*/

// extract info from init_simpack()'s parameter:

    const int FEL_TYPE_MASK     = 0x000f; // new
    const int REMOVE_DUP_MASK   = 0x0010; // orig REMOVE_DUPLICATES was pound-defined as 16 but not used
    const int REMOVE_DUPLICATES = 0x0010; // for compat w/simpack appl code qnet
    const int RESEED_MASK       = 0x0020; // orig RESEED was pound-defined as 32 but not used
    const int RESEED            = 0x0020; // for compat w/simpack

// Group 1: related to class Future:

    int init_simpack (int p1);
    int trace_visual (int mode);
    int schedule (int evnt,float inter_time,TOKEN old_style_tkn);
    int next_event (int * event_id, TOKEN * old_style_token_ptr);
    float sim_time (void);
    int update_arrivals (void);
    int update_completions (void);
    int trace_eventlist (void);
    int trace_eventlist_size (void);
    int cancel_event (int event_id);
    int cancel_token (TOKEN old_style_token);
    int report_stats (void);
    void end_simpack (void);

// 2. Facility-related:

    int create_facility (char * name, int number_of_servers);
    int request (int facility_id, TOKEN old_style_tkn, int pri);
    int preempt (int facility_id, TOKEN old_style_tkn, int pri);
    int release (int facility_id, TOKEN old_style_tkn);
    int status (int facility_id);
    int facility_size (int facility_id);
    float busy_time (int facility_id);
    int trace_facility (int facility_id);

// Group 3: random

    int stream (int n);
    double ranf (void);
    long seed (long Ik, int n);
    double uniform (double a, double b);
    int random (int i, int n);
    double expntl (double x);
    double erlang (double x, double s);
    double hyperx (double x, double s);
    double triang (double a, double c, double b);
    double normal (double x, double s);

// end capi.h

