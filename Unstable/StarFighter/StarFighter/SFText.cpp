#include "SFText.h"

SFText::SFText(sf::Font* font, int size, sf::Color color, sf::Vector2f position, PlayerTeams team)
{
	setFont(*font);
	setCharacterSize(size);
	setColor(color);
	setPosition(position);

	m_visible = true;
	m_team = team;
	m_alliance = (TeamAlliances)0;

	m_GarbageMe = false;
}

SFRectangle::SFRectangle(sf::RectangleShape rectangle, PlayerTeams team)
{
	m_visible = true;
	m_team = team;
	m_alliance = (TeamAlliances)0;

	m_GarbageMe = false;
}

SFGauge::SFGauge(SFText text, SFRectangle rectangle)
{
	m_visible = true;
	m_SFText = text;
	m_SFRectangle = rectangle;

	m_GarbageMe = false;
}

void SFGauge::SetVisible(bool visible)
{
	m_visible = visible;
	m_SFText.m_visible = visible;
	m_SFRectangle.m_visible = visible;
}