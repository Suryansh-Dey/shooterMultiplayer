#include <curl/curl.h>
#include <thread>
#include <atomic>
class State
{
public:
    float x, y;
    int movementAngle_degree, gunAngle_degree;
    std::vector<Shooter::Bullet> magazine;
    void copy(Shooter &player);
    void sync(Shooter &player);
};
void State::copy(Shooter &player) {}
void State::sync(Shooter &player) {}
class Client
{
    struct Match
    {
        bool active = false;
    };
    unsigned int id;
    CURL *curl;
    std::string serverURL, response;
    // Shared memory with clientThread!!
    State state1, state2;
    std::atomic<bool> clientThreadPermission = true, clientThreadInvoked = false;

    std::thread clientThread;

    static std::string encodeURL(State state);
    static State decodeURL(std::string state);
    static void clientThreadFunction(Client *client, unsigned int id);
    static size_t storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code);

public:
    Client(std::string serverURL);
    void joinRandom();
    inline unsigned int getId();
    void joinByCode(std::string code);
    Match getMatch();
    void sendAndRecieve(Shooter &player1, Shooter &player2);
    ~Client();
};

size_t Client::storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code)
{
    size_t total_size = size * nmemb;
    code->append(static_cast<char *>(contents), total_size);
    return total_size;
}
void Client::clientThreadFunction(Client *client, unsigned int id)
{
    CURL *curl = curl_easy_init();
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Client::storeToStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    while (client->clientThreadPermission)
    {
        if (client->clientThreadInvoked)
        {
            curl_easy_setopt(curl, CURLOPT_URL, ("http://localhost:3000/state/" + std::to_string(id)).c_str());
            response = "";
            curl_easy_perform(curl);
            std::cout << response << '\n';
            client->clientThreadInvoked = false;
        }
    }
}
Client::Client(std::string serverURL) : serverURL(serverURL)
{
    this->curl = curl_easy_init();
    curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/start");
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, Client::storeToStringCallback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &(this->response));
    CURLcode result = curl_easy_perform(this->curl);
    if (result != CURLE_OK)
    {
        fprintf(stderr, "Client::Client()::curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        exit(1);
    }
    this->id = std::stoi(this->response);
    clientThread = std::thread(this->clientThreadFunction, this, this->id);
}
void Client::joinRandom()
{
    curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/joinRandom");
    curl_easy_perform(this->curl);
}
unsigned int Client::getId()
{
    return this->id;
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
void Client::sendAndRecieve(Shooter &player1, Shooter &player2)
{
    while (Client::clientThreadInvoked)
    {
    }
    state1.copy(player1);
    state2.sync(player2);
    Client::clientThreadInvoked = true;
}
Client::~Client()
{
    this->clientThreadPermission = false;
    clientThread.join();
    curl_easy_setopt(this->curl, CURLOPT_URL, "http://localhost:3000/end");
    curl_easy_perform(this->curl);
}