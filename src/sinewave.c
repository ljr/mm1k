#include <stdio.h>
#include <math.h>
#define _USE_MATH_DEFINES



int main(int argc, char *argv[])
{
	float change_number;

	int mean_sin_wave = 40;
	float bound_sin_wave = mean_sin_wave * .5;
	float freq_sin_wave = 0.04;

	for (change_number = 0; change_number < 400; change_number++) {
		printf("%f\n", mean_sin_wave + bound_sin_wave * sin(freq_sin_wave*M_PI*change_number));
	}

	return 0;
}


