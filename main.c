#include "conf.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <curl/curl.h>

// local structs 
typedef struct {
    uint8_t ipv4[4];
    uint8_t port;
} addr_t;
/**
    callback for write func
*/
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    return fwrite(ptr, size, nmemb, stream);
}

int main(void) 
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    // get file from NETwork
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(FILE_TO_SAVE,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, URL_DOWNLOAD);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
	// clean
        curl_easy_cleanup(curl);
    }
    // reopen file
    if(freopen(FILE_TO_SAVE, "r+", fp) == NULL) {
    	perror("err freopen NULL return");
        remove(FILE_TO_SAVE);
        exit(-1);
    }
    addr_t* addr_arr = malloc(sizeof (addr_t));
    if(addr_arr == NULL) {
        perror("malloc not work");
        fclose(fp);
        remove(FILE_TO_SAVE);
        exit(-1);
    }
    int items = 0;
    while(
            fscanf(fp, "%hhu.%hhu.%hhu.%hhu:%hhu",
                &addr_arr[items].ipv4[0],
                &addr_arr[items].ipv4[1],
                &addr_arr[items].ipv4[2],
                &addr_arr[items].ipv4[3],
                &addr_arr[items].port
                ) != EOF
            )
    {
        items++;
        addr_arr = realloc(addr_arr, (items+1)*sizeof(addr_t));

        if(addr_arr == NULL) {
            perror("realloc not work");
            fclose(fp);
            remove(FILE_TO_SAVE);
            exit(-1);
        }
    }
    // now choose rand addr
    srand(time(NULL)+getpid());
    int random_num = rand() % items + 0;
    printf("%hhu.%hhu.%hhu.%hhu:%hhu", 
            addr_arr[random_num].ipv4[0],
            addr_arr[random_num].ipv4[1],
            addr_arr[random_num].ipv4[2],
            addr_arr[random_num].ipv4[3],
            addr_arr[random_num].port
          );
    return 0;
}

