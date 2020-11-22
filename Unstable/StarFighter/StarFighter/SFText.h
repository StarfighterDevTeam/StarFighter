#ifndef SFTEXT_H_INCLUDED
#define SFTEXT_H_INCLUDED

#include "HudGrid.h"

using namespace sf;

class SFText : public Text
{
public:
	SFText(){};
	SFText(const sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position);
	~SFText() {};
	SFText* Clone();
	void GarbageMe();

	bool m_visible;
	bool m_garbageMe;
	bool m_DontGarbageMe;
};

class SFRectangle : public RectangleShape
{
public:
	SFRectangle();
	~SFRectangle(){};
	void GarbageMe();

	bool m_visible;
	bool m_garbageMe;
	bool m_DontGarbageMe;
};

#endif // SFTEXT_H_INCLUDED