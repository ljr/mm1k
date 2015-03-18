#include <event/queuing.h>
#include <stdlib.h> // atoi, atof and friends...
#include <math.h>
#define _USE_MATH_DEFINES

#include "window.h"
#include "config.h"


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


Configuration *conf;


// event functions
void Sampling();
void ChangeArrival();
void Arrive();
void RqstSrvr();
void RlsSrvr();


// flow control
int req = 1;
double arrival_rate;


// metrics
double change_arrival_rate;
double last_response_time;
double last_utilization = .0;
Window *response_time; 
float b = 0.0;
float step;
Window *utilization;


int main(int argc, char *argv[])
{
	if (argc != 4) {
		cerr << "Usage:\n\t" << argv[0] << 
			" seed1 seed2 config.json" << endl;
		return 1;
	}

	s1 = atol(argv[1]);
	s2 = atol(argv[2]);
	conf = load_configuration(argv[3]);


	arrival_rate = conf->mean_sine_wave;
	step = (2*M_PI) / (float) conf->changes_in_wave_period;
	response_time = new Window(conf->response_time_window_size);
	// TODO: another parameter for utilization window size? I think not.
	utilization = new Window(conf->response_time_window_size); 
	change_arrival_rate = 
		conf->sine_wave_period / (float) conf->changes_in_wave_period;

	new Future(LINKED);
	fqueue = new Facility("queue");

	seed(s1, 1);
	seed(s2, 2);

	Token customer(1, new_payload(0.0)), change(0), sample(0);
	Future::Schedule(ARRIVAL, 0.0, customer);
	Future::Schedule(SAMPLING, conf->sample_time, sample);
	Future::Schedule(CHANGE_MEAN_ARRIVAL, change_arrival_rate, change);

	while (Future::SimTime() < conf->total_execution_time) {
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
	Future::Schedule(SAMPLING, conf->sample_time, sample);

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

	arrival_rate = conf->mean_sine_wave + conf->sine_amplitude * sin(b);
	b += step;

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
		double service_time = expntl(conf->mean_service_time);
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

        fqueue->Release(who);
        Future::UpdateDepartures();
}


struct Payload *new_payload(double arrival)
{
	struct Payload *pl = new struct Payload;

	pl->arrival_time = arrival;

	return pl;
}



