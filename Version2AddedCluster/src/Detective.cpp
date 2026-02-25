#include "Detective.hpp"
#include "Suspect.hpp"
#include <filesystem>
#include <fstream>

C_Detective::C_Detective(Country c, Cluster cl, bool g, bool s, int iq, float trust)
    : C_BasePawn(c, cl, g, s), IQ(iq), trust(trust) {
   SetBias("./data/Bias.csv");
}

C_Detective::~C_Detective()
{

}

void C_Detective::Talk(int ID_Talk, int extra_info)
{
    if (IQ > 100){
        switch (ID_Talk) {
        case 0:
            printf("START OF INTERROGATION\n");
            printf("---------------------\n");
            break;
        case 1:
            printf("[FOLLOW UP] Corroborating with Suspect %d\n", extra_info);
            break;
        case 2:
            printf("[Answer Stored: Suspect %d ", extra_info);
            break;
        case 3:
            printf("answered % d]\n", extra_info);
            break;
        case 4:
            printf("[Category: TO ASK] - Points to another suspect.\n");
            break;
        case 5:
            printf("[Category: Safe]\n");
            break;
        case 6:
            printf("-- THIS ANSWER HAS NOT BEEN LISTENED TO DUE TO RACISM --\n");
            break;
        case 7:
            printf("Suspect: %d has been asked all questions posible\n", extra_info);
            break;
        case 8:
            printf("---------------------\n");
            printf("END OF INTERROGATION\n\n");
            break;
        case 9:
            printf("Ah, end of the day at last, I can go home now and read one of my favorite book \n Romeo and Juliet\n");
            printf("Let me read aloud a line of the book:\n");
            //read random

            printf("||Tomorrow will be a better day...||\n");
            break;
        case 10:
            switch (dementia)
            {
            printf("The Next Morning\n");
            case 0:
                printf("The Detective waked up, took a nice shower and head over to work after having a healty breakfast.\n");
                break;
            case 1: 
                printf("The Detective waked up, stumbled with the desk, took a nice shower and head over to work after having a healty breakfast.\n");
                break;
            case 2: 
                printf("The Detective waked up,stumbled with the desk, took a nice shower and head over to work after having some cereal for breakfast.\n");
                break;
            default:
                printf("The Detective waked up,stumbled with the desk, took a shower that was burning hot and head over to work after having some cereal for breakfast.\n");
                break;
            }
            dementia++;
            break;
        case 11:
            break;
        case 12:
            break;

        }
    }
    else {
        int numofword;
        if (ID_Talk == 9) {
            numofword = (rand() % 25) + 10;
            for (int i = 1; i < numofword; i++) {
                int randomword = rand() % RandomLowIQWords.size();
                printf("%s ", RandomLowIQWords[randomword].c_str());
            }
            printf("Humpty Dumpty\n");

            //Imprimir Quote

            for (int i = 1; i < numofword; i++) {
                int randomword = rand() % RandomLowIQWords.size();
                printf("%s ", RandomLowIQWords[randomword].c_str());
            }
            printf("\n");

        }
        else {
            numofword = (rand() % 5) + 1;
            for (int i = 1; i < numofword; i++) {
                int randomword = rand() % RandomLowIQWords.size();
                printf("%s ", RandomLowIQWords[randomword].c_str());
            }
        }
    }
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
    this->cluster = (C_BasePawn::Cluster)((int) this->cluster + 1);
}


void C_Detective::GenerateSuspectOrder(std::vector<std::shared_ptr<C_Suspect>> suspects)
{
    if (suspects.size() == 0) return;

    int currentId = 0;
    for (const auto& suspect : suspects) {
        hatefullness.push_back({ GetDishonestyBias(*suspect), currentId++ });
    }

    for (int p = 0; p < hatefullness.size(); p++) {
        for (int i = 0; i < hatefullness.size() - 1; i++) {
            if (hatefullness[i].hate < hatefullness[i + 1].hate) {
                THate temp = hatefullness[i];
                hatefullness[i] = hatefullness[i + 1];
                hatefullness[i + 1] = temp;
            }
        }
    }
}

void C_Detective::Remember(std::shared_ptr<C_Suspect> sus, Statement cat)
{
    if (memory.find(sus->ID) == memory.end()) {
        SuspectInfo newInfo;
        newInfo.ID = sus->ID;
        newInfo.suspectRef = sus; // Store the address of the suspect pawn
        newInfo.name = sus->name;
        memory.emplace(sus->ID, newInfo);
    }
}

const SuspectInfo* C_Detective::GetInfo(const int& suspectID) const
{
    auto it = memory.find(suspectID);
    if (it != memory.end()) {
        return &(it->second);
    }
    return nullptr; // empty if not found
}


std::shared_ptr<C_Suspect> C_Detective::GetSuspectByID(int targetID) const
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
            memPrev.cat = Category::Safe;
            memCurr.cat = Category::Safe;
        }
        else {
            printf("[ALIBI FAILED] Contradiction found between %d and %d.\n", idA, idB);

            memPrev.cat = Category::Suspicious;
            memCurr.cat = Category::Suspicious;

            printf("CULPABLE: %d\n", idA);
            FoundCulprit = true;
            Culprit = idA;

        }
    }
}

bool C_Detective::QuestionAskedSuspect(std::shared_ptr<C_Suspect> sus, int Question) {
    if (memory.find(sus->ID) == memory.end()) return false;

    if (memory[sus->ID].timeline.find(SuspectInfo::KQuestionAsked(Question)) == memory[sus->ID].timeline.end()) return false;

    return true;
}

void C_Detective::Interrogate(std::shared_ptr<C_Suspect> sus, int Question, Category cat)
{
    Statement report = sus->ProvideStatement(Question);

    if (memory.find(sus->ID) == memory.end()) {
        SuspectInfo newInfo;
        newInfo.ID = sus->ID;
        newInfo.name = sus->name;
        newInfo.suspectRef = sus;
        memory[sus->ID] = newInfo;
    }

    //sus->ID - 101; va de 0 al numero de sospechoso
    bool CanIgnore = false;

    int limit = hatefullness.size() * 0.4f;
    for (int i = 0; i < limit && !CanIgnore; i++) {
        if (sus->ID - 101 == hatefullness[i].id) {
            CanIgnore = true;
        }
    }

    if (CanIgnore) {
        WillIgnore = rand() % 100 > IgnoreChange ? false : true;
    }

    // We store the statement in the suspect's known timeline in Thorne's head
    int* ptr = (int*)(&report);
    bool breakloop = false;
    int answerID = -2;

    if (!WillIgnore || cat == Category::ToAsk) {
        for (int i = 0; i < 6 && !breakloop; i++) {
            if ((ptr[i]) > -2) {
                answerID = ptr[i] - 101;
                breakloop = true;

                // Store the answer
                memory[sus->ID].AddEntry((SuspectInfo::KQuestionAsked(Question)), answerID);
                Talk(2, sus->ID);
                Talk(3, answerID);

                // Determine Status
                if (answerID > -1 && Question != SuspectInfo::KQuestionAsked::TimeStayed && Question != SuspectInfo::KQuestionAsked::WatchLeave) {
                    Talk(4);
                    memory[sus->ID].cat = Category::ToAsk;
                }
                else {
                    Talk(5);
                    memory[sus->ID].cat = Category::Safe;
                }
            }
        }

        if (cat == Category::ToAsk && LastSuspectedInterrogates != -2 && LastSuspectedInterrogates != sus->ID) {
            AnalyzeAlibis(LastSuspectedInterrogates, sus->ID);
        }
    }
    else {
        Talk(6);

        if (rand() % 100 < DrinkingChance) GramoPorLitro += 0.15f;

    }

    WillIgnore = false;

   
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
