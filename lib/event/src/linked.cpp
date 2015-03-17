/*=============================================================================

   Sim++ version 1.01 is the property of Paul A. Fishwick and Robert M. Cubert,
   copyright (c) 1995; see license in the root directory of the distribution.

linked.cpp
-------------------------------------------------------------------------------
This class, Linked, is one of a dozen or so derived classes of abstract base
class Vlist.  Class dfn in vlist.h.
-----------------------------------------------------------------------------*/


#include "queuing.h"


/*=============================================================================
Insert into list.  The key determines whether insert is FIFO, LIFO, based on
time, or based on priority.  This fn replaces 2 fns in old simpack:
insert_list() and add_front_list().  The former is called many times in old
simpack.  The latter is called only once in old simpack, from facility
release() for non-preempted tokens; it is handled by the fn below with key =
AT_FRONT.

Altho FIFO insertion is NOT used in simpack as far as i can tell, i've povided
a "rear" ptr to expedite such insertion, in case i DO use it in future.
-----------------------------------------------------------------------------*/

void Linked::Insert (Event item, int key)
  {
  size ++;
  if (size > hi_wtr_mark) hi_wtr_mark = size;

  Lnode * new_node = new Lnode();
  (Event &) (* new_node) = item;

  Lnode * cur_node = front;
  Lnode * prv_node = front;

  // insert in list according to key
  switch (key)
    {
    case AT_FRONT: // ie: LIFO == Stack
      break;
    case AT_REAR:  // ie: FIFO == Queue
      cur_node = NULL;
      prv_node = rear;
      break;
    case TIME_KEY:
      while (cur_node && (cur_node->Time() <= item.Time() ))
        {
        prv_node = cur_node;
        cur_node = cur_node->next;
        }//end while
      break;
    case BEHIND_PRIORITY_KEY:
      while(cur_node && (cur_node->Priority() >= item.Priority() ))
        {
        prv_node = cur_node;
        cur_node = cur_node->next;
        }//end while
      break;
    case AHEAD_PRIORITY_KEY:
      while (cur_node && (cur_node->Priority() > item.Priority() ))
        {
        prv_node = cur_node;
        cur_node = cur_node->next;
        }//end while
      break;
    default:
      ErrXit (9131, "Bad key");
    } // end switch

  if ( ! cur_node)          // add new elm at rear
    {
    rear = new_node;
    if ( ! prv_node)            // insert into empty list
      front = new_node;
    else                        // list was NOT empty
      prv_node->next = new_node;
    }
  else                          // insert new elm between 2 existing elms
    {
    if (prv_node != cur_node)
      {                         // list HAD 2 or more items
      prv_node->next = new_node;
      new_node->next = cur_node;
      }
    else                        // insert new elm at front of non-empty list
      {
      front = new_node;
      new_node->next = cur_node;
      }
    }
  } // end fn

/*=============================================================================
Remove from list.  Remove FROM_FRONT for stacks and queues.  Remove
MATCHING_TOKEN_ID or MATCHING_EVENT_ID for removal from an arbitrary position.

This code contains the functionality of the fol old simpack fns:

  cancel_event()  API fn, called from application code, removing elm which
                  matched the specified event_id, and which returned NOT_FOUND
                  on failure; now check on return for event_id == NOT_FOUND.

  cancel_token()  API fn, called from application code, removing elm which
                  matched the specified token_id, and which returned NOT_FOUND
                  on failure; now check on return for event_id == NOT_FOUND.

  listrmqueue()   called from preempt(), removing elm which matched the
                  specified token id, and which took task exit on failure;
                  now check on return for event_id == NOT_FOUND.

  remove_front_list()  called from next_event(), release(), and end_simpack(),
                       to remove front elm, did NO CHECKING FOR EMPTY LIST,
                  probable cause of segmentation fault core dumps; now take
                  task exit on failure, ok.

-----------------------------------------------------------------------------*/

Event Linked::Remove (int where, int token_or_event_id, void (*func) () )
  {
  Event eee;

  if (Empty () )
    {
    CannotRemoveFromEmpty (where);
    eee.EventId (NOT_FOUND);
    return eee;
    }

  size--;

  Lnode * cur_node = front;
  Lnode * prv_node = NULL;
  switch (where)
    {
    case FROM_FRONT:
      break;
    case MATCHING_TOKEN_ID: // for CencelToken() and Preempt()
      while (cur_node && cur_node->Id() != token_or_event_id)
        {
        prv_node = cur_node;
        cur_node = cur_node->next;
        }
      break;
    case MATCHING_EVENT_ID:
      while (cur_node && cur_node->EventId() != token_or_event_id)
        {
        prv_node = cur_node;
        cur_node = cur_node->next;
        }
      break;
    case MATCHING_EVENT_ROUTINE:
      while (cur_node && cur_node->fn != func)
        {
        prv_node = cur_node;
        cur_node = cur_node->next;
        }
      break;
    default:
      ErrXit (9132, "Bad where");
    } // end switch

  if ( ! cur_node)
    {
    cout << "E9126 Token_or_event_id " << token_or_event_id
         << " not found in list\n";
    ErrXit (9133, "No match");
    }

  eee = (Event &) (* cur_node); // save info which is in node we remove
  if (cur_node == front)
    front = cur_node->next;
  else
    prv_node->next = cur_node->next;
  delete cur_node;

  // if removing front node, and front is same as rear, change rear ptr to
  // NULL because removed node was the ONLY node; otherwise change rear
  // ptr to trailing ptr when removing on id match a node other than the front
  // node.

  if (cur_node == rear)
    rear = prv_node;

  return eee; // return the info which was in the removed node
  }//end fn



/*=============================================================================
RemoveSel(): Remove seletivo. Retira da fila o primeiro elemento de uma
determinada prioridade (classe), percorrendo ate max posicoes. Caso nao 
o encontre, repete o procedimento para a classe imediatamente inferior.

Opcoes possiveis: FROM_FRONT
                  MATCHING_CLASS

-----------------------------------------------------------------------------*/

//Mario
Event Linked::RemoveSel (int where, int classe, int max)
  {
  Event eee;
  int i=0;

  if (Empty () )
    {
    CannotRemoveFromEmpty (where);
    eee.EventId (NOT_FOUND);
    return eee;
    }

  size--;

  Lnode * cur_node = front;
  Lnode * prv_node = NULL;
  switch (where)
    {
    case FROM_FRONT:
      break;
    case MATCHING_CLASS:
       // Retira da fila um elemento de uma determinada classe/prioridade
       do
       {
          // Procura por uma requisicao de uma certa classe
          // ate atingir max posicoes ou o fim da fila
          while (cur_node && cur_node->Priority() != classe && i < max)
          {
             prv_node = cur_node;
             cur_node = cur_node->next;
             i++;
	  }
	  // Nao encontrou elemento da classe procurada
	  // Inicia nova busca, agora na classe inferior
	  if (cur_node==NULL || i>=max)
	  {
	     classe--;
	     i = 0;
             cur_node = front;
             prv_node = NULL;
	  }
       } while (classe > 0 && cur_node->Priority() != classe);
       break;

    default:
      ErrXit (9132, "Bad where");
    } // end switch

  eee = (Event &) (* cur_node); // save info which is in node we remove
  if (cur_node == front)
    front = cur_node->next;
  else
    prv_node->next = cur_node->next;
  delete cur_node;

  if (cur_node == rear)
    rear = prv_node;

  return eee; // return the info which was in the removed node
  }//end fn


/*=============================================================================
There were 2 ways to display a FEL in the old simpack:

(1) One was in trace_update(), to "output a detailed graphical trace".
    This code supported FEL flavors LINKED, HEAP, and CALENDAR.  Some of
    the code from trace_update() was extracted and has become display mode
    1 in the fn below.  Output (for LINKED) looks like dis:

        ## EVENT LIST
                +-----+  +-----+  +-----+  +-----+
        Token   |00000|  |00000|  |00000|  |00000|
        Time  <=|000.0|  |000.0|  |000.0|  |000.0|
        Event   |00000|  |00000|  |00000|  |00000|
                +-----+  +-----+  +-----+  +-----+


(2) The other was in trace_eventlist(), which supported FEL flavor
    LINKED only, and whose entire body has become display mode 2 in
    the fn below.  Output (for LINKED) looks like dis:

        Time: 0.0
        Events: (TM 0.0,EV 0,TK 0) (TM 0.0,EV 0,TK 0) (TM 0.0,EV 0,TK 0)
        ------------------------------------------------------------

Calls from trace_update() and trace_eventlist() are via Future::DisplayFEL(),
which takes advantage of virtual fn to "select" a particular derived class
of the abstract base class Vlist.  DisplayFEL() is a method of
class Future because the event_list object (FEL ptr) lives in that class.
-----------------------------------------------------------------------------*/

void Linked::Display (int mode)
  {
  double now = Future::SimTime();
  ShowSizeEtc (now);
  if (Empty () ) return; // end of case

  switch (mode)
    {
    case 1:
      {
      int width = SetBoxWidth (now);

      int ii;
      for (ii=0; ii < 6; ii++) cout << ' ';
      Lnode * node = front;
      unsigned count = 0;
      while (node)
        {
        BoxEdge (width);
        count++;
        node = node->next;
        }
      cout << "\nToken ";
      node = front;
      cout. precision(1);
      while (node)
        {
        cout << "  |" << setw(width) << node->Id() << '|';
        node = node->next;
        }
      cout << "\nTime  ";
      node = front;
      while (node)
        {
        cout << "<=|" << setw(width) << node->Time() << '|';
        node = node->next;
        }
      cout << "\nEvent ";
      node = front;

      char stg [TINY_BFR_SIZ+1];
      if (width >= TINY_BFR_SIZ) ErrXit (9134, "Width too big");

      while (node)
        {
        cout << "  |";
        int evnt_id = node->EventId();
        if (evnt_id < 0)
          {
          char tmpstg [2 * TINY_BFR_SIZ + 1];
          strncpy(tmpstg, node->FnDescr(), width);
          strcat(tmpstg, "        ");
          tmpstg[width] = 0;
          cout << tmpstg;
          }
        else
          cout << setw(width) << evnt_id;
        cout << '|';
        node = node->next;
        }
      cout << "\n      ";
      node = front;
      for (ii=0; ii < count; ii++) BoxEdge (width);
      cout << '\n';
      break;
      }
    case 2:

      {
      cout << "Events: ";
      Lnode * node = front;
      while (node)
        {
        cout << "(TM " << node->Time() << ','; /// %3.1f
        cout << "EV " << setw(1) << node->EventId() << ',';
        cout << "TK " << setw(1) << node->Id() << ") ";
        cout << "        ";
        node = node->next;
        }
      cout << '\n';
      cout << "------------------------------------------------------------\n";
      break;
      }

    default: ErrXit (9135, "Bad mode");
    } // end switch
  } // end fn



/*=============================================================================
This used to be Facility::Trace()
-----------------------------------------------------------------------------*/

void Linked::Display (char * descr)
  {
  cout << "Display Linked '" << descr << "' queue";
  Display (1);
  cout << "\n------------------------------------------------------------\n";
  }



/*=============================================================================
Destructor ensures return to freepool of all elms which may still be in the
list.
-----------------------------------------------------------------------------*/

Linked::~Linked ()
  {
  Lnode * tmp;
  int limit = size;
  int ii;
  for (ii = 0; (ii < limit) && (front != NULL); ii++)
    {
    tmp = front->next;
    delete front;
    front = tmp;
    }
  }

/*=============================================================================
-----------------------------------------------------------------------------*/

// end linked.cpp

