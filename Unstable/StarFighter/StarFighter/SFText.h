#ifndef SFTEXT_H_INCLUDED
#define SFTEXT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>
#include "Globals.h"

using namespace sf;

class SFText : public Text
{
public:
	SFText() {};
	SFText(sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position);
	~SFText() {};
	virtual void update(Time deltaTime);
	bool m_visible;
	bool m_GarbageMe;
	void SetPosition(sf::Vector2f position);
};

class SFRectangle : public sf::RectangleShape
{
public:
	SFRectangle() {};
	SFRectangle(sf::Vector2f position, sf::Vector2f size, sf::Color color, float outline_thickness, sf::Color outline_color);
	~SFRectangle() {};
	bool m_visible;
	bool m_GarbageMe;
};

class SFGauge : public sf::RectangleShape
{
public:
	SFGauge() {};
	SFGauge(SFText text, SFRectangle rectangle);
	~SFGauge() {};
	bool m_visible;
	void setVisible(bool visible);
	void setVisible(bool text_visible, bool rectangle);

	void setString(string& str);
	void setPosition(sf::Vector2f position, sf::Vector2f offset);
	bool m_GarbageMe;

	SFText m_SFText;
	SFRectangle m_SFRectangle;
};

#endif // SFTEXT_H_INCLUDED
