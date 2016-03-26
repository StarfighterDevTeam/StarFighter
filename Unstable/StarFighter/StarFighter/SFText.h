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
	SFText(const sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position);
	~SFText() {};
	virtual void update(Time deltaTime, float hyperspeedMultiplier);
	SFText* Clone();
	bool m_visible;
	bool m_GarbageMe;
	bool m_DontGarbageMe;
};

#endif // SFTEXT_H_INCLUDED