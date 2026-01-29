#include "Suspect.hpp"

C_Suspect::C_Suspect(Country c, Cluster cl, bool g, bool s, bool guilty, std::string n, float dec)
	: C_BasePawn(c, cl, g, s), guilty(guilty), ID(0), name(n), stress(0.0f), deception(dec) {
    this->ID = 0;
}

void C_Suspect::AddAlibi(std::vector<Statement>& alibi)
{
    realTimeline = alibi;
}

Statement C_Suspect::ProvideStatement(int Question)
{
	Statement emptyStatement{ {}, "UNDEFINED", -2, -2, -2, -2, -2, -2};
	printf("INT QUESTION: %d\n", Question);
	switch (Question)
	{
	case 1: // asking for arriveWithID
		emptyStatement.arriveWithID = realTimeline.begin()->arriveWithID;
		break;
	case 2: // asking for seenWithID
		emptyStatement.seenWithID = realTimeline.begin()->seenWithID;
		break;
	case 3: // asking for vouchID
		emptyStatement.vouchID = realTimeline.begin()->vouchID;
		break;
	case 4: // asking for seenIDLeave
		emptyStatement.seenIDLeave = realTimeline.begin()->seenIDLeave;
		break;
	case 5: // asking for leaveWithID
		emptyStatement.seenWithID = realTimeline.begin()->seenWithID;
		break;
	case 6: // asking for duration
		emptyStatement.duration = realTimeline.begin()->duration;
		break;
	default:
		break;
	}
    return emptyStatement;
}
