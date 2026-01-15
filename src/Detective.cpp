#include "Detective.hpp"
#include "Suspect.hpp"
#include <filesystem>
#include <fstream>

C_Detective::C_Detective(Country c, Cluster cl, bool g, bool s, int iq, float trust)
    : C_BasePawn(c, cl, g, s), IQ(iq), trust(trust) {
    //switch (c) {
    //case UK:
    //    if (cl == V1) {
    //        classism = 2;
    //        sexism = 2;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    else {
    //        classism = 1;
    //        sexism = 1;
    //        homophobia = 1;
    //        racism = 0;
    //    }
    //    break;
    //case FR:
    //    if (cl == V2) {
    //        classism = 1;
    //        sexism = 1;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    else {
    //        classism = 2;
    //        sexism = 1;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    break;
    //case DE:
    //    if (cl == V1) {
    //        classism = 1;
    //        sexism = 1;
    //        homophobia = 0;
    //        racism = 0;
    //    }
    //    else {
    //        classism = 1;
    //        sexism = 1;
    //        homophobia = 0;
    //        racism = 0;
    //    }
    //    break;
    //case JP:
    //    if (cl == V4) {
    //        classism = 2;
    //        sexism = 3;
    //        homophobia = 2;
    //        racism = 2;
    //    }
    //    else {
    //        classism = 2;
    //        sexism = 2;
    //        homophobia = 2;
    //        racism = 1;
    //    }
    //    break;
    //case IT:
    //    if (cl == V2) {
    //        classism = 1;
    //        sexism = 3;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    else {
    //        classism = 1;
    //        sexism = 2;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    break;
    //case ES:
    //    if (cl == V7) {
    //        classism = 1;
    //        sexism = 2;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    else {
    //        classism = 0;
    //        sexism = 1;
    //        homophobia = 0;
    //        racism = 0;
    //    }
    //    break;
    //case RU:
    //    if (cl == V1) {
    //        classism = 2;
    //        sexism = 3;
    //        homophobia = 3;
    //        racism = 2;
    //    }
    //    else {
    //        classism = 2;
    //        sexism = 2;
    //        homophobia = 3;
    //        racism = 2;
    //    }
    //    break;
    //case AU:
    //    if (cl == V8) {
    //        classism = 0;
    //        sexism = 1;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    else {
    //        classism = 1;
    //        sexism = 2;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    break;
    //case SE:
    //    if (cl == V2) {
    //        classism = 0;
    //        sexism = 0;
    //        homophobia = 0;
    //        racism = 0;
    //    }
    //    else {
    //        classism = 0;
    //        sexism = 0;
    //        homophobia = 0;
    //        racism = 0;
    //    }
    //    break;
    //case EU:
    //    if (cl == V8) {
    //        classism = 1;
    //        sexism = 1;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    else {
    //        classism = 1;
    //        sexism = 2;
    //        homophobia = 1;
    //        racism = 1;
    //    }
    //    break;
    ////}
    SetBias("./data/Bias.csv");
}

void C_Detective::SetBias(std::string filePath)
{
    std::ifstream file(filePath);
    std::string line;

    int num = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cluster,classism, sex, homo, race ;

        std::getline(ss, cluster, ',');
        std::getline(ss, classism, ',');
        std::getline(ss, sex, ',');
        std::getline(ss, homo, ',');
        std::getline(ss, race);
        num++;
        //printf("%d\n", (int)(num * 0.49f));
        if (this->country == (int)(num * 0.49f)) {
            if (this->cluster == std::stoi(cluster)) {
                this->racism = std::stof(race);
                this->sexism = std::stof(sex);
                this->homophobia = std::stof(homo);
                this->classism = std::stof(classism);
            }
            
        }


    }
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

void C_Detective::Interrogate(C_Suspect* sus, Statement s)
{
    Statement report = sus->ProvideStatement(s);

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
        printf("Thorne Notes: %s claims they were never in the %s\n", sus->name.c_str(), s.location.c_str());
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
