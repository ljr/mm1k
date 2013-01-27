#ifndef WINDOW_H
#define WINDOW_H
#include <queue>

class Window {

private:
	double sum;
	int size;
	std::queue<double> window;

public:
	Window(int size);

	void setSize(int size);
	int getSize();
	double getMean();
	std::queue<double> getWindow();

	double insertValue(double value);

};

#endif
