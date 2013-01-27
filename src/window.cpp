
#include "window.h"

Window::Window(int size)
{
	setSize(size);
}

void Window::setSize(int size)
{
	this->size = size; 
}

int Window::getSize()
{ 
	return this->size;
}

double Window::getMean()
{ 
	return this->sum / this->window.size();
}

std::queue<double> Window::getWindow()
{ 
	return this->window;
}

double Window::insertValue(double value)
{
	this->sum += value;

	if (window.size() >= size) {
		this->sum -= window.front();
		window.pop();
	}

	window.push(value);

	return getMean();
}


