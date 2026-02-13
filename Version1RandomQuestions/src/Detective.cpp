#include "Detective.hpp"
#include "Suspect.hpp"
#include <filesystem>
#include <fstream>

C_Detective::C_Detective(Country c, Cluster cl, bool g, bool s, int iq, float trust)
    : C_BasePawn(c, cl, g, s), IQ(iq), trust(trust) {
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

    //memory[sus.ID].AddEntry(cat);
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

void C_Detective::AnalyzeAlibis(int idA, int idB)
{
    if (idA< 0 || idB< 0) return;

    auto& memPrev = memory[idA];
    auto& memCurr = memory[idB];

    auto itPrev = memPrev.timeline.find(SuspectInfo::KQuestionAsked(LastQuestionAsked));
    auto itCurr = memCurr.timeline.find(SuspectInfo::KQuestionAsked(LastQuestionAsked));

    if (itPrev != memPrev.timeline.end() && itCurr != memCurr.timeline.end()) {

        int answerPrevious = itPrev->second; // The ID that the previous guy gave (Should be idCurrent)
        int answerCurrent = itCurr->second;  // The ID the current guy gave (Should be idPrevious)

        if (answerCurrent == idA - 101 && answerPrevious == idB - 101) {

            printf("[ALIBI CONFIRMED] Suspect %d and %d cover each other.\n", idA, idB);

            // Mark both as SAFE so we stop asking about this loop
            memPrev.cat = Category::Safe;
            memCurr.cat = Category::Safe;
        }
        else {
            printf("[ALIBI FAILED] Contradiction found between %d and %d.\n", idA, idB);

            memPrev.cat = Category::Suspicious;
            memCurr.cat = Category::Suspicious;

            //Aqui se guarda el ID del que miente, para marcarlo como culpable

            printf("CULPABLE: %d\n", idA);
            FoundCulprit = true;
            Culprit = idA;

        }
        

    }
}

void C_Detective::Interrogate(C_Suspect* sus, int Question, Category cat)
{
    Statement report = sus->ProvideStatement(Question);


    if (memory.find(sus->ID) == memory.end()) {
        SuspectInfo newInfo;
        newInfo.ID = sus->ID;
        newInfo.name = sus->name;
        newInfo.suspectRef = sus;
        memory[sus->ID] = newInfo;
        
    }

    // We store the statement in the suspect's known timeline in Thorne's head
    int* ptr = (int*)(&report);
    bool breakloop = false;
    int answerID = -2;
    for (int i = 0; i < 6 && !breakloop; i++) {
        if ((ptr[i]) > -2) {
            answerID = ptr[i] - 101; // Adjusting based on your logic
            breakloop = true;

            // Store the answer
            memory[sus->ID].AddEntry((SuspectInfo::KQuestionAsked(Question)), answerID);
            printf("[Answer Stored: Suspect %d answered %d]\n", sus->ID, answerID);

            // Determine Status
            if (answerID > -1) {
                // He gave a valid ID (a person). 
                printf("[Category: TO ASK] - Points to another suspect.\n");
                memory[sus->ID].cat = Category::ToAsk;
            }
            else {
                printf("[Category: Safe]\n");
                memory[sus->ID].cat = Category::Safe;
            }
        }
    }
    
    if (cat == Category::ToAsk && LastSuspectedInterrogates != -2 && LastSuspectedInterrogates != sus->ID) {
        AnalyzeAlibis(LastSuspectedInterrogates, sus->ID);
    }
    printf("---------------------\n");
    printf("END OF INTERROGATION\n\n");
    //memory[sus->ID].AddEntry(SuspectInfo::KQuestionAsked(Question),);
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
