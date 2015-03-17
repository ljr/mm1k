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


/* Parse text to JSON, then render back to text, and print! */
void load_configuration(const char *filename)
{
	if (!(json = cJSON_Parse(read_json(filename)))) {
		fprintf(stderr, "[OOPS]: error before: [%s]\n", 
			cJSON_GetErrorPtr());
	}
}


cJSON *json_getvalue(const char *item) 
{
	return cJSON_GetObjectItem(json, item);
}


