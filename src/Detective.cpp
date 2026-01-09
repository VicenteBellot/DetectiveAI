#include "Detective.hpp"
#include "Suspect.hpp"

void C_Detective::Remember(C_Suspect& sus, const std::string& question, const std::string& answer, Statement cat)
{
    if (memory.find(sus.ID) == memory.end()) {
        SuspectInfo newInfo;
        newInfo.ID = sus.ID;
        newInfo.suspectRef = &sus; // Store the address of the suspect pawn
        newInfo.name = sus.name;
        memory.emplace(sus.ID, newInfo);
    }

    memory[sus.ID].AddEntry(question, answer, cat);
}

const SuspectInfo* C_Detective::GetInfo(const int& suspectID) const
{
    auto it = memory.find(suspectID);
    if (it != memory.end()) {
        return &(it->second);
    }
    return nullptr; // empty if not found
}

void C_Detective::UpdateShortlist()
{
    suspectedIDs.clear();
    std::vector<std::pair<int, float>> rankings;
    float score;
    for (auto const& [id, info] : memory) {
        score = 0.0f;

        // 1. Logic Weight: Count suspicious notes
        for (const auto& stmt : info.timeline) {
            if (stmt.cat == Suspicious) {
                score += 2.0f;
            }
        }

        // 2. Bias Weight: If the detective is biased, he "imagines" more guilt
        // High IQ reduces the impact of bias on the final score
        float biasImpact = GetDishonestyBias(*info.suspectRef);
        float mentalClarity = std::clamp((IQ - 120.0f) / 40.0f, 0.0f, 1.0f);

        float prejudiceScore = biasImpact * (1.0f - mentalClarity);

        rankings.push_back({ id, score + prejudiceScore });
    }

    // Sort by score (highest guilt first)
    std::sort(rankings.begin(), rankings.end(), [](auto& a, auto& b) {
        return a.second > b.second;
        });

    // Take the top 2 as the "Shortlist"
    for (int i = 0; i < std::min(2, (int)rankings.size()); ++i) {
        suspectedIDs.push_back(rankings[i].first);
    }
}

void C_Detective::ApplyPressure(C_Suspect& sus)
{
    InterrogationStyle style = this->DecideStyle(sus);

    float pressureAmount = 0.0f;

    switch (style) {
    case Aggressive:
        pressureAmount = 1.5f + (this->frustration * 0.5f); // High pressure
        break;
    case Formal:
        pressureAmount = 1.0f;
        break;
    case Casual:
        pressureAmount = 0.5f;
        break;
    }

    sus.UpdateStress(pressureAmount, this->trust);
}

void C_Detective::AdjustBias(const C_Suspect& sus, bool foundLie)
{
    float adjustment = foundLie ? 0.05f : -0.05f;

    if (BaseBias[sus.cluster][0] > 0) classism += adjustment;
    if (BaseBias[sus.cluster][3] > 0) racism += adjustment;
    if (sus.gender != this->gender) sexism += adjustment;
    if (sus.sexualOrientation != this->sexualOrientation) homophobia += adjustment;

    classism = std::clamp(classism, 0.0f, 1.0f);
    racism = std::clamp(racism, 0.0f, 1.0f);
    sexism = std::clamp(sexism, 0.0f, 1.0f);
    homophobia = std::clamp(homophobia, 0.0f, 1.0f);
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

void C_Detective::Interrogate(C_Suspect& sus, const std::string& question, const std::string& answer, Statement s)
{

    bool foundContradiction = false;

    for (const auto& prevStmt : memory[sus.ID].timeline) {
        if (prevStmt.startTime == s.startTime && prevStmt.location != s.location) { // redundant with the remember func
            FlagAsSuspicious(sus.ID, s.location);
            FlagAsSuspicious(sus.ID, prevStmt.location);
            foundContradiction = true;
        }
    }

    if (VerifyWithPartner(sus.ID, s)) {
        foundContradiction = true;
    }

    this->Remember(sus, question, answer, s);

    if (foundContradiction) {
        this->frustration += 0.15f;    // Getting more annoyed
        this->trust -= 0.1f;           // Global trust drops
        AdjustBias(sus, true);         // "I knew someone like you would lie!"
    }
    else {
        this->frustration = std::max(0.0f, this->frustration - 0.05f);    // Calming down
        if (s.cat == Safe) AdjustBias(sus, false);
    }
}

float C_Detective::AssessGuilt(const C_Suspect& sus) const
{
    float biasImpact = GetAggreivityBias(sus); // bias formula

    // Retrieve notes taken so far
    const SuspectInfo* info = this->GetInfo(sus.ID);
    float evidenceWeight = 0.0f;

    if (info) {
        for (auto cat : info->timeline) {
            if (cat.cat == Suspicious) evidenceWeight += 0.2f;
        }
    }

    return evidenceWeight + (biasImpact * 0.3f);
}

bool C_Detective::VerifyWithPartner(int suspectID, const Statement& s)
{
    if (s.seenWithID == -1) return false;

    const SuspectInfo* partnerInfo = GetInfo(s.seenWithID);
    if (!partnerInfo) return false; // Not talked with him

    bool foundMatch = false;
    for (const auto& pStmt : partnerInfo->timeline) {
        // Check if times overlap
        if (pStmt.startTime == s.startTime) {
            // Check if the partner confirms being with this suspect
            if (pStmt.seenWithID == suspectID && pStmt.location == s.location) {
                foundMatch = true;
                break;
            }
        }
    }

    if (!foundMatch) {
        // Contradiction: Partner doesn't back up the story
        FlagAsSuspicious(suspectID, s.location);
        FlagAsSuspicious(s.seenWithID, s.location);
        this->trust -= 0.15f;
        return true;
    }
    return false;
}

bool C_Detective::CheckForContradictions(const std::string& currentAnswer, Category cat) const
{
    for (auto const& [id, info] : memory) {
        for (size_t i = 0; i < info.generalNotes.size(); ++i) {
            if (info.timeline[i].cat == cat) {
                const std::string& knownFact = info.generalNotes[i].second;

                if (knownFact != currentAnswer) {
                    return true; // Found a contradiction!
                }
            }
        }
    }
    return false;
}

void C_Detective::FlagAsSuspicious(int suspectID, const std::string& answer)
{
    auto it = memory.find(suspectID);
    if (it != memory.end()) {
        for (size_t i = 0; i < it->second.generalNotes.size(); ++i) {
            if (it->second.generalNotes[i].second == answer) {
                it->second.timeline[i].cat = Suspicious;
                break;
            }
        }
    }
}

void C_Detective::GenerateFollowUp(int suspectID, int conflictingID, const Statement& s)
{
    InvestigationQuestion q;
    q.targetSuspectID = suspectID;
    q.targetCategory = Suspicious;
    q.isPressure = false;

    std::string partnerName = "someone";
    auto it = memory.find(s.seenWithID);
    if (it != memory.end() && !it->second.name.empty()) {
        partnerName = it->second.name;
    }
    // Customizing the text based on the contradiction
    C_Suspect* sus = GetSuspectByID(suspectID);
    bool shouldBeAggressive = false;

    if (sus) {
        // Thorne gets aggressive if he's frustrated or his bias kicks in
        shouldBeAggressive = (this->DecideStyle(*sus) == Aggressive || sus->stress > 0.7f);
    }

    if (shouldBeAggressive) {
        q.isPressure = true;
        if (s.seenWithID != -1) {
            q.text = "[SLAMS TABLE] " + partnerName + " says you're lying about the " + s.location + "! Talk!";
        }
        else {
            q.text = "[SHOUTING] You weren't at the " + s.location + ". Where were you really?!";
        }
    }
    else {
        // original logic
        if (s.seenWithID != -1) {
            q.text = "I just spoke with " + partnerName + ". They don't remember being in the " + s.location + " with you. Care to explain?";
        }
        else {
            q.text = "You've got a hole in your timeline at " + std::to_string(s.startTime) + ". What's the truth?";
        }
    }

    this->questionPool.push_back(q);
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
