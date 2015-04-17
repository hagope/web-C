#include "helpers.h"
#include <stdio.h>

#ifdef DEBUG
    #define DEBUG_PRINT
#else
    #define DEBUG_PRINT for(;0;)
#endif

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

/* http://stackoverflow.com/questions/779875/what-is-the-function-to-replace-string-in-c */
// You must free the result if result is non-NULL.
char *str_replace(char *orig, char *rep, char *with) {
    char *result;   // the return string
    char *ins;      // the next insert point
    char *tmp;      // varies
    int len_rep;    // length of rep
    int len_with;   // length of with
    int len_front;  // distance between rep and end of last rep
    int count;      // number of replacements

    if (!orig)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = orig;
    for(count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

/* Strip tweets of URLs, newlines
 * convert HTML codes
 */
char *sanitize_tweet(char *src)
{
    src++; src[strlen(src)-1]=0;
    char *srccpy = malloc(sizeof(char) * strlen(src) + 1);
    char *dst = malloc(sizeof(char) * strlen(src) + 1);
    strcpy(srccpy, src);
    decode_html_entities_utf8(dst, srccpy);

    free(srccpy);
    
    return dst;
}

/* This function will traverse the JSON
 * and return the first value that
 * matches the Key value,
 * otherwise returns NULL
 * For proper error handling of JSON object:
 * https://json-c.github.io/json-c/json-c-0.12/doc/html/json__tokener_8h.html#a0d9a666c21879647e8831f9cfa691673
*/
const char *json_get_first_value_from_key(char *json_str, char *in_key) {
    json_tokener *tok;
    json_object *json_data_obj;
    tok = json_tokener_new();
    int stringlen = 0;
    enum json_tokener_error jerr;

    do {
        stringlen = strlen(json_str);
        json_data_obj = json_tokener_parse_ex(tok, json_str, stringlen);
    } while ((jerr = json_tokener_get_error(tok)) == json_tokener_continue);
    if (jerr != json_tokener_success)
    {
        fprintf(stderr, "Error: %s\n", json_tokener_error_desc(jerr));
        /* Handle errors, as appropriate for your application. */
    }
    if (tok->char_offset < stringlen) // XXX shouldn't access internal fields
    {
        /* Handle extra characters after parsed object as desired. */
        /* e.g. issue an error, parse another object from that point, etc... */
    }

    DEBUG_PRINT printf("json_data_obj.to_string()=%s\n", json_object_to_json_string(json_data_obj));

    int objLen=0;
    const char *ret;
    do {
        /* Traverse the JSON */
        json_object_object_foreach(json_data_obj, key, val) {
            if(strcmp(key, in_key) == 0) {
                ret = json_object_to_json_string_ext(val,JSON_C_TO_STRING_PRETTY );
                return ret;
           }
            DEBUG_PRINT printf("%s\t : %s\n", key, json_object_to_json_string(val));
        } 
        json_data_obj = val;
        objLen =  json_object_object_length(val);
        DEBUG_PRINT printf("object length: %d\n", objLen);
    } while(objLen > 0 && objLen < 32512); /* objLen is 32512 at the last node */

    json_object_put(json_data_obj);
    return ret;
}
