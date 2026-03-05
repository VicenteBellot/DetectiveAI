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
	Statement emptyStatement{ -2, -2, -2, -2, -2, -2,-2, -2,{}, "UNDEFINED" };
	switch (Question)
	{
	case 0: // asking for arriveWithID
		emptyStatement.arriveWithID = realTimeline.begin()->arriveWithID;
		break;
	case 1: // asking for seenWithID
		emptyStatement.seenWithID = realTimeline.begin()->seenWithID;
		break;
	case 2: // asking for vouchID
		emptyStatement.vouchID = realTimeline.begin()->vouchID;
		break;
	case 3: // asking for seenIDLeave
		emptyStatement.seenIDLeave = realTimeline.begin()->seenIDLeave;
		break;
	case 4: // asking for leaveWithID
		emptyStatement.seenWithID = realTimeline.begin()->seenWithID;
		break;
	case 5: // asking for duration
		emptyStatement.duration = realTimeline.begin()->duration;
		break;
	case 6: // asking for duration
		emptyStatement.sawArriveID = realTimeline.begin()->sawArriveID;
		break;
	case 7: // asking for duration
		emptyStatement.sawTwiceID = realTimeline.begin()->sawTwiceID;
		break;
	default:
		break;
	}
    return emptyStatement;
}
