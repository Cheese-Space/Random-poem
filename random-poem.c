#include <stdio.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <time.h>
bool is_connected() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();
    CURLcode result;
    curl_easy_setopt(curl, CURLOPT_URL, "1.1.1.1");
    curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1);
    result = curl_easy_perform(curl);
    if (result == CURLE_COULDNT_CONNECT) {
        curl_easy_cleanup(curl);
	curl_global_cleanup();
        return false;
    }
    curl_easy_cleanup(curl);
    return true;
}
struct memory {
  char *response;
  size_t size;
};
static size_t cb(char *data, size_t size, size_t nmemb, void *clientp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)clientp;
  char *ptr = realloc(mem->response, mem->size + realsize + 1);
  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;

  return realsize;
}
int main() {
	if (is_connected() == false) {
		printf("connect to the internet to use this program!");
		return 1;
	}
	struct memory chunk = {0};
	CURLcode res;
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/Cheese-Space/Random-poem/refs/heads/main/Poems.json");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	cJSON *json = cJSON_Parse(chunk.response);
	free(chunk.response);
	srandom(time(NULL));
	cJSON *item = cJSON_GetArrayItem(json, random() % cJSON_GetArraySize(json));
	printf("%s\n", item->valuestring);
	cJSON_Delete(json);
	return 0;
}
