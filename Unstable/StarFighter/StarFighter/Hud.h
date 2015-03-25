#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Independant.h"

#define ARMOR_BAR_SIZE_X		20
#define ARMOR_BAR_SIZE_Y		200

class PlayerHud
{
public:
	PlayerHud();
	void Init(int m_armor, int m_shield);
	void Update(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, sf::Time deltaTime, bool hub);
	void Draw(sf::RenderTexture& offscreen);

	sf::RectangleShape backgroundColor;
	sf::RectangleShape armorBar;
	sf::RectangleShape shieldBar;
	sf::Text Money;
	sf::Text GrazeScore;
	sf::Text SceneName;
	sf::Font* font;
	sf::Font* font2;
	sf::Text* framerate;

	bool max_hazard_level_reached;
	bool has_shield;
};

#endif // HUD_H_INCLUDED