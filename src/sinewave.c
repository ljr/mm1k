#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES



int main(int argc, char *argv[])
{
	int change_number;

	float mean_sin_wave = atof(argv[1]);
	float amplitude = mean_sin_wave * atof(argv[2]);
	int number_of_points = atoi(argv[3]);

	float b = 0.0;
	float step = (2*M_PI) / (float) number_of_points;

	for (change_number = 0; change_number < number_of_points; change_number++) {
		printf("%f\n", mean_sin_wave + amplitude * sin(b));
		b += step;
	}

	return 0;
}


