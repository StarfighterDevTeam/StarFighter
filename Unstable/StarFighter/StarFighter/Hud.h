#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Independant.h"

class Game;

#define ARMOR_BAR_SIZE_X		20
#define ARMOR_BAR_SIZE_Y		200

class PlayerHud
{
public:
	PlayerHud::PlayerHud();
	void Init(int m_armor, int m_shield);
	void Update(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_score, int m_hazard_break_value, std::string scene_name, sf::Time deltaTime);
	void Draw (sf::RenderWindow* window);

private:
	sf::RectangleShape armorBar;
	sf::RectangleShape shieldBar;
	sf::Text Money;
	sf::Text GrazeScore;
	sf::Text SceneName;

	sf::Font* font;
	sf::Font* font2;

	sf::RectangleShape hazardBar;
	sf::RectangleShape hazardBarMax;
	sf::Text* hazardBreakText;
	sf::Text* hazardBreakScore;
	sf::Text* framerate;

	bool no_hazard_value;
};

class HudElement : public Independant
{
public:
	HudElement();
	sf::RectangleShape rect;
	sf::Text title;
	sf::Vector2f title_offset;
	void update(sf::Time deltaTime) override;
	void Draw(sf::RenderWindow* window);
};

#endif // HUD_H_INCLUDED