#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#include "config.h"


static cJSON *json;


/* Read a file, parse, render back, etc. */
static char *read_json(const char *filename)
{
	FILE *f;
	long len;
	char *data;
	
	if (!(f = fopen(filename, "rb"))) {
		fprintf(stderr, "[OOPS]: error while opening file '%s'.", 
			filename);
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	data = (char *) malloc(len + 1);
	fread(data, 1, len, f);
	fclose(f);

	return data;	
}


static cJSON *json_getvalue(const char *item) 
{
	return cJSON_GetObjectItem(json, item);
}


/* Parse text to JSON, then render back to text, and print! */
Configuration *load_configuration(const char *filename)
{
	Configuration *conf;

	if (!(json = cJSON_Parse(read_json(filename)))) {
		fprintf(stderr, "[OOPS]: error before: [%s]\n", 
			cJSON_GetErrorPtr());
	}

	conf = (Configuration *) malloc(sizeof(Configuration));
	conf->total_execution_time = 
		json_getvalue("total_execution_time")->valuedouble;
	conf->mean_service_time = 
		json_getvalue("mean_service_time")->valuedouble;
	conf->sample_time = json_getvalue("sample_time")->valuedouble;
	conf->mean_sine_wave = json_getvalue("mean_sine_wave")->valuedouble;
	conf->sine_amplitude = conf->mean_sine_wave * 
		json_getvalue("sine_amplitude")->valuedouble;
	conf->sine_wave_period = json_getvalue("sine_wave_period")->valuedouble;
	conf->changes_in_wave_period = 
		json_getvalue("changes_in_wave_period")->valueint;
	conf->response_time_window_size = 
		json_getvalue("response_time_window_size")->valueint;


}





