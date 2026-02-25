#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "Detective.hpp"

#include "imgui/imgui.h"

class C_GameManager {
public:
    C_GameManager() {
    }

    ~C_GameManager();

    C_Detective* Thorne;
    std::vector<std::shared_ptr<C_Suspect>> suspects;
    Scenario currentScenario;

    // UI State
    int activeSuspectIndex = 0;
    bool showEvidenceBoard = false;
    int quantitySustec = 10;

    float MaxComaEtilico = 6.26;

    int rounds = 0;
    int maxroundsPerDay = 2;

    void MainLoop(float deltaTime);

    void EndOfDayCycle();

    float Speed = 1.0f;

    void LoadScenarioFromCSV(std::string filePath);
    void LoadAlibisFromCSV(std::string filePath);
    void LoadRoomsFromCSV(std::string filePath);
    void DrawInterface(float DeltaTime);

    void LoadLowIQWords(std::string filepath);

    void InitializeGame();

    float Cooldown = 5.0f;
    float tick = 0.0f;

    unsigned int AskedQuestion = -1;

    
};

#endif // !GAMEMANAGER_HPP
