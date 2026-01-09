#ifndef INFORMATION_DATA_HPP
#define INFORMATION_DATA_HPP

#include <robinmap/robin_hood.h>
#include <vector>
#include <string>


class C_Suspect;

enum Category {
    Safe, Suspicious, Other, Alibi, Location, Witness
};

struct Scenario {
    int mapID;
    std::string mapName;
    std::vector<std::string> worldLocations = {};
};

struct Statement {
    Category cat;         // Current state (Safe/Suspicious)
    Category type;
    std::string location;
    int startTime;    // e.g., 1400 (2:00 PM)
    int duration;     // e.g., 60 (minutes)
    int seenWithID;   // ID of the other bot mentioned (-1 if alone)
    bool isPlayer;    // Was this statement about the player?
};

struct SuspectInfo
{
    std::string name;
    int ID;
    C_Suspect* suspectRef = nullptr;

    std::vector<std::pair<std::string, std::string>> generalNotes;
    std::vector<Statement> timeline;;

    void AddEntry(const std::string& question, const std::string& answer, Statement cat) {
        generalNotes.emplace_back(question, answer);
        timeline.push_back(cat);
    }
};
#endif // !INFORMATION_DATA_HPP
