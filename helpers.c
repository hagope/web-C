#include "helpers.h"

void init_string(struct string *s) {
    s->len = 0;
    s->ptr = malloc(s->len+1);
    if (s->ptr == NULL) {
      fprintf(stderr, "in init_string(), malloc() failed\n");
      exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    if (s->ptr == NULL) {
      fprintf(stderr, "realloc() failed\n");
      exit(EXIT_FAILURE);
    }
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size*nmemb;
}


/* This function will traverse the JSON
 * and return the first value that
 * matches the Key value,
 * otherwise returns NULL
*/
const char *json_get_first_value_from_key(char *json_str, char *in_key) {
    const char *retNull = NULL;

    json_tokener *tok;
    json_object *json_data_obj;
    tok = json_tokener_new();
    json_data_obj = json_tokener_parse(json_str);
    int stringlen = 0;
    enum json_tokener_error jerr;

    do {
        stringlen = strlen(json_str);
        json_data_obj = json_tokener_parse_ex(tok, json_str, stringlen);
    } while ((jerr = json_tokener_get_error(tok)) == json_tokener_continue);
    if (jerr != json_tokener_success)
    {
        fprintf(stderr, "Error: %s\n", json_tokener_error_desc(jerr));
        // Handle errors, as appropriate for your application.
    }
    if (tok->char_offset < stringlen) // XXX shouldn't access internal fields
    {
        // Handle extra characters after parsed object as desired.
        // e.g. issue an error, parse another object from that point, etc...
    }

    //printf("json_data_obj.to_string()=%s\n", json_object_to_json_string(json_data_obj));
    int objLen=0;
    int json_level=0;

    do {
        //printf("%d - *****************\n", json_level);
        /*Traverse the JSON
        * "results" => "channel" => "item" => "condition" => "temp"
        * http://stackoverflow.com/questions/29555192/parsing-deeply-nested-json-key-using-json-c
        */
        json_object_object_foreach(json_data_obj, key, val) {
            if(strcmp(key, in_key) == 0) {
                const char *ret = json_object_to_json_string(val);
                return ret;
            }
            //printf("%s\t : %s\n", key, json_object_to_json_string(val));
            json_data_obj = val;
        }
        objLen =  json_object_object_length(json_data_obj);
        //printf("object length: %d\n", objLen);
        json_level++;
    } while(objLen > 0 && objLen < 32512); /* objLen is 32512 at the last node */

    json_object_put(json_data_obj);
    return retNull;
}

