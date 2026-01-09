#include "Suspect.hpp"

C_Suspect::C_Suspect(Country c, Cluster cl, bool g, bool s, bool guilty, std::string n, float dec)
	: C_BasePawn(c, cl, g, s), guilty(guilty), ID(0), name(n), stress(0.0f), deception(dec) {
    this->ID = 0;
}

void C_Suspect::UpdateStress(float detectiveAggression, float detectiveTrust)
{
    float multiplier = guilty ? 1.5f : 1.0f;
    float change = (detectiveAggression * 0.2f) - (detectiveTrust * 0.1f);
    stress = std::clamp(stress + (change * multiplier), 0.0f, 1.0f);
}

bool C_Suspect::ShouldLie(float detectiveIQ) const
{
    if (!guilty) return false;

    float lieChance = deception - (stress * 0.5f);

    float detectionRisk = detectiveIQ / 200.0f;

    return lieChance > detectionRisk;
}

Statement C_Suspect::ProvideStatement(int hour, const std::vector<std::string>& worldLocations)
{
    Statement s = realTimeline[hour]; // The Truth

    if (!guilty) return s;

    if (ShouldLie(140.0f)) { // Assuming an average Thorne IQ for the check
        // Determine the "Lie Pool"
        // If low stress, they stay within their knownLocations (Safe Lie)
        // If high stress, they might pick any worldLocation (Risky Lie)

        const std::vector<std::string>& pool = (stress < 0.7f) ? knownLocations : worldLocations;
        std::vector<std::string> options;

        for (const auto& loc : pool) {
            if (loc != s.location) options.push_back(loc);
        }

        if (!options.empty()) {
            s.location = options[rand() % options.size()];
            s.cat = Safe; // They attempt to look innocent
        }
    }
    else if (stress > 0.85f) {
        // The suspect "Cracks" - providing a suspicious, vague answer
        s.location = "I... I don't know anymore.";
        s.cat = Suspicious;
    }

    return s;
}
