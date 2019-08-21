#include "SFText.h"

SFText::SFText(sf::Font* font, unsigned int size, sf::Color color, sf::Vector2f position, PlayerTeams team)
{
	setFont(*font);
	setCharacterSize(size);
	setColor(color);
	setPosition(position);

	m_visible = true;
	m_team = team;
	m_alliance = AllianceNeutral;

	m_garbageMe = false;
}

void SFText::Update(Time deltaTime)
{
	//see override function in class SFTextPop
}

SFRectangle::SFRectangle(sf::Vector2f position, sf::Vector2f size, sf::Color color, float outline_thickness, sf::Color outline_color, PlayerTeams team)
{
	setSize(size);
	setOrigin(size.x / 2, size.y/2);
	setFillColor(color);
	setOutlineThickness(outline_thickness);
	setOutlineColor(outline_color);

	m_visible = true;
	m_team = team;
	m_alliance = (TeamAlliances)0;

	m_garbageMe = false;
}

SFGauge::SFGauge(SFText text, SFRectangle rectangle)
{
	m_visible = true;
	m_SFText = text;
	m_SFRectangle = rectangle;

	m_garbageMe = text.m_visible || rectangle.m_visible;
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
	printf("string : %s", str);
	m_SFText.setString("hello");
}

void SFGauge::setPosition(sf::Vector2f position, sf::Vector2f offset)
{
	m_SFText.setPosition(sf::Vector2f(position.x + offset.x, position.y + offset.y));
	m_SFRectangle.setPosition(sf::Vector2f(position.x + offset.x, position.y + offset.y));
}