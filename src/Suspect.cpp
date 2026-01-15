#include "Suspect.hpp"

C_Suspect::C_Suspect(Country c, Cluster cl, bool g, bool s, bool guilty, std::string n, float dec)
	: C_BasePawn(c, cl, g, s), guilty(guilty), ID(0), name(n), stress(0.0f), deception(dec) {
    this->ID = 0;
}

void C_Suspect::AddAlibi(std::vector<Statement>& alibi)
{
    realTimeline = alibi;
}

Statement C_Suspect::ProvideStatement(Statement roomName)
{
    Statement emptyStatement;
    for (const auto& statement : realTimeline)
    {
        if (statement.location == roomName.location)
        {
            return statement;
        }
    }

    emptyStatement.location = "None";
    emptyStatement.arriveWithID = -1;
    emptyStatement.seenWithID = -1;
    emptyStatement.vouchID = -1;
    emptyStatement.seenIDLeave = -1;
    emptyStatement.leaveWithID = -1;
    emptyStatement.duration = 0;

    return emptyStatement;
}
