#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>


/* Parse text to JSON, then render back to text, and print! */
void load_configuration(const char *filename);

/*  */
cJSON *json_getvalue(const char *item);


#endif

