#include "queuing.h"
#include <stdio.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <queue>

#define DELIM	","

using namespace std;

enum EventId {
	CHANGE_MEAN_ARRIVAL,
	ARRIVAL, 
	REQUEST_SERVER, 
	RELEASE_SERVER
};

struct Payload {
	double service_time;
	double arrival_time;	
};

Facility * fqueue = NULL;

void ChangeArrival();
void Arrive();
void RqstSrvr();
void RlsSrvr();

// http://stackoverflow.com/questions/10209935/drawing-sine-wave-using-opencv
int mean_sin_wave;
float bound_sin_wave;
float bound_rate;
float freq_sin_wave;
int change_number = 0;

double mean_arrival;
double mean_service;
float step_percentage;
int window_size;
std::queue<double> window;
int nreq;
int req = 1;
double sample_time;

bool done = false;
double sum = 0;

int main (int argc, char *argv[])
{
	if (argc != 10) {
		cerr << "usage:\n\t" << argv[0] << " seed mean_arrival mean_service step_percentage window_size nreq theta bound_rate sample_time" << endl;
		cerr << "example:\n\t" << argv[0] << " 1010 40 35 1.35 5000 100000 0.015 0.05 5000" << endl;
		return 1;
	}

	long s = atoi(argv[1]);
	mean_arrival = atof(argv[2]);
	mean_service = atof(argv[3]);
	step_percentage = atof(argv[4]);
	window_size = atoi(argv[5]);
	nreq = atoi(argv[6]);
	freq_sin_wave = atof(argv[7]);
	bound_rate = atof(argv[8]);	
	sample_time = atof(argv[9]);
//cout << freq_sin_wave << " " << bound_rate << " "  << sample_time << endl;

	mean_sin_wave = mean_arrival;
	bound_sin_wave = mean_arrival * bound_rate;
//cout << bound_sin_wave << endl;

	new Future(LINKED);

	stream(1);
	seed(s, 1);

	fqueue = new Facility("queue");

	Token customer(1), change(0);
	Future::Schedule(CHANGE_MEAN_ARRIVAL, 0.0, change);
	Future::Schedule(ARRIVAL, 0.0, customer);

	while (!done) {
		Estatus es = Future::NextEvent();
		switch (es.event_id) {
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
//cout<<"before"<<change_number<<endl;
				RlsSrvr();  
				break;
			default: 
				ErrXit(1, "bad event_id");
		}
	} // end while

	Future::ReportStats();

	return 0; // pacify compiler
} // end main



/*=============================================================================
  Event routines
  -----------------------------------------------------------------------------*/

void ChangeArrival()
{
	Token change = Future::CurrentToken();
	Future::UpdateArrivals();

	mean_arrival = mean_sin_wave + bound_sin_wave * sin(freq_sin_wave*M_PI*change_number);
	if (change_number > 0) {
		cerr << change_number << DELIM << mean_arrival << DELIM << Future::getUtilization() 
				<< DELIM << sum / window.size() <<  endl;
	}
	//cerr << mean_arrival << endl;
	change_number++;

	Future::Schedule(CHANGE_MEAN_ARRIVAL, sample_time, change);
	//Future::Schedule(CHANGE_MEAN_ARRIVAL, 5000, change);
}


void Arrive()
{
	Token customer = Future::CurrentToken();
	Future::UpdateArrivals();

	// TODO:  onde colocar... no schedule do request ou do arrival
	struct Payload *pl = new struct Payload;
	pl->service_time = mean_arrival;
	pl->arrival_time = Future::SimTime();
	customer.SetPbox(pl);
	Future::Schedule(REQUEST_SERVER, 0.0, customer);

	if (req < nreq) {
		//Future::Schedule(ARRIVAL, expntl(mean_arrival), customer);
		customer.Id(customer.Id() + 1);
		Future::Schedule(ARRIVAL, mean_arrival, customer);
		req++;
	}
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
	double response_time;

	Token customer = Future::CurrentToken();
	int who = customer.Id();

	struct Payload *pl = ((struct Payload *) customer.Pbox());
	response_time = Future::SimTime() - pl->arrival_time;
	sum += response_time;
cout << response_time << endl;

	if (req > window_size) {
		sum -= window.front();
		window.pop();
//cout<<"after"<<change_number<<endl;
	}
	window.push(response_time);
cout<<window.front()<< " " << window.size()<<endl;
	exit(0);


	//cerr << who << DELIM << response_time << DELIM << sum / window.size() << DELIM << pl->service_time;
	//cerr << who << DELIM << sum / window.size() << DELIM << Future::getUtilization() << DELIM << pl->service_time;
	//cerr << who << DELIM << sum / window.size() << DELIM << Future::getUtilization() << DELIM << mean_arrival;
	//cerr << pl->service_time;
	//cerr << Future::getUtilization();
	//cerr << endl;

	done = who == nreq;
	fqueue->Release(who);
	Future::UpdateDepartures();
}

// end ssq

