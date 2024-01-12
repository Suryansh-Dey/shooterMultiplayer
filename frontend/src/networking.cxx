#include <curl/curl.h>
class Client
{
private:
    size_t callback(void *contents, size_t size, size_t nmemb, std::string *output);
public:
    Client(const char* serverURL);
};

size_t Client::callback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    size_t total_size = size * nmemb;
    output->append(static_cast<char *>(contents), total_size);
    return total_size;
}
Client::Client(const char* serverURL)
{
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, serverURL);
}