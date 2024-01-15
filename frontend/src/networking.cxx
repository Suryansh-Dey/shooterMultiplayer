#include <curl/curl.h>
#include <thread>
#include <atomic>
class State
{
    public:
    float x, y;
    int movementAngle_degree, gunAngle_degree;
    std::vector<struct Bullet> magazine;
    void copy(Shooter &player);
    void sync(Shooter &player);
};
class Client
{
    struct Match
    {
        bool active = false;
    };
    int id;
    CURL *curl;
    std::string serverURL;
    static bool clientThread(State &state1, State &state2);
    static size_t storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code);

public:
    Client(std::string serverURL);
    void joinRandom();
    int generateCode();
    void joinByCode(std::string code);
    Match getMatch();
    State sendAndRecieve(State actions);
};

size_t Client::storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code)
{
    size_t total_size = size * nmemb;
    code->append(static_cast<char *>(contents), total_size);
    return total_size;
}
Client::Client(std::string serverURL) : serverURL(serverURL)
{
    this->curl = curl_easy_init();
    curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/start");
    CURLcode result = curl_easy_perform(this->curl);
    if (result != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        exit(1);
    }
}
void Client::joinRandom()
{
    curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/joinRandom");
    curl_easy_perform(this->curl);
}
int Client::generateCode()
{
    std::string code;
    curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/generateCode");
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, this->storeToStringCallback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &code);
    curl_easy_perform(this->curl);
    return std::stoi(code);
}
void Client::joinByCode(std::string code)
{
    curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/joinByCode?code=" + code);
    curl_easy_perform(this->curl);
}
Client::Match Client::getMatch()
{
    Client::Match match;
    return match;
}
State Client::sendAndRecieve(State actions)
{
}