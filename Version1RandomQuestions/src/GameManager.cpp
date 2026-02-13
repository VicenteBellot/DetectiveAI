#include "GameManager.hpp"

#include "BasePawn.hpp"
#include "Suspect.hpp"

#include <fstream>
#include <sstream>

#include <filesystem>
#include <iostream>

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
    ImGui::SliderFloat("Tick Speed", &Speed, 0.1f, 5.0f);
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
    ImGui::Text("Frustration: %.2f", Thorne->frustration);
    ImGui::Text("Racism: %.2f", Thorne->racism);
    ImGui::Text("Homophobia: %.2f", Thorne->homophobia);
    ImGui::Text("Classism: %.2f", Thorne->classism);
    ImGui::Text("Sexism: %.2f", Thorne->sexism);
    ImGui::Text("Current Style: %s", (Thorne->frustration > 0.6f ? "Aggressive" : "Professional"));

    ImGui::End();

    if (activeSuspectIndex != -1) {
        C_Suspect* target = suspects[activeSuspectIndex];

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
        if (tick > Cooldown) {
            tick = 0;
            printf("START OF INTERROGATION\n");
            printf("---------------------\n");
            const int Q_SAW_LEAVE = 5;
            bool AskFor = false;
            C_Suspect* nextTarget = nullptr;

            if (Thorne->LastSuspectedInterrogates != -2) {
                auto& memoryID = Thorne->memory[Thorne->LastSuspectedInterrogates];
                auto it = memoryID.timeline.find(SuspectInfo::KQuestionAsked(Thorne->LastQuestionAsked));

                if (it != memoryID.timeline.end()) {
                    int answertolastquestion = it->second;

                    // Check if we need to follow up (And it's NOT the one-way question)
                    if (memoryID.cat == Category::ToAsk && Thorne->LastQuestionAsked != Q_SAW_LEAVE) {
                        auto& memoryIDsecond = Thorne->memory[answertolastquestion];
                        auto itsecond = memoryIDsecond.timeline.find(SuspectInfo::KQuestionAsked(Thorne->LastQuestionAsked));

                        if (itsecond == memoryIDsecond.timeline.end()) {
                            printf("[FOLLOW UP] Corroborating with Suspect %d\n", answertolastquestion);
                            AskFor = true;
                            nextTarget = suspects[answertolastquestion];
                        }
                    }
                }
            }

            if (!AskFor) {
                nextTarget = suspects[activeSuspectIndex];
                Thorne->LastQuestionAsked = rand() % currentScenario.timelineQuestions.size();

                // Advance index for the NEXT exploration turn
                activeSuspectIndex = (activeSuspectIndex + 1) % quantitySustec;
            }

            if (nextTarget) {
                Category catParam = AskFor ? Category::ToAsk : Category::Safe;
                Thorne->Interrogate(nextTarget, Thorne->LastQuestionAsked, catParam);

                // Update state for next tick
                Thorne->LastSuspectedInterrogates = nextTarget->ID;
                target = nextTarget; // Update the global target for rendering
            }
        }
        float question = 0;
        for (auto& q : currentScenario.timelineQuestions) {
            if (question == Thorne->LastQuestionAsked)
            {
                ImGui::TextColored(ImVec4(0, 255, 0, 100), ("Asked " + q.text).c_str()); 
                question++;
                continue;
            }
            ImGui::Text(q.text.c_str());
            question++;
        }
        

        //if (!Thorne->questionPool.empty()) {
        //    ImGui::Separator();
        //    ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "Pressure Points:");
        //    for (auto& followUp : Thorne->questionPool) {
        //        if (followUp.targetSuspectID == target->ID) {


        //            if (ImGui::Button(followUp.text.c_str())) {
        //                // Handle confrontation logic...
        //            }
        //        }
        //    }
        //}
        ImGui::End();
    }

    ImGui::Begin("Suspect Files");
    for (int i = 0; i < suspects.size(); i++) {
        if (i == activeSuspectIndex) {
            ImGui::TextColored(ImVec4(0, 255, 0, 100), ("Asking: " + suspects[i]->name).c_str());
        }
        else if (i == Thorne->LastSuspectedInterrogates - 101){
            ImGui::TextColored(ImVec4(0, 255, 255, 100), ("Last Asked: " + suspects[i]->name).c_str());
        }
        else {

            ImGui::Text((suspects[i]->name).c_str());
        }
    }
    ImGui::End();
}

void C_GameManager::InitializeGame()
{
    srand(static_cast<unsigned int>(time(NULL)));
    C_BasePawn::CharacterOrigin CharacterOrigin = C_BasePawn::GenerateRandomOrigin(true);
    Thorne = new C_Detective(CharacterOrigin.country, CharacterOrigin.cluster, true, true,0,0);
    LoadScenarioFromCSV("./data/Gala/Questions.csv");
    LoadRoomsFromCSV("./data/Gala/Locations.csv");

    int Alibis = rand() % 5 + 1;

    printf("Alibis: %d\n", Alibis);
    LoadAlibisFromCSV("./data/Gala/Alibis/" + std::to_string(Alibis) + ".csv");
    for (int i = 1; i < quantitySustec + 1; i++) {
        C_BasePawn::CharacterOrigin susOrigin = C_BasePawn::GenerateRandomOrigin(false);
        C_Suspect* s = new C_Suspect(susOrigin.country, susOrigin.cluster, rand()%2, rand() % 2, false, "Suspect_" + std::to_string(i), 0.7f);
        s->ID = 100 + i;

        // Assign a random Alibi Path from the library
        s->realTimeline = currentScenario.alibiLibrary[i-1];

        suspects.push_back(s);

    }

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
