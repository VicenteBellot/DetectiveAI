#ifndef DETECTIVE_PAWN_HPP
#define DETECTIVE_PAWN_HPP

#include <vector>
#include "BasePawn.hpp"
#include "Information.hpp"
#include <robinmap/robin_hood.h>

struct THate {
    float hate = -1;
    int id;
};

class C_Suspect;

class C_Detective : public C_BasePawn{

/// SE LLAMA Inspector Gustav ArandAI

public:
    C_Detective(Country c, Cluster cl, bool g, bool s,
        int iq, float trust);
    
    ~C_Detective();
    
    void SetBias(std::string filePath);

    enum InterrogationStyle { Casual, Formal, Aggressive };


    robin_hood::unordered_map<int, SuspectInfo> memory;

    void GenerateSuspectOrder(std::vector<std::shared_ptr<C_Suspect>> suspects);

    void Remember(std::shared_ptr<C_Suspect> sus, Statement cat);

    bool QuestionAskedSuspect(std::shared_ptr<C_Suspect> sus, int Question);

    const SuspectInfo* GetInfo(const int& suspectID) const;

    std::vector<THate>hatefullness;

    float IgnoreChange = 99; //over 100
    bool WillIgnore = false;

    float classism;
    float racism;
    float homophobia;
    float sexism;
    int IQ;
    float trust; // 0.0–1.0

    bool FoundCulprit = false;
    int Culprit = -2;

    float frustration = 0;

    int FirstSuspectedInterrogates = -2;

    int LastSuspectedInterrogates = -2;
    int LastQuestionAsked = -2;

    std::shared_ptr<C_Suspect> GetSuspectByID(int targetID) const;

    InterrogationStyle DecideStyle(const C_Suspect& sus) const;
    void AnalyzeAlibis(int idA, int idB);
    void Interrogate(std::shared_ptr<C_Suspect> sus, int Question, Category cat);
    
    float GetAggreivityBias(const C_Suspect& sus) const;
    float GetDishonestyBias(const C_Suspect& sus) const;

    static constexpr float CountryDishonestyTable[10][10] = {
        // UK,   EU,   FR,   DE,   JP,   IT,   ES,   RU,   AU,   SE  
        { 0.0f, 0.6f, 0.3f, 0.0f, 0.0f, 0.6f, 0.3f, 0.3f, 0.6f, 0.0f }, // UK (Detective)
        { 0.6f, 0.0f, 0.3f, 0.3f, 0.6f, 0.3f, 0.0f, 0.9f, 0.0f, 0.3f }, // EU (Detective)
        { 0.3f, 0.3f, 0.0f, 0.6f, 0.3f, 0.0f, 0.9f, 0.6f, 0.3f, 0.3f }, // FR (Detective)
        { 0.0f, 0.3f, 0.6f, 0.0f, 0.0f, 0.9f, 0.6f, 0.3f, 0.9f, 0.0f }, // DE (Detective)
        { 0.0f, 0.6f, 0.3f, 0.0f, 0.0f, 0.6f, 0.6f, 0.3f, 0.9f, 0.0f }, // JP (Detective)
        { 0.6f, 0.3f, 0.0f, 0.9f, 0.6f, 0.0f, 0.0f, 0.6f, 0.3f, 0.6f }, // IT (Detective)
        { 0.3f, 0.0f, 0.9f, 0.6f, 0.6f, 0.0f, 0.0f, 0.3f, 0.0f, 0.6f }, // ES (Detective)
        { 0.3f, 0.9f, 0.6f, 0.3f, 0.3f, 0.6f, 0.3f, 0.0f, 0.9f, 0.3f }, // RU (Detective)
        { 0.6f, 0.0f, 0.3f, 0.9f, 0.9f, 0.3f, 0.0f, 0.9f, 0.0f, 0.3f }, // AU (Detective)
        { 0.0f, 0.3f, 0.3f, 0.0f, 0.0f, 0.6f, 0.6f, 0.3f, 0.3f, 0.0f }, // SE (Detective)
    };

    static constexpr float CountryAgressivityTable[10][10] = {
        // UK,   EU,   FR,   DE,   JP,   IT,   ES,   RU,   AU,   SE  
        { 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f, 0.0f }, // UK (Detective)
        { 0.8f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f, 0.0f, 0.8f, 0.0f, 0.8f }, // EU (Detective)
        { 0.8f, 0.8f, 0.0f, 0.8f, 0.8f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f }, // FR (Detective)
        { 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f, 0.0f }, // DE (Detective)
        { 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f, 0.0f }, // JP (Detective)
        { 0.8f, 0.8f, 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f }, // IT (Detective)
        { 0.8f, 0.0f, 0.8f, 0.8f, 0.8f, 0.0f, 0.0f, 0.8f, 0.0f, 0.8f }, // ES (Detective)
        { 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.0f, 0.8f, 0.8f }, // RU (Detective)
        { 0.8f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f, 0.0f, 0.8f, 0.0f, 0.8f }, // AU (Detective)
        { 0.0f, 0.8f, 0.8f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f, 0.8f, 0.0f }, // SE (Detective)
    };

    // Base bias for each Cluster
    static constexpr float BaseBias[8][4] = {
        {0.0f, 0.0f, 0.0f, 0.0f},   // V1
        {0.0f, 0.0f, 0.0f, 0.0f},   // V2
        {0.3f, 0.0f, 0.0f, 0.3f},   // V3
        {0.0f, 0.0f, 0.0f, 0.0f},   // V4
        {0.6f, 0.3f, 0.0f, 0.6f},   // V5
        {0.6f, 0.3f, 0.0f, 0.6f},   // V6
        {0.3f, 0.0f, 0.0f, 0.3f},   // V7
        {0.3f, 0.0f, 0.0f, 0.3f}    // V8
    };

    // Archetype of each detective with both Cluster
    static constexpr float DetectiveArchetype[19][4] = {
        {0.6f, 0.6f, 0.3f, 0.3f},{0.3f, 0.3f, 0.3f, 0.0f},  // UK V1 & V2
        {0.3f, 0.3f, 0.3f, 0.3f},                           // EU V8
        {0.3f, 0.3f, 0.3f, 0.3f},{0.6f, 0.3f, 0.3f, 0.3f},  // FR V2 & V4
        {0.3f, 0.3f, 0.0f, 0.0f},{0.3f, 0.3f, 0.0f, 0.0f},  // DE V1 & V4
        {0.6f, 0.9f, 0.6f, 0.6f},{0.6f, 0.6f, 0.6f, 0.3f},  // JP V4 & V2
        {0.3f, 0.9f, 0.3f, 0.3f},{0.3f, 0.6f, 0.3f, 0.3f},  // IT V2 & V7
        {0.3f, 0.6f, 0.3f, 0.3f},{0.0f, 0.3f, 0.0f, 0.0f},  // ES V7 & V8
        {0.6f, 0.9f, 0.9f, 0.6f},{0.6f, 0.6f, 0.9f, 0.6f},  // RU V1 & V2
        {0.0f, 0.3f, 0.3f, 0.3f},{0.3f, 0.6f, 0.3f, 0.3f},  // AU V8 & V3
        {0.0f, 0.0f, 0.0f, 0.0f},{0.0f, 0.0f, 0.0f, 0.0f},  // SE V2 & V8
    };

};

#endif // !DETECTIVE_PAWN_HPP
