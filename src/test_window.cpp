#include <iostream>
#include <assert.h>
#include "window.h"

using namespace std;

int main(int argc, char *argv[])
{
	Window *w = new Window(3);

	cout << w->insertValue(3) << endl;
	assert(w->getMean() == 3.);

	cout << w->insertValue(5) << endl;
	assert(w->getMean() == 4.);

	cout << w->insertValue(7) << endl;
	assert(w->getMean() == 5.);

	cout << w->insertValue(6) << endl;
	assert(w->getMean() == 6.);

	delete w;

	w = new Window(1);

	cout << w->insertValue(3) << endl;
	assert(w->getMean() == 3.);

	cout << w->insertValue(1) << endl;
	assert(w->getMean() == 1.);

	cout << w->insertValue(7) << endl;
	assert(w->getMean() == 7.);

	cout << w->insertValue(5) << endl;
	assert(w->getMean() == 5.);

	delete w;

	return 0;
}


