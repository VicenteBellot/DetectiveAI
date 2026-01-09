#ifndef BASE_PAWN_HPP
#define BASE_PAWN_HPP

class C_BasePawn {
public:
	enum Cluster { V1, V2, V3, V4, V5, V6, V7, V8 };
	enum Country { UK, FR, DE, JP, IT, ES, RU, AU, SE, EU };

	Country country;
	Cluster cluster;
	bool gender; // true = male, false = female
	bool sexualOrientation; // true = hetero, false = homo

	C_BasePawn(Country c, Cluster cl, bool g, bool s)
		: country(c), cluster(cl), gender(g), sexualOrientation(s) {
	}
	virtual ~C_BasePawn() = default;
};

#endif // !BASE_PAWN_HPP
