/* twitter.c
 * Get your tweets in C!
 * ✓ 1. read auth keys from keys.txt;
 * ✓ 2. cURL Twitter API endpoint for timeline;
 * ✓ 3. parse the JSON repsonse;
 * ✓ 4. sanitize tweets (strip url and escapes);
 * ✓ 5. store tweets in a text file;
 * □ 6. create a command line interface;
 * □ 7. basic error checking;
 *          □ a. check if files exist;
 *          □ b. if cURL fails, wait, try again;
 * □ 8. include favorite, retweet, endpoints;
 * □ 9. implement cURL calls rather than calling
 *      oauth deprecated functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oauth.h>
#include "helpers.h"

#ifdef DEBUG
    #define DEBUG_PRINT
#else
    #define DEBUG_PRINT for(;0;)
#endif

int putenv(char *string);

/* Attribution:
 * https://github.com/sigabrt/slurp/blob/8de8fc24d82279e755ce2179dceee5f4ca6df8c0/slurp.c#L277
 */
void read_auth_keys(const char *filename, int bufsize,
        char *ckey, char *csecret, char *atok, char *atoksecret)
{
    FILE *file = fopen(filename, "r");

    if(fgets(ckey, bufsize, file) == NULL)
    {
        return;
    }
    ckey[strlen(ckey)-1] = '\0'; // Remove the newline

    if(fgets(csecret, bufsize, file) == NULL)
    {
        return;
    }
    csecret[strlen(csecret)-1] = '\0';

    if(fgets(atok, bufsize, file) == NULL)
    {
        return;
    }
    atok[strlen(atok)-1] = '\0';

    if(fgets(atoksecret, bufsize, file) == NULL)
    {
        return;
    }
    atoksecret[strlen(atoksecret)-1] = '\0';

    fclose(file);
}

void process_reply(char *reply, char *out_file) {

    json_object *json_data_obj;
    json_data_obj = json_tokener_parse(reply);

    /* reply from Twitter API a JSON array 
     * TODO: validate the reply before parsing
     */

    int json_array_len = json_object_array_length(json_data_obj);
    int array_iter;

    FILE *f;
    if(out_file != NULL)
        f = fopen(out_file,"w");
    
    for(array_iter=0; array_iter<json_array_len; array_iter++)
    {
        json_object *temp_json_obj;
        temp_json_obj = json_object_array_get_idx(json_data_obj,array_iter);
        DEBUG_PRINT printf("json_data_obj.to_string()=%s\n", json_object_to_json_string(temp_json_obj));
        char *src;
        char *id;
        src = json_get_first_value_from_key(json_object_to_json_string(temp_json_obj), "text");
        id = json_get_first_value_from_key(json_object_to_json_string(temp_json_obj), "id");
        if(out_file != NULL) {
            fprintf(f,"%s\t%s\n", sanitize_tweet(src), id );
        } else {
            printf("%s\t%s\n", sanitize_tweet(src), id );
        }
        json_object_put(temp_json_obj);
    }

    json_object_put(json_data_obj);

    if(reply) free(reply);

    if(out_file != NULL)
        if(fclose(f)==0) 
            printf("tweets stored in %s\n", out_file);

}

/* If out_file is NULL, will print to stdout */
int oauth_get_tweets(int use_post, char *key_file, char *out_file) {
    const char *twitter_api_endpoint =  "https://api.twitter.com/1.1/"
                                        "statuses/home_timeline.json"
                                        "?count=30&include_entities=false"
                                        "&exclude_replies=true"
                                        "&trim_user=true"
                                        "&contributor_details=false";

    /* Use this endpoint and POST to update status */
    /* const char *twitter_api_endpoint =   "https://api.twitter.com/1.1/" */
                                            /* "statuses/update.json" */
                                            /* "?status=Aint%20twitter%20great"; */

    /* Attribution (next 22 lines):
     *  https://github.com/sigabrt/slurp/blob/8de8fc24d82279e755ce2179dceee5f4ca6df8c0/slurp.c#L41
    */
    // Read the OAuth keys
    // ===================
    // These may be found on your twitter dev page, under "Applications"
    // You will need to create a new app if you haven't already
    // The four keys should be on separate lines in this order:
    char *ckey, *csecret, *atok, *atoksecret;
    int bufsize = 64;
    ckey = (char *)malloc(bufsize * sizeof(char));
    csecret = (char *)malloc(bufsize * sizeof(char));
    atok = (char *)malloc(bufsize * sizeof(char));
    atoksecret = (char *)malloc(bufsize * sizeof(char));
    read_auth_keys(key_file, bufsize, ckey, csecret, atok, atoksecret);
    if(ckey == NULL || csecret == NULL ||
            atok == NULL || atoksecret == NULL)
    {
        fprintf(stderr, "Couldn't read key file. Aborting...\n");
        free(ckey);
        free(csecret);
        free(atok);
        free(atoksecret);
        return 1;
    }

    char *req_url = NULL;
    char *postarg = NULL;
    char *reply;

    if (use_post) {
        req_url = oauth_sign_url2(twitter_api_endpoint, &postarg, OA_HMAC, NULL, ckey, csecret, atok, atoksecret);
        reply = oauth_http_post(req_url,postarg);
        //reply = oauth_curl_post(req_url,postarg, NULL);
    } else {
        req_url = oauth_sign_url2(twitter_api_endpoint, NULL, OA_HMAC, NULL, ckey, csecret, atok, atoksecret);
        DEBUG_PRINT printf("req_url: %s\n", req_url);
        reply = oauth_http_get(req_url,NULL);
        //reply = oauth_curl_get(req_url, NULL, NULL), reply;
    }

    DEBUG_PRINT printf("query:'%s'\n",req_url);
    DEBUG_PRINT printf("reply:'%s'\n",reply);
    if(reply != NULL) {
        process_reply(reply, out_file);
    } else {
        puts("cant get tweets, trying again...");
        return(1);
    }

    return(0);
}

/*
 * compile:
 * gcc -lssl -loauth -o twitter twitter.c
 */

int main (int argc, char **argv) {
    /* TODO: Fix this: */
    /* curl_easy_setopt(curl, CURLOPT_CAPATH, capath); */
    putenv("CURLOPT_SSL_VERIFYPEER=0"); 
    /* try 3 times */
    if(oauth_get_tweets(0, "keys.txt", "/tmp/tweets.txt"))
        if(oauth_get_tweets(0, "keys.txt", "/tmp/tweets.txt"))
            if(oauth_get_tweets(0, "keys.txt", "/tmp/tweets.txt"))
                return EXIT_FAILURE; 

    return EXIT_SUCCESS;
}
