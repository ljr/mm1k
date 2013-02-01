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
long s1, s2; // seed

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
float sine_wave_period;
float b = 0.0;
float step;
Window *utilization;


int main(int argc, char *argv[])
{
	if (argc != 11) {
		cout << "Usage:\n\t" <<  argv[0] << " 1seed 2seed 3n_requests(simtime) 4mean_service 5sample_time 6mean_sin_wave 7amplitude 8changes_in_wave_period 9rt_window_size 10sine_wave_period" << endl;
		cout << "Example:\n\t" <<  argv[0] << " 10 10 72000 28 300 40 .05 42 10 2100" << endl;
		cout << "Passed " << argc << " arguments." << endl;
		return 1;
	}

	s1 = atol(argv[1]);
	s2 = atol(argv[2]);
	deadline = atof(argv[3]);
	mean_service = atof(argv[4]);
	sample_time = atof(argv[5]);
	mean_sin_wave = atof(argv[6]);
	arrival_rate = mean_sin_wave;
	amplitude = mean_sin_wave * atof(argv[7]);
	changes_in_wave_period = atoi(argv[8]);
	step = (2*M_PI) / (float) changes_in_wave_period;
	response_time_size = atoi(argv[9]);
	response_time = new Window(response_time_size);
	utilization = new Window(response_time_size); // TODO: SHOULD CREATE A SPECIFIC PARAMETER FOR THIS? I THINK NOT.
	sine_wave_period = atof(argv[10]);
	change_arrival_rate = sine_wave_period / (float) changes_in_wave_period;

	new Future(LINKED);
	fqueue = new Facility("queue");

	seed(s1, 1);
	seed(s2, 2);

	Token customer(1, new_payload(0.0)), change(0), sample(0);
	Future::Schedule(ARRIVAL, 0.0, customer);
	Future::Schedule(SAMPLING, sample_time, sample);
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
			<< DELIM << last_response_time
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
	stream(1);
	Future::Schedule(ARRIVAL, expntl(arrival_rate), customer);
//		req++;
//	}
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



