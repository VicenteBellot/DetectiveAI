#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP
#include <GL/glew.h>

#include <filesystem>
#include <iostream>

#include "Detective.hpp"
#include "StoryGenerator.hpp"
#include "Jobsystem.hpp"
#include "imgui/imgui.h"

struct T_Pos {
    float x, y;
    bool inlight;
};

class C_GameManager {
public:
    C_GameManager() = default;
    ~C_GameManager();

    int m_LastWindowW = 1500;
    int m_LastWindowH = 850;

    Jobsystem JB;

    

    C_Detective* Thorne;
    std::vector<std::shared_ptr<C_Suspect>> suspects;
    Scenario currentScenario;

    // UI State
    int activeSuspectIndex = 0;
    bool showEvidenceBoard = false;
    int quantitySustec = 10;

    float MaxComaEtilico = 6.26f;

    int rounds = 0;
    int maxroundsPerDay = 1;
    float secsOfNight = 10.0f;  //Time that takes to reach night
    float  m_DayNightTimer = 0.0f;
    bool   m_EndOfDayCycleFired = false;
    bool   EndOfGameByAlcohol = false;

    bool GeneratingStory = false;
    bool GeneratedStory = false;
    bool SavedStory = false;
    bool GeneratingVoice = false;
    bool GeneratedVoice = false;

    std::future<std::string> storyFuture;
    void GenerateStoryFuture();
    void SaveStory();

    void GenerateAudioFuture();

    void MainLoop(float deltaTime);

    void CheckAndHandleResize(int newWidth, int newHeight);

    void AddBeerPosition();

    void DrawSpriteWindow();
    std::vector<GLuint> m_TextureIDs;
    std::vector<T_Pos> BeerPos;

    void EndOfDayCycle();
    void EndOfDayImage();
    void DeathImage();

    float Speed = 1.0f;

    void LoadScenarioFromCSV(std::string filePath);
    void LoadAlibisFromCSV(std::string filePath);
    void LoadRoomsFromCSV(std::string filePath);
    void LoadFonts();
    std::vector< ImFont*> Fonts;

    void LoadImages(std::string folderPath);

    void DrawInterface(float DeltaTime);

    void LoadLowIQWords(std::string filepath);

    void InitializeGame();

    float Cooldown = 5.0f;
    float tick = 0.0f;

    unsigned int AskedQuestion = -1;
    
};

#endif // !GAMEMANAGER_HPP
