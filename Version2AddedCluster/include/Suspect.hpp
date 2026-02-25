#ifndef SUSPECT_PAWN_HPP
#define SUSPECT_PAWN_HPP

#include "BasePawn.hpp"
#include "Information.hpp"
#include <string>
#include <vector>
#include <algorithm>

class C_Suspect : public C_BasePawn{
public:
    C_Suspect(Country c, Cluster cl, bool g, bool s, bool guilty, std::string n, float dec);
    
    bool guilty;
    int ID;
    std::string name;

    float stress;      // 0.0 (Calm) to 1.0 (Panic)
    float deception;   // Skill at lying (influenced by Cluster/IQ)

    std::vector<std::string> knownLocations = {};
    std::vector<Statement> realTimeline;

    void AddAlibi(std::vector<Statement>& alibi);


    Statement ProvideStatement(int Question);
};

#endif // !SUSPECT_PAWN_HPP
