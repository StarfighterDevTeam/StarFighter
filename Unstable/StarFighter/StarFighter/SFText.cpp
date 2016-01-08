#include "SFText.h"

SFText::SFText(sf::Font* font, int size, sf::Color color, sf::Vector2f position, PlayerTeams team)
{
	setFont(*font);
	setCharacterSize(size);
	setColor(color);
	setPosition(position);

	m_visible = true;
	m_team = team;

	m_alliance = AllianceNeutral;
}