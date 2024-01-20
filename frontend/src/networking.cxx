#include <curl/curl.h>
#include <thread>
#include <atomic>
#include <sstream>
class State
{
public:
    bool gameOn = true;
    float x, y;
    int headingAngle_degree, gunAngle_degree;
    std::vector<Shooter::Bullet> magazine;
    unsigned int time;
    void copy(Shooter &player);
    void sync(Shooter &player);
};
void State::copy(Shooter &player)
{
    this->x = player.x;
    this->y = player.y;
    this->headingAngle_degree = player.headingAngle_degree;
    this->gunAngle_degree = player.gunAngle_degree;
    this->time = player.time;
    this->magazine = player.magazine;
}
void State::sync(Shooter &player)
{
    player.x = this->x;
    player.y = this->y;
    player.headingAngle_degree = this->headingAngle_degree;
    player.gunAngle_degree = this->gunAngle_degree;
    player.time = this->time;
    player.magazine = this->magazine;
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
    static State decodeURL(std::stringstream data, unsigned int id, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT);
    static void clientThreadFunction(Client *client, std::string serverURL, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT);
    static size_t storeToStringCallback(void *contents, size_t size, size_t nmemb, std::string *code);

public:
    Client(std::string serverURL, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT);
    void joinRandom();
    inline unsigned int getId();
    void joinByCode(std::string code);
    Match getMatch();
    bool sendAndRecieve(Shooter &player1, Shooter &player2);
    ~Client();
};

std::string Client::serverURL;

std::string Client::encodeURL(State state, unsigned int id, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT)
{
    std::string data = "id=" + std::to_string(id) + "&state=";
    data += std::to_string(state.x / float(SCREEN_WIDTH)) + "," + std::to_string(state.y / float(SCREEN_HEIGHT)) + "," + std::to_string(state.headingAngle_degree) + "," + std::to_string(state.gunAngle_degree) + "," + std::to_string(state.time) + ",";
    for (Shooter::Bullet bullet : state.magazine)
    {
        if (bullet.state == Shooter::Bullet::travelling)
            data += std::to_string(bullet.x / float(SCREEN_WIDTH)) + "," + std::to_string(bullet.y / float(SCREEN_HEIGHT)) + "," + std::to_string(bullet.shootingAngle_degree) + "," + std::to_string(bullet.type) + "," + std::to_string(bullet.shootingTime) + ",";
        else
            data += "s" + std::to_string(uint8_t(bullet.state)) + ",";
    }
    return data;
}
State Client::decodeURL(std::stringstream data, unsigned int id, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT)
{
    State state;
    if (data.str() == "0")
    {
        state.gameOn = false;
        return state;
    }
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
    data.ignore();
    data >> state.time;
    data.ignore();
    while (true)
    {
        Shooter::Bullet bullet;
        float val;
        switch (data.peek())
        {
        case EOF:
            return state;
        case 's':
            data.ignore();
            data >> val;
            bullet.state = Shooter::Bullet::flag(val);
            break;
        default:
            bullet.state = Shooter::Bullet::travelling;
            data >> val;
            bullet.x = val * SCREEN_WIDTH;
            data.ignore();

            data >> val;
            bullet.y = val * SCREEN_HEIGHT;
            data.ignore();

            data >> val;
            bullet.shootingAngle_degree = val;
            data.ignore();

            data >> val;
            bullet.type = Shooter::bulletType(val);
            data.ignore();

            data >> val;
            bullet.shootingTime = val;
        }
        data.ignore();
        state.magazine.emplace_back(bullet);
    }
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
        curl_easy_setopt(curl, CURLOPT_URL, (serverURL + "/update?" + Client::encodeURL(client->state1, client->id, SCREEN_WIDTH, SCREEN_HEIGHT)).c_str());
        curl_easy_perform(curl);
        client->state2 = Client::decodeURL(std::stringstream(response), client->id, SCREEN_WIDTH, SCREEN_HEIGHT);

        client->clientThreadInvoked = false;
    }
    curl_easy_cleanup(curl);
}
Client::Client(std::string serverURL, uint32_t SCREEN_WIDTH, uint32_t SCREEN_HEIGHT)
{
    Client::serverURL = serverURL;
    curl_global_init(CURL_GLOBAL_ALL);
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
bool Client::sendAndRecieve(Shooter &player1, Shooter &player2)
{
    bool gameOn;
    while (Client::clientThreadInvoked)
    {
    }
    state1.copy(player1);
    state2.sync(player2);
    gameOn = state2.gameOn;
    Client::clientThreadInvoked = true;
    return gameOn;
}
Client::~Client()
{
    this->clientThreadPermission = false;
    clientThread.join();
    curl_easy_setopt(this->curl, CURLOPT_URL, (Client::serverURL + "/quit?id=" + std::to_string(this->id)).c_str());
    curl_easy_perform(this->curl);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}