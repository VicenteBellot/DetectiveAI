#include "GameManager.hpp"

#include "BasePawn.hpp"
#include "Suspect.hpp"

#include <fstream>
#include <sstream>

#include <filesystem>
#include <iostream>

C_GameManager::~C_GameManager()
{
}

void C_GameManager::MainLoop(float deltaTime)
{
    DrawInterface(deltaTime);
    if (rounds >= maxroundsPerDay) {
        EndOfDayCycle();
        rounds = 0;
        printf("night night\n");
    }
    if (Thorne->GramoPorLitro > MaxComaEtilico) {
        Thorne->dead = true;
        Thorne->Talk(11);
    }
}

void C_GameManager::EndOfDayCycle()
{
    Thorne->Talk(9);
    Thorne->Talk(10);
}

void C_GameManager::LoadScenarioFromCSV(std::string filePath)
{
    std::ifstream file(filePath);
    std::string line;

    // Skip Header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string topic, text;

        std::getline(ss, text, ',');
        std::getline(ss, topic, ',');

        if (!text.empty()) {
            ScenarioQuestion sq;
            sq.topic = topic;
            sq.text = text;
            currentScenario.timelineQuestions.push_back(sq);
        }
    }
}

void C_GameManager::LoadAlibisFromCSV(std::string filePath) {
    std::ifstream file(filePath);
    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string pID, hr, loc, arrivedpartner, stayedwith, vouch, sawleaving, leftwith;

        std::getline(ss, pID, ',');
        std::getline(ss, loc, ',');
        std::getline(ss, arrivedpartner, ',');
        std::getline(ss, stayedwith, ',');
        std::getline(ss, vouch, ',');
        std::getline(ss, sawleaving, ',');
        std::getline(ss, leftwith, ',');
        std::getline(ss, hr);

        Statement s;
        s.location = loc;
        s.arriveWithID = std::stoi(arrivedpartner);
        s.seenWithID = std::stoi(stayedwith);
        s.vouchID = std::stoi(vouch);
        s.seenIDLeave = std::stoi(sawleaving);
        s.leaveWithID = std::stoi(leftwith);
        s.duration = std::stoi(hr);
        s.cat = Safe; // It's the "Truth"


        // Store in a map: currentScenario.alibiLibrary[PathID].push_back(s)
        //currentScenario.alibiLibrary.insert(std::stoi(pID), s);
        currentScenario.alibiLibrary[std::stoi(pID) - 101].push_back(s);
    }
}

void C_GameManager::LoadRoomsFromCSV(std::string filePath)
{
    std::ifstream file(filePath);
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string hourStr;

        std::getline(ss, hourStr, ',');

        if (!hourStr.empty()) {
            currentScenario.worldLocations.push_back(hourStr.c_str());
        }
    }

}

void C_GameManager::DrawInterface(float DeltaTime)
{
    tick += DeltaTime * Speed;

    ImGui::Begin("Locations");
    for (auto i : currentScenario.worldLocations) {
        ImGui::Text(i.c_str());

    }
    ImGui::End();

    ImGui::Begin("TickSpeed");
    ImGui::SliderFloat("Tick Speed", &Speed, 0.1f, 20.0f);
    ImGui::End();

    ImGui::Begin("Alibis");
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

    if (ImGui::BeginTable("AlibiTable", 8, flags))
    {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Location");
        ImGui::TableSetupColumn("Arrive With");
        ImGui::TableSetupColumn("Seen With");
        ImGui::TableSetupColumn("Vouch ID");
        ImGui::TableSetupColumn("Saw Leave");
        ImGui::TableSetupColumn("Leave With");
        ImGui::TableSetupColumn("Duration");
        ImGui::TableHeadersRow();

        for (auto const& [suspectID, alibiList] : currentScenario.alibiLibrary)
        {
            for (auto const& alibi : alibiList)
            {
                ImGui::TableNextRow();
                if (suspects.at(suspectID)->guilty)
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(255, 0, 0, 100)); // Transparent Red
                }
                ImGui::TableNextColumn();
                ImGui::Text("%d", suspectID+101);

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(alibi.location.c_str());

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.arriveWithID);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.seenWithID);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.vouchID);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.seenIDLeave);

                ImGui::TableNextColumn();
                ImGui::Text("%d", alibi.leaveWithID);

                ImGui::TableNextColumn();
                ImGui::Text("%d min", alibi.duration);
            }
        }

        // 4. Critical: End the table
        ImGui::EndTable();
    }

    ImGui::End();

    // 3. Thorne's Internal State (Mental Dashboard)
    ImGui::Begin("Inspector Thorne's Notes");
    ImGui::Text("Origin Country: %s", Thorne->CountryName[Thorne->country].c_str());
    ImGui::Text("Detectives Cluster: V%d", Thorne->cluster);
    ImGui::Separator();
    ImGui::Text("Frustration: %.2f", Thorne->frustration);
    ImGui::Text("Racism: %.2f", Thorne->racism);
    ImGui::Text("Homophobia: %.2f", Thorne->homophobia);
    ImGui::Text("Classism: %.2f", Thorne->classism);
    ImGui::Text("Sexism: %.2f", Thorne->sexism);
    ImGui::Separator();
    ImGui::Text("Current Style: %s", (Thorne->frustration > 0.6f ? "Aggressive" : "Professional"));

    ImGui::Separator();
    ImGui::ProgressBar(Thorne->GramoPorLitro / MaxComaEtilico, ImVec2(0, 0), "Alcohol Rate");
    ImGui::Separator();

    ImGui::End();

    if (activeSuspectIndex != -1) {
        std::shared_ptr target = suspects[Thorne->hatefullness[activeSuspectIndex].id];

        ImGui::Begin("Interrogation Room");
        ImGui::Text("Subject: %s", target->name.c_str());
        ImGui::Text("Race: V%d", target->cluster);
        ImGui::Text("Country of Origin: %s", target->CountryName[target->country].c_str());
        if (target->gender) ImGui::Text("Gender: Male");
        else ImGui::Text("Gender: Female");

        if(target->sexualOrientation) ImGui::Text("Orientation: Heterosexual");
        else ImGui::Text("Orientation: Other");


        ImGui::ProgressBar(target->stress, ImVec2(0, 0), "Suspect Stress");

        ImGui::Separator();

        // Show questions from the Spreadsheet
        ImGui::Text("Investigation Topics:");
        //Ask Questions
        if (tick > Cooldown && rounds < maxroundsPerDay) {
            tick = 0;
            Thorne->Talk(0);
            bool AskFor = false;
            std::shared_ptr<C_Suspect> nextTarget = nullptr;

            if (Thorne->LastSuspectedInterrogates != -2) {
                auto& memoryID = Thorne->memory[Thorne->LastSuspectedInterrogates];
                auto it = memoryID.timeline.find(SuspectInfo::KQuestionAsked(Thorne->LastQuestionAsked));

                if (it != memoryID.timeline.end()) {
                    int answertolastquestion = it->second;

                    // Check if we need to follow up (And it's NOT the one-way question)
                    if (memoryID.cat == Category::ToAsk && Thorne->LastQuestionAsked != SuspectInfo::KQuestionAsked::WatchLeave) {
                        auto& memoryIDsecond = Thorne->memory[answertolastquestion];
                        auto itsecond = memoryIDsecond.timeline.find(SuspectInfo::KQuestionAsked(Thorne->LastQuestionAsked));

                        if (itsecond == memoryIDsecond.timeline.end()) {
                            Thorne->Talk(1, answertolastquestion);
                            AskFor = true;
                            nextTarget = suspects[answertolastquestion];
                        }
                    }
                }
            }

            int maxQuestions = SuspectInfo::KQuestionAsked::TimeStayed + 1;
            int i = 0;
            if (!AskFor) {
                nextTarget = suspects[Thorne->hatefullness[activeSuspectIndex].id];

                do {
                    i++;
                    Thorne->LastQuestionAsked = rand() % currentScenario.timelineQuestions.size();
                } while (Thorne->QuestionAskedSuspect(nextTarget, Thorne->LastQuestionAsked) && i < maxQuestions);


                // Advance index for the NEXT exploration turn
                activeSuspectIndex = (activeSuspectIndex + 1);
                if (activeSuspectIndex >= quantitySustec) {
                    printf("round passed\n");
                    activeSuspectIndex = 0;
                    rounds++;
                }
            }

            if (nextTarget) {
                Category catParam = AskFor ? Category::ToAsk : Category::Safe;
                if (i < maxQuestions) {
                    Thorne->Interrogate(nextTarget, Thorne->LastQuestionAsked, catParam);
                    Thorne->LastSuspectedInterrogates = nextTarget->ID;
                }
                else {
                    Thorne->Talk(7, nextTarget->ID);
                }
                Thorne->Talk(8);
                target = nextTarget; // Update the global target for rendering
            }
        }
        int question = 0;
        for (auto& q : currentScenario.timelineQuestions) {
            if (question == Thorne->LastQuestionAsked){
                ImGui::TextColored(ImVec4(0, 255, 0, 100), ("Last Question Asked " + q.text).c_str());
            }
            else if (Thorne->QuestionAskedSuspect(target, question)) {
                ImGui::TextColored(ImVec4(255, 255, 0, 100), ("Already Stored " + q.text).c_str());
            }
            else {
                ImGui::Text(q.text.c_str());

            }
            question++;
        }
        ImGui::End();
    }

    ImGui::Begin("Suspect Files");
    for (int i = 0; i < suspects.size(); i++) {
        if (i == Thorne->hatefullness[activeSuspectIndex].id) {
            ImGui::TextColored(ImVec4(0, 255, 0, 100), ("Asking: " + suspects[i]->name).c_str());
        }
        else if (i == Thorne->LastSuspectedInterrogates - 101){
            ImGui::TextColored(ImVec4(0, 255, 255, 100), ("Last Asked: " + suspects[i]->name).c_str());
        }
        else{

            ImGui::Text((suspects[i]->name).c_str());
        }
    }
    ImGui::End();
}

void C_GameManager::LoadLowIQWords(std::string filepath)
{
    std::ifstream file(filepath);
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string tostore;
        std::getline(ss, tostore, '\n');

        Thorne->RandomLowIQWords.push_back(tostore);
    }

}

void C_GameManager::InitializeGame()
{
    srand(static_cast<unsigned int>(time(NULL)));
    C_BasePawn::CharacterOrigin CharacterOrigin = C_BasePawn::GenerateRandomOrigin(true);
    Thorne = new C_Detective(CharacterOrigin.country, CharacterOrigin.cluster, true, true,101,0);
    LoadScenarioFromCSV("./data/Gala/Questions.csv");
    LoadRoomsFromCSV("./data/Gala/Locations.csv");
    LoadLowIQWords("./data/Words.txt");

    int Alibis = rand() % 5 + 1;

    //printf("Alibis: %d\n", Alibis);
    LoadAlibisFromCSV("./data/Gala/Alibis/" + std::to_string(Alibis) + ".csv");
    for (int i = 1; i < quantitySustec + 1; i++) {
        C_BasePawn::CharacterOrigin susOrigin = C_BasePawn::GenerateRandomOrigin(false);
        std::shared_ptr<C_Suspect> s = std::make_shared<C_Suspect>(susOrigin.country, susOrigin.cluster, rand() % 2, rand() % 2, false, "Suspect_" + std::to_string(i), 0.7f);
        s->ID = 100 + i;

        // Assign a random Alibi Path from the library
        s->realTimeline = currentScenario.alibiLibrary[i-1];

        suspects.push_back(s);

    }
    Thorne->GenerateSuspectOrder(suspects);

    switch (Alibis) {
    case(1):
        suspects.at(4)->guilty = true;
        break;
    case(2):
        suspects.at(1)->guilty = true;
        break;
    case(3):
        suspects.at(2)->guilty = true;
        break;
    case(4):
        suspects.at(9)->guilty = true;
        break;
    case(5):
        suspects.at(0)->guilty = true;
        break;
    }
}
