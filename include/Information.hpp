#ifndef INFORMATION_DATA_HPP
#define INFORMATION_DATA_HPP

#include <robinmap/robin_hood.h>
#include <vector>
#include <string>
#include <map>


class C_Suspect;

enum Category {
    Safe, Suspicious, ToAsk
};

struct ScenarioQuestion {
    int hour;
    std::string topic;
    std::string text;
};

struct Statement {
    int arriveWithID = -1;
    int seenWithID = -1;
    int vouchID = -1;
    int seenIDLeave = -1;
    int leaveWithID = -1;
    int duration = -1;
    Category cat;
    std::string location = "None";
};

struct Scenario {
    int mapID;
    std::string mapName;
    std::vector<std::string> worldLocations = {};
    std::vector<ScenarioQuestion> timelineQuestions;
    std::map<int, std::vector<Statement>> alibiLibrary;
};


struct SuspectInfo
{
    enum KQuestionAsked {
        ArriveWith,
        StayedWith,
        VouchPresence,
        WatchLeave,
        LeaveWith,
        TimeStayed
    };

    std::string name;
    int ID;
    C_Suspect* suspectRef = nullptr;

    std::vector<std::pair<std::string, std::string>> generalNotes; //Not in use yet

    robin_hood::unordered_map<KQuestionAsked, int> timeline;
    //std::vector<Statement> timeline;

    Category cat;

    void AddEntry(SuspectInfo::KQuestionAsked quest, int cat) {
        timeline.emplace(quest, cat);
    }
};
#endif // !INFORMATION_DATA_HPP
