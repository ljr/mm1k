#include <queuing.h>
	#include "window.h"
#include <stdlib.h> // atoi, atof and friends...
#include <assert.h>

#define DELIM ","

// simpack
Facility * fqueue = NULL;
enum EventId {
	SAMPLING,
	CHANGE_MEAN_ARRIVAL,
	ARRIVAL, 
	REQUEST_SERVER, 
	RELEASE_SERVER
};
long s1, s2; // seeds

// payload to the token
struct Payload {
	double arrival_time; // simulation time token arrives
};
struct Payload *new_payload(double arrival);


// event functions
void Change();
void Sampling();
void Arrive();
void RqstSrvr();
void RlsSrvr();

// flow control
double deadline;
double arrival_rate;
// metrics
double sample_time; // double: to pass to Future::Schedule
double last_response_time;
double last_utilization = .0;
Window *response_time; 
int response_time_size;
double mean_service;
double change;
float step;
Window *utilization;


int main(int argc, char *argv[])
{
	if (argc != 10) {
		cout << "Usage:\n\t" <<  argv[0] << " 1seed1 2seed2 3n_requests(simtime) 4mean_service 5sample_time 6mean_arrival_rate 7rt_window_size 8change 9step" << endl;
		cout << "Example:\n\t" <<  argv[0] << " 10 10 300000 28 300 40 10 100000 4" << endl;
		cout << "Passed " << argc << " arguments." << endl;
		return 1;
	}

	s1 = atol(argv[1]);
	s2 = atol(argv[2]);
	deadline = atof(argv[3]);
	mean_service = atof(argv[4]);
	sample_time = atof(argv[5]);
	arrival_rate = atof(argv[6]);
	response_time_size = atoi(argv[7]);
	response_time = new Window(response_time_size);
	utilization = new Window(response_time_size); // TODO: SHOULD CREATE A SPECIFIC PARAMETER FOR THIS? I THINK NOT.
	change = atof(argv[8]);
	step = atof(argv[9]);

	new Future(LINKED);
	fqueue = new Facility("queue");

	seed(s1, 1);
	seed(s2, 2);

	Token customer(3, new_payload(0.0)), chang(2), sample(1);
	Future::Schedule(ARRIVAL, 0.0, customer);
	Future::Schedule(SAMPLING, sample_time, sample);
	Future::Schedule(CHANGE_MEAN_ARRIVAL, change, chang);

	while (Future::SimTime() < deadline) {
		Estatus es = Future::NextEvent();
		switch (es.event_id) {
			case SAMPLING:
				Sampling();
				break;
			case CHANGE_MEAN_ARRIVAL:
				Change();
				break;
			case ARRIVAL:
				Arrive();
				break;
			case REQUEST_SERVER:
				RqstSrvr();
				break;
			case RELEASE_SERVER:
				RlsSrvr();
				break;
			default:
				ErrXit(1, "bad event_id");
		}
	}


	Future::ReportStats();
	delete utilization, response_time;
	return 0;
}

void Change()
{
	Token chang  = Future::CurrentToken();
	Future::UpdateArrivals();
	arrival_rate += step;
}


void Sampling()
{
	Token sample = Future::CurrentToken();
	Future::UpdateArrivals();
	Future::Schedule(SAMPLING, sample_time, sample);

	// sample utilization
	utilization->insertValue(Future::getUtilization());
	last_utilization = Future::getUtilization();

	cerr 
			<< Future::SimTime()
			<< DELIM << arrival_rate 
			<< DELIM << response_time->getMean()
			<< endl;
}

void Arrive()
{
	Token customer = Future::CurrentToken();
	Future::UpdateArrivals();

	Future::Schedule(REQUEST_SERVER, 0.0, customer);

	customer.Id(customer.Id() + 1);
	customer.SetPbox(new_payload(Future::SimTime()));
	stream(1);
	Future::Schedule(ARRIVAL, expntl(arrival_rate), customer);
}

void RqstSrvr()
{
	Token customer = Future::CurrentToken();
	if (fqueue->Request(customer) == FREE) {
		stream(2);
		double service_time = expntl(mean_service);
		Future::Schedule(RELEASE_SERVER, service_time, customer);
	}
}

void RlsSrvr()
{
        Token customer = Future::CurrentToken();
        int who = customer.Id();

	struct Payload *pl = ((struct Payload *) customer.Pbox());
	assert(pl != NULL);
	last_response_time = Future::SimTime() - pl->arrival_time;
	response_time->insertValue(last_response_time);

        fqueue->Release(who);
        Future::UpdateDepartures();
}


struct Payload *new_payload(double arrival)
{
	struct Payload *pl = new struct Payload;

	pl->arrival_time = arrival;

	return pl;
}



