ok#include "queuing.h"
#include <stdio.h>
ok#include <math.h>
ok#define _USE_MATH_DEFINES
ok#include <queue>

ok#define DELIM	","

using namespace std;

okenum EventId {
ok	CHANGE_MEAN_ARRIVAL,
ok	ARRIVAL, 
ok	REQUEST_SERVER, 
ok	RELEASE_SERVER
ok};

okstruct Payload {
	double service_time;
ok	double arrival_time;	
ok};

okFacility * fqueue = NULL;

okvoid ChangeArrival();
okvoid Arrive();
okvoid RqstSrvr();
okvoid RlsSrvr();

ok// http://stackoverflow.com/questions/10209935/drawing-sine-wave-using-opencv
okint mean_sin_wave;
okfloat bound_sin_wave;
okfloat bound_rate;
okfloat freq_sin_wave;
okint change_number = 0;

okdouble mean_arrival;
okdouble mean_service;
float step_percentage;
okint window_size;
okstd::queue<double> window;
okint nreq;
okint req = 1;
okdouble sample_time;


okbool done = false;
okdouble sum = 0;

okint main (int argc, char *argv[])
ok{
	if (argc != 10) {
ok		cerr << "usage:\n\t" << argv[0] << " seed mean_arrival mean_service step_percentage window_size nreq theta bound_rate sample_time" << endl;
		cerr << "example:\n\t" << argv[0] << " 1010 40 35 1.35 5000 100000 0.015 0.05 5000" << endl;
		return 1;
	}

ok	long s = atoi(argv[1]);
ok	mean_arrival = atof(argv[2]);
ok	mean_service = atof(argv[3]);
	step_percentage = atof(argv[4]);
ok	window_size = atoi(argv[5]);
ok	nreq = atoi(argv[6]);
ok	freq_sin_wave = atof(argv[7]);
ok	bound_rate = atof(argv[8]);	
ok	sample_time = atof(argv[9]);
//cout << freq_sin_wave << " " << bound_rate << " "  << sample_time << endl;

ok	mean_sin_wave = mean_arrival;
ok	bound_sin_wave = mean_arrival * bound_rate;
//cout << bound_sin_wave << endl;

ok	new Future(LINKED);

ok	stream(1);
ok	seed(s, 1);

ok	fqueue = new Facility("queue");

ok	Token customer(1), change(0);
ok	Future::Schedule(CHANGE_MEAN_ARRIVAL, 0.0, change);
ok	Future::Schedule(ARRIVAL, 0.0, customer);

ok	while (!done) {
ok		Estatus es = Future::NextEvent();
ok		switch (es.event_id) {
ok			case CHANGE_MEAN_ARRIVAL:
ok				ChangeArrival();
ok				break;
ok			case ARRIVAL:
ok				Arrive();
ok				break;
ok			case REQUEST_SERVER: 
ok				RqstSrvr(); 
ok				break;
ok			case RELEASE_SERVER: 
ok//cout<<"before"<<change_number<<endl;
ok				RlsSrvr();  
ok				break;
ok			default: 
ok				ErrXit(1, "bad event_id");
ok		}
ok	} // end while

ok	Future::ReportStats();

ok	return 0; // pacify compiler
ok} // end main



/*=============================================================================
  Event routines
  -----------------------------------------------------------------------------*/

okvoid ChangeArrival()
ok{
ok	Token change = Future::CurrentToken();
ok	Future::UpdateArrivals();

ok	mean_arrival = mean_sin_wave + bound_sin_wave * sin(freq_sin_wave*M_PI*change_number);
ok	if (change_number > 0) {
ok		cerr << change_number << DELIM << mean_arrival << DELIM << Future::getUtilization() 
ok				<< DELIM << sum / window.size() <<  endl;
ok	}
ok	//cerr << mean_arrival << endl;
ok	change_number++;

ok	Future::Schedule(CHANGE_MEAN_ARRIVAL, sample_time, change);
	//Future::Schedule(CHANGE_MEAN_ARRIVAL, 5000, change);
ok}


okvoid Arrive()
ok{
ok	Token customer = Future::CurrentToken();
ok	Future::UpdateArrivals();

	// TODO:  onde colocar... no schedule do request ou do arrival
ok	struct Payload *pl = new struct Payload;
ok	pl->service_time = mean_arrival;
ok	pl->arrival_time = Future::SimTime();
ok	customer.SetPbox(pl);
ok	Future::Schedule(REQUEST_SERVER, 0.0, customer);

ok	if (req < nreq) {
		//Future::Schedule(ARRIVAL, expntl(mean_arrival), customer);
ok		customer.Id(customer.Id() + 1);
ok		Future::Schedule(ARRIVAL, mean_arrival, customer);
ok		req++;
ok	}
ok}

okvoid RqstSrvr()
ok{
ok	Token customer = Future::CurrentToken();
ok	if (fqueue->Request(customer) == FREE) {
ok		double service_time = expntl(mean_service);
ok		Future::Schedule(RELEASE_SERVER, service_time, customer);
ok	}
ok}

okvoid RlsSrvr()
ok{
ok	double response_time;

ok	Token customer = Future::CurrentToken();
ok	int who = customer.Id();

ok	struct Payload *pl = ((struct Payload *) customer.Pbox());
ok	response_time = Future::SimTime() - pl->arrival_time;
ok	sum += response_time;
cout << response_time << endl;

ok	if (req > window_size) {
ok		sum -= window.front();
ok		window.pop();
//cout<<"after"<<change_number<<endl;
ok	}
ok	window.push(response_time);
cout<<window.front()<< " " << window.size()<<endl;
	exit(0);


ok	//cerr << who << DELIM << response_time << DELIM << sum / window.size() << DELIM << pl->service_time;
ok	//cerr << who << DELIM << sum / window.size() << DELIM << Future::getUtilization() << DELIM << pl->service_time;
ok	//cerr << who << DELIM << sum / window.size() << DELIM << Future::getUtilization() << DELIM << mean_arrival;
ok	//cerr << pl->service_time;
ok	//cerr << Future::getUtilization();
ok	//cerr << endl;

ok	done = who == nreq;
ok	fqueue->Release(who);
ok	Future::UpdateDepartures();
ok}

// end ssq

