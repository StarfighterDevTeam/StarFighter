#ifndef SFTEXT_H_INCLUDED
#define SFTEXT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>
#include "Globals.h"

using namespace sf;

enum PlayerTeams
{
	PlayerBlue,
	PlayerRed,
	PlayerBlue2,
	PlayerRed2,
	PlayerNeutral,
	NBVAL_PlayerTeams,
};

enum TeamAlliances
{
	Alliance1,
	Alliance2,
	AllianceNeutral,
	NBVAL_TeamAlliances,
};

class SFText : public Text
{
public:
	SFText() {};
	SFText(sf::Font* font, int size, sf::Color color, sf::Vector2f position, PlayerTeams team);
	~SFText() {};
	bool m_visible;
	PlayerTeams m_team;
	TeamAlliances m_alliance;
};

#endif // SFTEXT_H_INCLUDED
