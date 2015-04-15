/* weather.c
 * This C program demonstrates:
 * ✓ 1. fetch data from a web API using cURL;
 * ✓ 2. parse the JSON response using json-c;
 * ✓ 3. traverse the JSON and print certain keys;
 * ✓ 3a. routine for getting value from key;
 * □ 4. store the result in a SQLite database; and
 * ✓ 5. basic error checking.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "helpers.h"

/* Yahoo Weather API: https://developer.yahoo.com/weather/ */
/* TODO: De-hard-code the location */
/* this string can be modified for your location:
 * const static char *api_endpoint =   "https://query.yahooapis.com/v1/public/"
 *                                   "yql?q=select%20item.condition%20from%20"
 *                                   "weather.forecast%20where%20woeid%20in%20"
 *                                   "(select%20woeid%20from%20geo.places(1)%20"
 *                                   "where%20text%3D%22sunnyvale%2C%20ca%22)&format="
 *                                   "json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys";
*/

const static char *api_endpoint =   "https://query.yahooapis.com/v1/public/yql?q="
                                    "select%20item.condition%20from%20"
                                    "weather.forecast%20where%20woeid%20%3D%"
                                    "202502265&format=json&env=store%3A%2F%2F"
                                    "datatables.org%2Falltableswithkeys";
                                    
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

        curl_easy_setopt(curl, CURLOPT_URL, api_endpoint);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        //curl_easy_setopt(curl, CURLOPT_CAPATH , getenv("SSL_CERT_DIR"));

        /* puts("*** curl_easy_perform(curl) ***"); */
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
        } else {
            /* For proper error handling of JSON object:
             * https://json-c.github.io/json-c/json-c-0.12/doc/html/json__tokener_8h.html#a0d9a666c21879647e8831f9cfa691673
             */

            //printf("%s\n",s.ptr);
            
            printf("temp:%s\n", json_get_first_value_from_key(s.ptr, "temp"));                
            printf("code:%s\n", json_get_first_value_from_key(s.ptr, "code"));
            printf("text:%s\n", json_get_first_value_from_key(s.ptr, "text"));

            free(s.ptr);
    }
        curl_easy_cleanup(curl); 
    }
    
    return 0;
}
