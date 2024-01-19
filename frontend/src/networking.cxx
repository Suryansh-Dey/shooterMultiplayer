#include <curl/curl.h>
#include <thread>
#include <atomic>
#include <sstream>
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
    CURL *curl;
    std::string response;
    static std::string serverURL;
    // Shared memory with clientThread!!
    State state1, state2;
    std::atomic<bool> clientThreadPermission = true, clientThreadInvoked = false;
    std::atomic<unsigned int> id = 0;

    std::thread clientThread;

    static std::string encodeURL(State state, unsigned int id);
    static State decodeURL(std::string URL, unsigned int id);
    static void clientThreadFunction(Client *client, std::string serverURL);
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

std::string Client::serverURL;

std::string Client::encodeURL(State state, unsigned int id)
{
    std::string data = Client::serverURL + "/update?id=" + std::to_string(id) + "&state=";
    data += std::to_string(state.x / float(SCREEN_WIDTH)) + " " + std::to_string(state.y / float(SCREEN_WIDTH)) + " " + std::to_string(state.movementAngle_degree) + " " + std::to_string(state.gunAngle_degree) + " ";
    // for(Shooter::Bullet bullet: state.magazine)
    // {
    //     data += std::to_string(bullet.x/float(SCREEN_WIDTH))+" ";
    //     data += std::to_string(bullet.y/float(SCREEN_HEIGHT))+" ";
    // }
    return data;
}
State Client::decodeURL(std::string URL, unsigned int id)
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
    state.x = val * SCREEN_WIDTH;
    data >> val;
    state.y = val * SCREEN_WIDTH;
    data >> state.movementAngle_degree;
    data >> state.gunAngle_degree;
    return state;
}
size_t Client::storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code)
{
    size_t total_size = size * nmemb;
    code->append(static_cast<char *>(contents), total_size);
    return total_size;
}
void Client::clientThreadFunction(Client *client, std::string serverURL)
{
    CURL *curl = curl_easy_init();
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Client::storeToStringCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    while (client->clientThreadPermission)
    {
        if (!client->clientThreadInvoked)
            continue;
        curl_easy_setopt(curl, CURLOPT_URL, (serverURL + "/update?id=" + std::to_string(client->id) + "&state=testing").c_str());
        response = "";
        curl_easy_perform(curl);
        std::cout << response << '\n';
        client->clientThreadInvoked = false;
    }
}
Client::Client(std::string serverURL)
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
    clientThread = std::thread(this->clientThreadFunction, this, Client::serverURL);
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