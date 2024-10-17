#include "SFText.h"

SFText::SFText(sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position)
{
	setFont(*font);
	setCharacterSize(size);
	setFillColor(color);
	setPosition(position);

	m_visible = true;

	m_GarbageMe = false;
}

void SFText::update(Time deltaTime)
{
	//see override function in class SFTextPop
}

void SFText::SetPosition(sf::Vector2f position)
{
	float globalBounds_x = getGlobalBounds().width;
	float globalBounds_y = getGlobalBounds().height;

	setPosition(sf::Vector2f(position.x - globalBounds_x * 0.5f, position.y - globalBounds_y * 0.5f));
}

SFRectangle::SFRectangle(sf::Vector2f position, sf::Vector2f size, sf::Color color, float outline_thickness, sf::Color outline_color)
{
	setSize(size);
	setOrigin(size.x / 2, size.y/2);
	setFillColor(color);
	setOutlineThickness(outline_thickness);
	setOutlineColor(outline_color);

	m_visible = true;

	m_GarbageMe = false;
}

SFGauge::SFGauge(SFText text, SFRectangle rectangle)
{
	m_visible = true;
	m_SFText = text;
	m_SFRectangle = rectangle;

	m_GarbageMe = text.m_visible || rectangle.m_visible;
}

void SFGauge::setVisible(bool visible)
{
	m_visible = visible;
	m_SFText.m_visible = visible;
	m_SFRectangle.m_visible = visible;
}

void SFGauge::setVisible(bool text_visible, bool rectangle_visible)
{
	m_SFText.m_visible = text_visible;
	m_SFRectangle.m_visible = rectangle_visible;

	m_visible = text_visible || rectangle_visible;
}

void SFGauge::setString(string& str)
{
	printf("string : %s", str.c_str());
	m_SFText.setString("hello");
}

void SFGauge::setPosition(sf::Vector2f position, sf::Vector2f offset)
{
	m_SFText.setPosition(sf::Vector2f(position.x + offset.x, position.y + offset.y));
	m_SFRectangle.setPosition(sf::Vector2f(position.x + offset.x, position.y + offset.y));
}