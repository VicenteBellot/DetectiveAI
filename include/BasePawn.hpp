#ifndef BASE_PAWN_HPP
#define BASE_PAWN_HPP

#include <stdlib.h>

class C_BasePawn {
public:
	enum Cluster { V1, V2, V3, V4, V5, V6, V7, V8 };
	enum Country { UK, FR, DE, JP, IT, ES, RU, AU, SE, EU };
	
	struct CharacterOrigin {
		C_BasePawn::Country country;
		C_BasePawn::Cluster cluster;
	};

	Country country;
	Cluster cluster;
	bool gender; // true = male, false = female
	bool sexualOrientation; // true = hetero, false = homo

	C_BasePawn(Country c, Cluster cl, bool g, bool s)
		: country(c), cluster(cl), gender(g), sexualOrientation(s) {
	}
	virtual ~C_BasePawn() = default;

	static CharacterOrigin GenerateRandomOrigin(bool isDetective) {
		C_BasePawn::Country countries[] = { UK, FR, DE, JP, IT, ES, RU, AU, SE, EU };
		C_BasePawn::Country randCountry = countries[rand() % 9];

		C_BasePawn::Cluster randCluster;
		if (isDetective) {
			// Detectives only come from these two "Elite" clusters in this scenario
			switch (randCountry) {
			case UK:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V1 : C_BasePawn::V2;
				break;
			case FR:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V2 : C_BasePawn::V4;
				break;
			case DE:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V1 : C_BasePawn::V4;
				break;
			case JP:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V2 : C_BasePawn::V4;
				break;
			case IT:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V7 : C_BasePawn::V2;
				break;
			case ES:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V7 : C_BasePawn::V8;
				break;
			case RU:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V1 : C_BasePawn::V2;
				break;
			case AU:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V8 : C_BasePawn::V3;
				break;
			case SE:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V2 : C_BasePawn::V8;
				break;
			case EU:
				randCluster = (rand() % 2 == 0) ? C_BasePawn::V8 : C_BasePawn::V4;
				break;
			}
		}
		else {
			randCluster = static_cast<C_BasePawn::Cluster>(rand() % 7);
		}

		return { randCountry, randCluster };
	}
};

#endif // !BASE_PAWN_HPP
