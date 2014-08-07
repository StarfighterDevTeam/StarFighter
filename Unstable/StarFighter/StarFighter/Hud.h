#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"

class Game;

#define ARMOR_BAR_SIZE_X		20
#define ARMOR_BAR_SIZE_Y		200

class PlayerHud
{
public:
	PlayerHud::PlayerHud();
	void Init(int m_armor, int m_shield);
	void update(sf::RenderWindow* window, int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_score, int m_hazard_break_value, sf::Time deltaTime);
	void draw (sf::RenderWindow* window);

private:
	sf::RectangleShape armorBar;
	sf::RectangleShape shieldBar;
	sf::Text HazardScore;
	sf::Text GrazeScore;

	sf::Font* font;
	sf::Font* font2;

	sf::RectangleShape hazardBar;
	sf::RectangleShape hazardBarMax;
	sf::Text* hazardBreakText;
	sf::Text* hazardBreakScore;
	sf::Text* framerate;
};

#endif // HUD_H_INCLUDED