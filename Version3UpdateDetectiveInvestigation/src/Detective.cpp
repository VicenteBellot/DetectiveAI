#include "Detective.hpp"
#include "Suspect.hpp"
#include <filesystem>
#include <fstream>

C_Detective::C_Detective(Country c, Cluster cl, bool g, bool s, float iq, float trust)
    : C_BasePawn(c, cl, g, s), IQ(iq), trust(trust) {
   SetBias("./data/Bias.csv");
   if (MinSleepHours < 0) MinSleepHours = 0;

}

C_Detective::~C_Detective()
{

}

void ReadBook(std::string filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        return;
    }
    
    __int64 lineCount = std::count(std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(), '\n');

    int LineNumber = rand() % lineCount;
    file.clear();                 
    file.seekg(0, std::ios::beg);

    std::string line;
    int currentLine = 0;

    while (currentLine < LineNumber && std::getline(file, line)) {
        currentLine++;
    }

    if (std::getline(file, line)) {
        printf("||%s\n\n",line.c_str());
    }
}

void C_Detective::Talk(int ID_Talk, int extra_info)
{
    if (IQ > MinDumbIQ){
        switch (ID_Talk) {
        case 0:
            printf("||======================||\n");
            printf("||START OF INTERROGATION||\n");
            printf("||======================||\n");
            break;
        case 1:
            printf("||[FOLLOW UP] Corroborating with Suspect %d\n", extra_info);
            break;
        case 2:
            printf("||[Answer Stored: Suspect %d ", extra_info);
            break;
        case 3:
            printf("answered % d]\n", extra_info);
            break;
        case 4:
            printf("||[Category: TO ASK] - Points to another suspect.\n");
            break;
        case 5:
            printf("||[Category: Safe]\n");
            break;
        case 6:
            printf("||====================================================||\n");
            printf("|| THIS ANSWER HAS NOT BEEN LISTENED TO DUE TO RACISM ||\n");
            printf("||====================================================||\n");
            break;
        case 7:
            printf("||Suspect: %d has been asked all questions posible\n", extra_info);
            break;
        case 8:
            printf("||====================||\n");
            printf("||END OF INTERROGATION||\n");
            printf("||====================||\n\n");
            printf("-------------------------------------------------------------------------------------\n");
            break;
        case 9:
            printf("\n||Ah, end of the day at last, I can go \n||home now and read one of my favorite book \n||Romeo and Juliet\n");
            printf("\n||Let me read aloud a line of the book:\n");
            ReadBook("./data/Books/Romeo.txt");

            printf("||================================||\n");
            printf("||Tomorrow will be a better day...||\n");
            printf("||================================||\n||\n");
            break;
        case 10:
            printf("||The Next Morning\n");
            switch (dementia)
            {
            case 0:
                printf("||The Detective waked up, took a nice shower and head over to work after having a healty breakfast.\n");
                break;
            case 1: 
                printf("||The Detective waked up, stumbled with the desk, took a nice shower and head over to work after having a healty breakfast.\n");
                break;
            case 2: 
                printf("||The Detective waked up,stumbled with the desk, took a nice shower and head over to work after having some cereal for breakfast.\n");
                break;
            default:
                printf("||The Detective waked up,stumbled with the desk, took a shower that was burning hot and head over to work after having some cereal for breakfast.\n");
                break;
            }

           
            printf("||=======================================================||\n");
            printf("||The rest has lowered the level of alcohol sustancially.||\n");
            printf("||=======================================================||\n\n");
            printf("-------------------------------------------------------------------------------------\n");
            
            break;
        case 11:
            printf("||==================================================================||\n");
            printf("||   The Detective died due to inhumanly possible rate of alcohol   ||\n");
            printf("|| He drove his car into the nearest tree at a velocity of %d km/h ||\n", (rand() % 250));
            printf("||==================================================================||\n");
            break;
        case 12:
            printf("||============||\n");
            printf("||END OF ROUND||\n");
            printf("||============||\n");
            printf("-------------------------------------------------------------------------------------\n");
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
            ReadBook("./data/Books/Humpty.txt");

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

void C_Detective::UpdateBias()
{
    if (GramoPorLitro > 0.5f && GramoPorLitro <= 2.5f) {
        racism *= 0.92f;
        classism *= 0.85f;
        sexism *= 0.95f;
        homophobia *= 0.97f;

        IQ *= 1.07f;
    }
    else if (GramoPorLitro > 2.5f) {
        racism *= 1.15f;
        classism *= 1.18f;
        sexism *= 1.2f;
        homophobia *= 1.32f;

        IQ *= 0.97f;
    }
}

void C_Detective::UpdateDementia()
{
    DementiaChance += 5;
    if (rand() % 100 < DementiaChance) {
        dementia++;
    }
    int HoursofSleep = rand() % (MaxSleepHours - MinSleepHours) + (MaxSleepHours);
    GramoPorLitro -= rand() % 100 < GoodSleepChance ? 0.15f * HoursofSleep : 0.12f * HoursofSleep;
    if (GramoPorLitro < 0.0f) GramoPorLitro = 0.0f;
    BeersDrank = 0;

    // se olvida de una pregunta 
    if (dementia > 2 && !memory.empty()) {
        auto it = memory.begin();
        std::advance(it, rand() % memory.size());
        if (!it->second.timeline.empty()) {
            auto entry = it->second.timeline.begin();
            std::advance(entry, rand() % it->second.timeline.size());

            if (it->second.cat == Category::Suspicious) {
                int suspectID = it->second.ID;
                if (suspicionCount.find(suspectID) != suspicionCount.end()) {
                    suspicionCount[suspectID] = std::max(0, suspicionCount[suspectID] - 1);
                    printf("||[DEMENTIA] Forgot contradiction about suspect %d. Suspicion dropped to %d\n",
                        suspectID, suspicionCount[suspectID]);
                }
            }

            it->second.timeline.erase(entry);
        }
    }
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

            printf("||[ALIBI CONFIRMED] Suspect %d and %d cover each other.\n", idA, idB);
            memPrev.cat = Category::Safe;
            memCurr.cat = Category::Safe;
        }
        else {
            printf("||[ALIBI FAILED] Contradiction found between %d and %d.\n", idA, idB);

            memPrev.cat = Category::Suspicious;
            memCurr.cat = Category::Suspicious;

            suspicionCount[idA]++;
            printf("||[SUSPICION] Suspect %d has %d contradictions\n", idA, suspicionCount[idA]);

            if (suspicionCount[idA] >= AccuseThreshold) {
                printf("||CULPABLE: %d\n", idA);
                FoundCulprit = true;
                Culprit = idA;
            }


        }
    }
}

void C_Detective::AnalyzeSawTwice(int suspectID)
{
    if (memory.find(suspectID) == memory.end()) return;

    auto it = memory[suspectID].timeline.find(SuspectInfo::KQuestionAsked::SawTwice);
    if (it == memory[suspectID].timeline.end()) return;

    int accusedID = it->second + 101;
    if (accusedID < 101) return; // -1 means nobody, skip

    if (memory.find(accusedID) == memory.end()) return;

    printf("||[SAW TWICE] Suspect %d was spotted in multiple locations by %d\n", accusedID, suspectID);

    suspicionCount[accusedID]++;
    printf("||[SUSPICION] Suspect %d has %d contradictions\n", accusedID, suspicionCount[accusedID]);

    if (suspicionCount[accusedID] >= AccuseThreshold) {
        printf("||CULPABLE: %d\n", accusedID);
        FoundCulprit = true;
        Culprit = accusedID;
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

    float limit = hatefullness.size() * 0.4f;
    for (float i = 0; i < limit && !CanIgnore; i+=1.0f) {
        if (sus->ID - 101 == hatefullness[(int)i].id) {
            CanIgnore = true;
        }
    }

    InterrogationStyle style = DecideStyle(*sus);
    if (style == Aggressive) frustration += 0.1f; // aggressive backfires over time
    if (style == Casual)     IgnoreChance -= 10;

    if (CanIgnore) {
        WillIgnore = rand() % 100 > IgnoreChance ? false : true;
    }

    // We store the statement in the suspect's known timeline in Thorne's head
    int* ptr = (int*)(&report);
    bool breakloop = false;
    int answerID = -2;

    if (!WillIgnore || cat == Category::ToAsk) {
        for (int i = 0; i < 8 && !breakloop; i++) {
            if ((ptr[i]) > -2) {
                answerID = ptr[i] - 101;
                breakloop = true;

                // Store the answer
                memory[sus->ID].AddEntry((SuspectInfo::KQuestionAsked(Question)), answerID);
                Talk(2, sus->ID);
                Talk(3, answerID);

                // Determine Status
                if (answerID > -1 && 
                    Question != SuspectInfo::KQuestionAsked::TimeStayed && 
                    Question != SuspectInfo::KQuestionAsked::WatchLeave && 
                    Question != SuspectInfo::KQuestionAsked::SawTwice) {
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
        if (Question == SuspectInfo::KQuestionAsked::SawTwice) {
            AnalyzeSawTwice(sus->ID);
        }
      
    }
    else {
        Talk(6);

        if (rand() % 100 < DrinkingChance) {
            DementiaChance -= 1;
            BeersDrank++;
            GramoPorLitro += 0.15f;
            UpdateBias();
        }

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
