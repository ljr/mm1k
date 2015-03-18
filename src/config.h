#ifndef CONFIG_H
#define CONFIG_H


// configuration imported by json
typedef struct {
	double total_execution_time;
	double mean_service_time;
	double sample_time;
	double mean_sine_wave;
	double sine_amplitude;
	double sine_wave_period;
	int changes_in_wave_period;
	int response_time_window_size;
} Configuration;

/* Parse text to JSON, then render back to text, and print! */
Configuration *load_configuration(const char *filename);


#endif

