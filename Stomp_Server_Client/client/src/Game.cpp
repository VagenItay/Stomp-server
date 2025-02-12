#include "Game.h"
#include <boost/algorithm/string.hpp>

Game::Game() {
    unordered_map<string,unordered_map<string,Event>> topicToUserSummary;
    this->topicToMapBetweenUserAndSummary = topicToUserSummary;
}
// login 127.0.0.1:7777 g g
//join Germany_Japan
//report /home/spl211/Desktop/new/Assignment3Update11-1/client/data/events1.json
void Game::UpdateSummary(Event event, string user_name, string game_name)
{

    bool flag = false;
    Event need_Update_Event = Event();
    eventCheck(need_Update_Event,user_name, game_name);
    if (need_Update_Event.get_name() == "")
    {
        flag = true;
    }
    topicToMapBetweenUserAndSummary.at(game_name).at(user_name).update_Summary(flag, event);
}

void Game::eventCheck(Event need_Update_Event,string user_name, string game_name)
{
    auto itr1=topicToMapBetweenUserAndSummary.find(game_name);
    if(itr1 == topicToMapBetweenUserAndSummary.end())
    {   
       topicToMapBetweenUserAndSummary.insert({game_name,{}});
    }
    auto itr2 = topicToMapBetweenUserAndSummary.at(game_name).find(user_name);
    if (itr2 == topicToMapBetweenUserAndSummary.at(game_name).end())
        {
            topicToMapBetweenUserAndSummary.at(game_name).insert({user_name,Event()});
        }
    else
        {
            need_Update_Event=topicToMapBetweenUserAndSummary.at(game_name).at(user_name);
        }
    
}

Event Game::buildEventFromSend(string send)
{
    string user_name;
    string game_name;
    string send_frame="";
    int start=send.find("user");
    string frame_send=send.substr(start);
    Event send_ = Event(frame_send);
    vector<string> mini_Vector;
    boost::split(mini_Vector, send, boost::is_any_of("\n"));
    for (string b : mini_Vector)
    {
        vector<string> Sub_Data_Vector;
        boost::split(Sub_Data_Vector, b, boost::is_any_of(":"));
        if (Sub_Data_Vector[0] == "destination")
        {
            int pos = Sub_Data_Vector[1].find("/");
            string sub = Sub_Data_Vector[1].substr(pos + 1);
            game_name = sub;
        }
        else if (Sub_Data_Vector[0] == "user")
        {
            user_name = Sub_Data_Vector[1];
        }
    }
    UpdateSummary(send_, user_name, game_name);
    return send_;
}

unordered_map<string,unordered_map<string,Event>>& Game::getMap(){
    return topicToMapBetweenUserAndSummary;
}