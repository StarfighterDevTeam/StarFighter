#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"

#define ARMOR_BAR_SIZE_X		20
#define ARMOR_BAR_SIZE_Y		200

class PlayerHud
{
public:
	PlayerHud();
	void Init(int m_armor, int m_shield);
	void update(int m_armor, int m_shield);
	sf::RectangleShape armorBar;
	sf::RectangleShape shieldBar;
};

#endif // HUD_H_INCLUDED