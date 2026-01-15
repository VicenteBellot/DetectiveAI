#include "Detective.hpp"
#include "Suspect.hpp"

C_Detective::C_Detective(Country c, Cluster cl, bool g, bool s, int iq, float trust)
    : C_BasePawn(c, cl, g, s), IQ(iq), trust(trust) {
    sexism = 10;
    classism = 10;
    homophobia = 10;
    racism = 10;
}

void C_Detective::Remember(C_Suspect& sus, Statement cat)
{
    if (memory.find(sus.ID) == memory.end()) {
        SuspectInfo newInfo;
        newInfo.ID = sus.ID;
        newInfo.suspectRef = &sus; // Store the address of the suspect pawn
        newInfo.name = sus.name;
        memory.emplace(sus.ID, newInfo);
    }

    memory[sus.ID].AddEntry(cat);
}

const SuspectInfo* C_Detective::GetInfo(const int& suspectID) const
{
    auto it = memory.find(suspectID);
    if (it != memory.end()) {
        return &(it->second);
    }
    return nullptr; // empty if not found
}


C_Suspect* C_Detective::GetSuspectByID(int targetID) const
{
    auto it = memory.find(targetID);
    if (it != memory.end()) {
        return it->second.suspectRef;
    }
    return nullptr;
}

C_Detective::InterrogationStyle C_Detective::DecideStyle(const C_Suspect& sus) const
{
    float aggression = GetAggreivityBias(sus); // Uses formula of aggresion

    aggression += (this->frustration * 0.5f);

    if (aggression > 1.5f) return InterrogationStyle::Aggressive;
    if (aggression > 0.8f) return InterrogationStyle::Formal;
    return InterrogationStyle::Casual;
}

void C_Detective::AnalyzeAlibis(int idA, int idB, std::string room)
{
    // Get the statements Thorne remembers
    auto itA = memory.find(idA);
    auto itB = memory.find(idB);
    if (itA == memory.end() || itB == memory.end()) return;
    // (Logic to find the statement for 'room' in memory for both IDs...)

    Statement* sA = nullptr;
    Statement* sB = nullptr;

    for (auto& stmt : itA->second.timeline) {
        if (stmt.location == room) {
            sA = &stmt;
            break;
        }
    }

    for (auto& stmt : itB->second.timeline) {
        if (stmt.location == room) {
            sB = &stmt;
            break;
        }
    }

    if (sA && sB) {
        bool mismatchFound = false;
        if ((sA->arriveWithID == idB && sB->arriveWithID != idA) ||
            (sB->arriveWithID == idA && sA->arriveWithID != idB)) {
            printf("ENTRY CONTRADICTION: One claims they entered together, the other denies it.\n");
            mismatchFound = true;
        }

        // They were together, but their stories about how long they stayed differ significantly
        if ((sA->vouchID == idB && sB->vouchID != idA) ||
            (sB->vouchID == idA && sA->vouchID != idB)) {
            printf("VOUCH CONTRADICTION: One-way alibi detected in %s.\n", room.c_str());
            mismatchFound = true;
        }

        if ((sA->leaveWithID == idB && sB->leaveWithID != idA) ||
            (sB->leaveWithID == idA && sA->leaveWithID != idB)) {
            printf("EXIT CONTRADICTION: Disagreement on whether they left %s together.\n", room.c_str());
            mismatchFound = true;
        }

        // 4. DURATION CHECK: Only relevant if they claim to have been together
        if (sA->vouchID == idB && sB->vouchID == idA) {
            if (std::abs(sA->duration - sB->duration) > 10) { // Tighter margin
                printf("DURATION DISCREPANCY: They claim to have been together, but times don't match.\n");
                mismatchFound = true;
            }
        }
        if (mismatchFound) {
            sA->cat = Suspicious;
            sB->cat = Suspicious;
        }
        else {
            sA->cat = Safe;
            sB->cat = Safe;
        }
    }
}

void C_Detective::Interrogate(C_Suspect* sus, std::string roomName)
{
    Statement report = sus->ProvideStatement(roomName);

    if (memory.find(sus->ID) == memory.end()) {
        SuspectInfo newInfo;
        newInfo.ID = sus->ID;
        newInfo.name = sus->name;
        newInfo.suspectRef = sus;
        memory[sus->ID] = newInfo;
    }

    // We store the statement in the suspect's known timeline in Thorne's head
    memory[sus->ID].timeline.push_back(report);
    //Remember(*sus, report);
    if (report.location == "None") {
        printf("Thorne Notes: %s claims they were never in the %s\n", sus->name.c_str(), roomName.c_str());
    }

}


float C_Detective::GetAggreivityBias(const C_Suspect& sus) const
{
    float countryFriction = CountryAgressivityTable[this->country][sus.country];
    
    float classismFriction = this->classism * BaseBias[sus.cluster][0];
    float sexismFriction = this->sexism * BaseBias[sus.cluster][1];
    float homoFriction = this->homophobia * BaseBias[sus.cluster][2];
    float racismFriction = this->racism * BaseBias[sus.cluster][3];

    float totalFriction = racismFriction + classismFriction;

    if (sus.gender != this->gender) totalFriction += sexismFriction;

    if (this->sexualOrientation != sus.sexualOrientation) totalFriction += homoFriction;

    return countryFriction + totalFriction;
}

float C_Detective::GetDishonestyBias(const C_Suspect& sus) const
{
    float countryFriction = CountryDishonestyTable[this->country][sus.country];

    float classismFriction = this->classism * BaseBias[sus.cluster][0];
    float sexismFriction = this->sexism * BaseBias[sus.cluster][1];
    float homoFriction = this->homophobia * BaseBias[sus.cluster][2];
    float racismFriction = this->racism * BaseBias[sus.cluster][3];

    float totalFriction = racismFriction + classismFriction;

    if (sus.gender != this->gender) totalFriction += sexismFriction;

    if (this->sexualOrientation != sus.sexualOrientation) totalFriction += homoFriction;

    return countryFriction + totalFriction;
}
