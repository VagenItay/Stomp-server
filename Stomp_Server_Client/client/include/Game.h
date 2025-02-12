
#pragma once

#include "../include/ConnectionHandler.h"
#include "event.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <unordered_map>
using namespace std;
using boost::asio::ip::tcp;
class Game
{
private:
    unordered_map<string,unordered_map<string,Event>> topicToMapBetweenUserAndSummary;

public:
    Game();
    void UpdateSummary(Event event,string user_name,string game_name);
    void newSummary(Event event,string user_name,string game_name);
    Event buildEventFromSend(string send);
    void eventCheck(Event need_Update_Event,string user_name,string game_name);
    unordered_map<string,unordered_map<string,Event>>& getMap();
};