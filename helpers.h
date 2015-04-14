#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "json_tokener.h"
#include "entities.h"

struct string {
    char *ptr;
    size_t len;
};

void init_string(struct string *s);

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s);

char *sanitize_tweet(char *src);

const char *json_get_first_value_from_key(char *json_str, char *in_key);
