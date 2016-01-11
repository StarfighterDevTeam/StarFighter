#ifndef SFTEXT_H_INCLUDED
#define SFTEXT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>
#include "Globals.h"

using namespace sf;

enum FontReferences
{
	Font_Arial,
	Font_Terminator,
	NBVAL_FontReferences,
};

class SFText : public Text
{
public:
	SFText() {};
	SFText(const sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team);
	~SFText() {};
	virtual void update(Time deltaTime);
	SFText* Clone();
	bool m_visible;
	PlayerTeams m_team;
	TeamAlliances m_alliance;
	bool m_GarbageMe;
};

class SFRectangle : public sf::RectangleShape
{
public:
	SFRectangle() {};
	SFRectangle(sf::Vector2f position, sf::Vector2f size, sf::Color color, float outline_thickness, sf::Color outline_color, PlayerTeams team);
	~SFRectangle() {};
	SFRectangle* Clone();
	bool m_visible;
	PlayerTeams m_team;
	TeamAlliances m_alliance;
	bool m_GarbageMe;
	sf::Color m_color;
	int m_layer;
};

class SFGauge : public SFText
{
public:
	SFGauge() {};
	SFGauge(SFText* text, SFRectangle* gauge, SFRectangle* gauge_container);
	~SFGauge();
	SFGauge* Clone();
	bool m_visible;
	void setVisible(bool visible);
	void setVisible(bool text_visible, bool rectangle);

	void setPosition(sf::Vector2f position);

	SFRectangle* m_gauge;
	SFRectangle* m_container;
	void draw(RenderTexture& screen);
	sf::Vector2f m_offset;

	unsigned int m_curValue;
	unsigned int m_maxValue;
	void update(unsigned int curValue, unsigned int maxValue);
	void UpdateFillColor(Color fill_color);
	void UpdateOutlineColor(Color outline_color);
	void UpdateTextColor(Color text_color);
};

#endif // SFTEXT_H_INCLUDED
