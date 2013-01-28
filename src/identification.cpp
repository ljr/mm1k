#include "queuing.h"
#include "window.h"
#include <stdlib.h> // atoi, atof and friends...
#include <math.h>
#define _USE_MATH_DEFINES

#define DELIM ","

// simpack
Facility * fqueue = NULL;
enum EventId {
	CHANGE_MEAN_ARRIVAL,
	SAMPLING,
	ARRIVAL, 
	REQUEST_SERVER, 
	RELEASE_SERVER
};
long s; // seed

// payload to the token
struct Payload {
	double arrival_time; // simulation time token arrives
};
struct Payload *new_payload(double arrival);


// event functions
void Sampling();
void ChangeArrival();
void Arrive();
void RqstSrvr();
void RlsSrvr();

// flow control
//bool done = false;
//int nreq;
double deadline;
int req = 1;
double arrival_rate;

// metrics
double sample_time; // double: to pass to Future::Schedule
double change_arrival_rate;
double last_response_time;
double last_utilization = .0;
Window *response_time; 
int response_time_size;
double mean_service;
float mean_sin_wave;
float amplitude;
int changes_in_wave_period;
float b = 0.0;
float step;
Window *utilization;


int main(int argc, char *argv[])
{
	if (argc != 9) {
		cout << "Usage:\n\t" <<  argv[0] << " 1seed 2n_requests(simtime) 3mean_service 4sample_time 5mean_sin_wave 6amplitude 7changes_in_wave_period 8rt_window_size" << endl;
		cout << "Example:\n\t" <<  argv[0] << " 1010 2000 28 300 40 .05 42 10" << endl;
		return 1;
	}

	s = atol(argv[1]);
//	nreq = atoi(argv[2]);
	deadline = atof(argv[2]);
	mean_service = atof(argv[3]);
	sample_time = atof(argv[4]);
	mean_sin_wave = atof(argv[5]);
	arrival_rate = mean_sin_wave;
	amplitude = mean_sin_wave * atof(argv[6]);
	changes_in_wave_period = atoi(argv[7]);
	step = (2*M_PI) / (float) changes_in_wave_period;
	change_arrival_rate = 21000 / (float) changes_in_wave_period;
	response_time_size = atoi(argv[8]);
	response_time = new Window(response_time_size);
	utilization = new Window(response_time_size); // TODO: SHOULD CREATE A SPECIFIC PARAMETER FOR THIS? I THINK NOT.

	new Future(LINKED);
	fqueue = new Facility("queue");

	stream(1);
	seed(s, 1);

	Token customer(1, new_payload(0.0)), change(0), sample(0);
	Future::Schedule(SAMPLING, 0.0, sample);
	Future::Schedule(ARRIVAL, 0.0, customer);
	Future::Schedule(CHANGE_MEAN_ARRIVAL, change_arrival_rate, change);

//	while (!done) {
	while (Future::SimTime() < deadline) {
		Estatus es = Future::NextEvent();
		switch (es.event_id) {
			case SAMPLING:
				Sampling();
				break;
			case CHANGE_MEAN_ARRIVAL:
				ChangeArrival();
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
			<< DELIM << last_utilization
			<< endl;
}

void ChangeArrival()
{
	Token change = Future::CurrentToken();
	Future::UpdateArrivals();
	Future::Schedule(CHANGE_MEAN_ARRIVAL, change_arrival_rate, change);

	arrival_rate = mean_sin_wave + amplitude * sin(b);
	b += step;
}

void Arrive()
{
	Token customer = Future::CurrentToken();
	Future::UpdateArrivals();

	Future::Schedule(REQUEST_SERVER, 0.0, customer);

//	if (req < nreq) { // XXX: now for a period of time.
	customer.Id(customer.Id() + 1);
	customer.SetPbox(new_payload(Future::SimTime()));
	Future::Schedule(ARRIVAL, arrival_rate, customer);
//		req++;
//	}
}

void RqstSrvr()
{
	Token customer = Future::CurrentToken();
	if (fqueue->Request(customer) == FREE) {
		double service_time = expntl(mean_service);
		Future::Schedule(RELEASE_SERVER, service_time, customer);
	}
}

void RlsSrvr()
{
        Token customer = Future::CurrentToken();
        int who = customer.Id();

	struct Payload *pl = ((struct Payload *) customer.Pbox());
	last_response_time = Future::SimTime() - pl->arrival_time;
	response_time->insertValue(last_response_time);

//      done = who == nreq;
        fqueue->Release(who);
        Future::UpdateDepartures();
}


struct Payload *new_payload(double arrival)
{
	struct Payload *pl = new struct Payload;

	pl->arrival_time = arrival;

	return pl;
}



