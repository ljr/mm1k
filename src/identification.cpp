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
double change_number = 0.0; // double: to rule the sine wave
double arrival_rate;

// metrics
double sample_time; // double: to pass to Future::Schedule
double change_arrival_rate;
double last_response_time;
double last_utilization = .0;
Window *response_time; 
int response_time_size;
double mean_service;
// http://stackoverflow.com/questions/10209935/drawing-sine-wave-using-opencv
int mean_sin_wave;
float bound_sin_wave;
float bound_rate;
float freq_sin_wave;
Window *utilization;


int main(int argc, char *argv[])
{
	if (argc != 10) {
		cout << "Usage:\n\t" <<  argv[0] << " seed n_requests(simtime) mean_service sample_time mean_sin_wave bound_rate freq_sin_wave rt_window_size change_arrival_rate" << endl;
		cout << "Example:\n\t" <<  argv[0] << " 1010 2000 28 300 40 .05 0.002 10 100" << endl;
		return 1;
	}

	s = atol(argv[1]);
//	nreq = atoi(argv[2]);
	deadline = atof(argv[2]);
	mean_service = atoi(argv[3]);
	sample_time = atof(argv[4]);
	mean_sin_wave = atoi(argv[5]);
	arrival_rate = mean_sin_wave;
	bound_rate = atof(argv[6]);
	bound_sin_wave = mean_sin_wave * bound_rate;
	freq_sin_wave = atof(argv[7]);
	response_time_size = atoi(argv[8]);
	response_time = new Window(response_time_size);
	utilization = new Window(response_time_size); // TODO: SHOULD CREATE A SPECIFIC PARAMETER FOR THIS? I THINK NOT.
	change_arrival_rate = atof(argv[9]);

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
/*
			<< DELIM << arrival_rate
			<< DELIM << last_response_time 
			<< DELIM << response_time->getMean() 
			<< DELIM << last_utilization
			<< DELIM << utilization->getMean() 
*/
			<< endl;
}

void ChangeArrival()
{
	Token change = Future::CurrentToken();
	Future::UpdateArrivals();
	Future::Schedule(CHANGE_MEAN_ARRIVAL, change_arrival_rate, change);


	change_number += freq_sin_wave;
	arrival_rate = mean_sin_wave + bound_sin_wave * sin(freq_sin_wave*M_PI);
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



