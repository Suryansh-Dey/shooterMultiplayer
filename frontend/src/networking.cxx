#include <curl/curl.h>
#include <thread>
#include <atomic>
#include <sstream>
class State
{
public:
    float x, y;
    int headingAngle_degree, gunAngle_degree;
    std::vector<Shooter::Bullet> magazine;
    void copy(Shooter &player);
    void sync(Shooter &player);
};
void State::copy(Shooter &player)
{
    this->x = player.x;
    this->y = player.y;
    this->headingAngle_degree = player.headingAngle_degree;
    this->gunAngle_degree = player.gunAngle_degree;
}
void State::sync(Shooter &player)
{
    player.x = this->x;
    player.y = this->y;
    player.headingAngle_degree = this->headingAngle_degree;
    player.gunAngle_degree = this->gunAngle_degree;
}
class Client
{
    struct Match
    {
        bool active = false;
    };
    CURL *curl;
    std::string response;
    static std::string serverURL;
    // Shared memory with clientThread!!
    State state1, state2;
    std::atomic<bool> clientThreadPermission = true, clientThreadInvoked = false;
    std::atomic<unsigned int> id = 0;

    std::thread clientThread;

    static std::string encodeURL(State state, unsigned int id, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT);
    static State decodeURL(std::string URL, unsigned int id, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT);
    static void clientThreadFunction(Client *client, std::string serverURL, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT);
    static size_t storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code);

public:
    Client(std::string serverURL, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT);
    void joinRandom();
    inline unsigned int getId();
    void joinByCode(std::string code);
    Match getMatch();
    void sendAndRecieve(Shooter &player1, Shooter &player2);
    ~Client();
};

std::string Client::serverURL;

std::string Client::encodeURL(State state, unsigned int id, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT)
{
    std::string data = "/update?id=" + std::to_string(id) + "&state=";
    data += std::to_string(state.x / float(SCREEN_WIDTH)) + "," + std::to_string(state.y / float(SCREEN_HEIGHT)) + "," + std::to_string(state.headingAngle_degree) + "," + std::to_string(state.gunAngle_degree) + ",";
    // for(Shooter::Bullet bullet: state.magazine)
    // {
    //     data += std::to_string(bullet.x/float(SCREEN_WIDTH))+" ";
    //     data += std::to_string(bullet.y/float(SCREEN_HEIGHT))+" ";
    // }
    return data;
}
State Client::decodeURL(std::string URL, unsigned int id, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT)
{
    State state;
    uint16_t dataStartIdx;
    for (uint16_t idx = 0; idx < URL.length(); idx++)
    {
        if (URL[idx] == '&')
        {
            dataStartIdx = idx + 1 + 6;
            break;
        }
    }
    std::stringstream data(URL.substr(dataStartIdx));
    float val;
    data >> val;
    data.ignore();
    state.x = val * SCREEN_WIDTH;
    data >> val;
    data.ignore();
    state.y = val * SCREEN_HEIGHT;
    data >> state.headingAngle_degree;
    data.ignore();
    data >> state.gunAngle_degree;
    return state;
}
size_t Client::storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code)
{
    size_t total_size = size * nmemb;
    code->append(static_cast<char *>(contents), total_size);
    return total_size;
}
void Client::clientThreadFunction(Client *client, std::string serverURL, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT)
{
    CURL *curl = curl_easy_init();
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Client::storeToStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    while (client->clientThreadPermission)
    {
        if (!client->clientThreadInvoked)
            continue;
        // Processing...
        response = "";
        curl_easy_setopt(curl, CURLOPT_URL, (serverURL + Client::encodeURL(client->state1, client->id, SCREEN_WIDTH, SCREEN_HEIGHT)).c_str());
        curl_easy_perform(curl);
        client->state2 = Client::decodeURL(response, client->id, SCREEN_WIDTH, SCREEN_HEIGHT);

        client->clientThreadInvoked = false;
    }
}
Client::Client(std::string serverURL, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT)
{
    Client::serverURL = serverURL;
    this->curl = curl_easy_init();
    curl_easy_setopt(this->curl, CURLOPT_URL, serverURL.c_str());
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, Client::storeToStringCallback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &(this->response));
    CURLcode result = curl_easy_perform(this->curl);
    if (result != CURLE_OK)
    {
        fprintf(stderr, "Client::Client()::curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        exit(1);
    }
    clientThread = std::thread(this->clientThreadFunction, this, Client::serverURL, SCREEN_WIDTH, SCREEN_HEIGHT);
}
void Client::joinRandom()
{
    this->response = "";
    curl_easy_setopt(this->curl, CURLOPT_URL, (Client::serverURL + "/join?id=random").c_str());
    curl_easy_perform(this->curl);
    this->id = std::stoi(this->response);
}
void Client::joinByCode(std::string code)
{
    this->response = "";
    curl_easy_setopt(this->curl, CURLOPT_URL, (Client::serverURL + "/join?id=" + code).c_str());
    curl_easy_perform(this->curl);
    this->id = std::stoi(this->response);
}
unsigned int Client::getId()
{
    return this->id;
}
Client::Match Client::getMatch()
{
    this->response = "";
    curl_easy_setopt(this->curl, CURLOPT_URL, (Client::serverURL + "/isAvailable?id=" + std::to_string(id)).c_str());
    curl_easy_perform(this->curl);
    return Client::Match{response == "true"};
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
    curl_easy_setopt(this->curl, CURLOPT_URL, (Client::serverURL + "/quit?id=" + std::to_string(this->id)).c_str());
    curl_easy_perform(this->curl);
}