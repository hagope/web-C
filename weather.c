/* weather.c
 * This C program demonstrates:
 * ✓ 1. fetch data from a web API using cURL;
 * ✓ 2. parse the JSON response using json-c;
 * ✓ 3. traverse the JSON and print certain keys;
 * □ 4. store the result in a SQLite database; and
 * ✓ 5. basic error checking.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "json.h"
#include "json_tokener.h"

/* Yahoo Weather API: https://developer.yahoo.com/weather/ */
/* TODO: De-hard-code the location */
const static char *api_endpoint =   "https://query.yahooapis.com/v1/public/"
                                    "yql?q=select%20item.condition%20from%20"
                                    "weather.forecast%20where%20woeid%20in%20"
                                    "(select%20woeid%20from%20geo.places(1)%20"
                                    "where%20text%3D%22sunnyvale%2C%20ca%22)&format="
                                    "json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";
                                    
struct string {
  char *ptr;
  size_t len;
};

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

int main(int argc, char *argv[]) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    /* Get the response from the REST API
     * and store in the string struct;
     * this code is adapted from:
     * http://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
     */

    if(curl)
    {
        struct string s;
        init_string(&s);
        json_object *json_data_obj;

        curl_easy_setopt(curl, CURLOPT_URL, api_endpoint);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        puts("*** PERFORM CURL  ***");
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        } else {
                puts("*** BEGIN CURL RESPONSE ***");
                /* For proper error handling of JSON object:
                 * https://json-c.github.io/json-c/json-c-0.12/doc/html/json__tokener_8h.html#a0d9a666c21879647e8831f9cfa691673
                 */

                json_tokener *tok;
                tok = json_tokener_new();
                json_data_obj = json_tokener_parse(s.ptr);
                int stringlen = 0;
                enum json_tokener_error jerr;
                do {
                        stringlen = strlen(s.ptr);
                        json_data_obj = json_tokener_parse_ex(tok, s.ptr, stringlen);
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
                                if(strcmp(key,"code") == 0 || strcmp(key,"temp") == 0 || strcmp(key,"text") == 0)
                                        printf("%s\t : %s\n", key, json_object_to_json_string(val));
                                json_data_obj = val;
                        }
                        objLen =  json_object_object_length(json_data_obj);
                        //printf("object length: %d\n", objLen);
                        json_level++;
                } while(objLen > 0 && objLen < 32512); /* objLen is 32512 at the last node */

                json_object_put(json_data_obj);

                free(s.ptr);
                puts("*** END CURL RESPONSE ***");
        }
        curl_easy_cleanup(curl);
    }
    
    return 0;
}

