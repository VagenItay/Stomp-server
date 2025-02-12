#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>
using namespace std;

using json = nlohmann::json;

Event::Event(std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string discription)
    : team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription)
{
}

Event::~Event()
{
}

Event::Event():team_a_name(""),team_b_name(""),name(""),time(-1)
{
    map<string, string> a;
    map<string, string> b;
    map<string, string> general;
    this->game_updates = general;
    this->team_a_updates = a;
    this->team_b_updates=b;
   
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const std::string &Event::get_discription() const
{
    return this->description;
}

void Event::update_Summary(bool flag, Event event)
{
    if(flag)
    {
        team_a_name=event.get_team_a_name();
        team_b_name=event.get_team_b_name();
        name=event.get_name();
        time=event.get_time();
    }
    // set game event
    for (const auto &kv : event.get_game_updates())
    {
        if (game_updates.count(kv.first) == 0)
        {
            game_updates.insert({kv.first, kv.second});
        }
        else
        {
            auto it = game_updates.find(kv.first);
            if (it != game_updates.end())
                it->second = kv.second;
        }
    }
    // set team a events
    for (const auto &kv : event.get_team_a_updates())
    {
        if (team_a_updates.count(kv.first) == 0)
        {
            team_a_updates.insert({kv.first, kv.second});
        }
        else
        {
            auto it = team_a_updates.find(kv.first);
            if (it != team_a_updates.end())
                it->second = kv.second;
        }
    }
    // team b
    for (const auto &kv : event.get_team_b_updates())
    {
        if (team_b_updates.count(kv.first) == 0)
        {
            team_b_updates.insert({kv.first, kv.second});
        }
        else
        {
            auto it = team_b_updates.find(kv.first);
            if (it != team_b_updates.end())
                it->second = kv.second;
        }
    }
    description += std::to_string(event.get_time()) + "-" + event.get_name() + ":\n" + "\n" + event.get_discription() + "\n\n";
}

Event::Event(const std::string &frame_body) : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{
    unordered_map<string, string> basicDataMap;
    insertDataToUnOrderedMap(basicDataMap, frame_body);
    name = basicDataMap.at("event name");
    team_a_name = basicDataMap.at("team a");
    team_b_name = basicDataMap.at("team b");
    time = stoi(basicDataMap.at("time"));
    int team_a_pos = frame_body.find("team a updates");
    int team_b_pos = frame_body.find("team b updates");
    int description_pos = frame_body.find("description");
    string teamA_updates = frame_body.substr(team_a_pos,team_b_pos-team_a_pos);
    string teamB_updates = frame_body.substr(team_b_pos,description_pos-team_b_pos);
    description=frame_body.substr(description_pos+12,frame_body.size()-2);
    map<string, string> teamA_updatesMap;
    map<string, string> teamB_updatesMap;
    insertDataToMap(teamA_updatesMap, teamA_updates);
    insertDataToMap(teamB_updatesMap, teamB_updates);
    team_a_updates = teamA_updatesMap;
    team_b_updates = teamB_updatesMap;
}

void Event::insertDataToUnOrderedMap(unordered_map<string, string> &map,string input)
{
    vector<string> cutting;
    boost::split(cutting, input, boost::is_any_of("\n"));
    for (string line : cutting)
    {
        vector<string> dataSeperate;
        boost::split(dataSeperate, line, boost::is_any_of(":"));
        if(dataSeperate.size()>1)
            map.insert({dataSeperate[0], dataSeperate[1]});
    }
}

void Event::insertDataToMap(map<string, string> &map, string input)
{
    vector<string> cutting;
    boost::split(cutting, input, boost::is_any_of("\n"));
    for (string line : cutting)
    {
        vector<string> dataSeperate;
        boost::split(dataSeperate, line, boost::is_any_of(":"));
        if(dataSeperate.size()>1&&dataSeperate[1]!="")
            map.insert({dataSeperate[0], dataSeperate[1]});
    }
}

names_and_events parseEventsFile(std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }

        events.push_back(Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}