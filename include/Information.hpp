#ifndef INFORMATION_DATA_HPP
#define INFORMATION_DATA_HPP

#include <robinmap/robin_hood.h>
#include <vector>
#include <string>
#include <map>


class C_Suspect;

enum Category {
    Safe, Suspicious, Other, Alibi, Location, AskFor, TimeLine
};

struct ScenarioQuestion {
    int hour;
    std::string topic;
    std::string text;
};

struct Statement {
    Category cat;
    std::string location = "None";
    int arriveWithID = -1;
    int seenWithID = -1;
    int vouchID = -1;
    int seenIDLeave = -1;
    int leaveWithID = -1;
    int duration = 0;
};

struct Scenario {
    int mapID;
    std::string mapName;
    std::vector<std::string> worldLocations = {};
    std::vector<ScenarioQuestion> timelineQuestions;
    robin_hood::unordered_map<int, std::vector<Statement>> alibiLibrary;
};


struct SuspectInfo
{
    std::string name;
    int ID;
    C_Suspect* suspectRef = nullptr;

    std::vector<std::pair<std::string, std::string>> generalNotes;
    std::vector<Statement> timeline;

    void AddEntry(Statement cat) {
        timeline.push_back(cat);
    }
};
#endif // !INFORMATION_DATA_HPP
