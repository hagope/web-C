/* twitter.c
 * Get your tweets in C!
 * ✓ 1. read auth keys from keys.txt;
 * ✓ 2. cURL Twitter API endpoint for timeline;
 * ✓ 3. parse the JSON repsonse;
 * ✓ 4. sanitize tweets (strip url and escapes);
 * □ 5. store tweets in a SQLite database; and
 * ✓ 5a. store tweets in a text file;
 * □ 6. basic error checking;
 * □ 7. include favorite, retweet, endpoints;
 * □ 8. implement cURL calls rather than calling
 *      oauth deprecated functions.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oauth.h>
#include "helpers.h"


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


int oauth_consumer_example(int use_post) {
    const char *twitter_api_endpoint = "https://api.twitter.com/1.1/statuses/home_timeline.json?count=10&include_entities=false&exclude_replies=true&trim_user=true&contributor_details=false";

    /* Use this endpoint and POST to update status */
    //const char *twitter_api_endpoint = "https://api.twitter.com/1.1/statuses/update.json?status=Aint%20twitter%20great";


    /* Attribution (next 22 lines):
     *  https://github.com/sigabrt/slurp/blob/8de8fc24d82279e755ce2179dceee5f4ca6df8c0/slurp.c#L41
    */
    // Read the OAuth keys
    // ===================
    // These may be found on your twitter dev page, under "Applications"
    // You will need to create a new app if you haven't already
    // The four keys should be on separate lines in this order:
    char *keyfile = "keys.txt";
    char *ckey, *csecret, *atok, *atoksecret;
    int bufsize = 64;
    ckey = (char *)malloc(bufsize * sizeof(char));
    csecret = (char *)malloc(bufsize * sizeof(char));
    atok = (char *)malloc(bufsize * sizeof(char));
    atoksecret = (char *)malloc(bufsize * sizeof(char));
    read_auth_keys(keyfile, bufsize, ckey, csecret, atok, atoksecret);
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
        printf("req_url: %s\n", req_url);
        reply = oauth_http_get(req_url,NULL);
        //reply = oauth_curl_get(req_url, NULL, NULL), reply;
    }

    printf("query:'%s'\n",req_url);
    //printf("reply:'%s'\n\n",reply);
    json_get_first_value_from_key(reply, "text");
    //printf("%s\n", tweet);
    
    if(req_url) free(req_url);
    if(postarg) free(postarg);

    if(reply) free(reply);

    return(0);
}

/**
 * Main Test and Example Code.
 * 
 * compile:
 * gcc -lssl -loauth -o twitter twitter.c
 */

int main (int argc, char **argv) {
  oauth_consumer_example(0);
  return(0);
}
