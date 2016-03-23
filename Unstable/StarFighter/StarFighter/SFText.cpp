#include "SFText.h"

//TEXTS
SFText::SFText(const sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position)
{
	setFont(*font);
	setCharacterSize(size);
	setColor(color);
	setPosition(position);

	m_visible = true;
	m_GarbageMe = false;
}

void SFText::update(Time deltaTime, float hyperspeedMultiplier)
{
	//see override function in class SFTextPop
}

SFText* SFText::Clone()
{
	SFText* clone = new SFText(getFont(), getCharacterSize(), getColor(), getPosition());
	clone->setString(getString());

	return clone;
}