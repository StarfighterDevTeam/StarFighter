#include "CrewMember.h"

extern Game* CurrentGame;

string dico_crew[NB_CREW_TYPES] = {
	"",			//Crew_Unknown,
	"Pirate",	//Crew_Pirate,
	"Civilian",	//Crew_Civilian,
	"Slave",	//Crew_Slave,
	"Mecha",	//Crew_Mecha,
};

CrewMember::CrewMember(CrewMemberType type)
{
	m_type = type;

	m_size = sf::Vector2f(CREWMEMBER_SIZE, CREWMEMBER_SIZE);

	//UI
	m_shape_container.setSize(sf::Vector2f(m_size));
	m_shape_container.setOrigin(sf::Vector2f(m_size.x * 0.5f, m_size.y * 0.5f));
	m_shape_container.setOutlineThickness(1.f);
	m_shape_container.setOutlineColor(sf::Color::White);

	switch (type)
	{
		case Crew_Pirate:
		{
			m_shape_container.setFillColor(sf::Color::Red);
			break;
		}
		case Crew_Civilian:
		{
			m_shape_container.setFillColor(sf::Color::Cyan);
			break;
		}
		case Crew_Slave:
		{
			m_shape_container.setFillColor(sf::Color::Magenta);
			break;
		}
		case Crew_Mecha:
		{
			m_shape_container.setFillColor(sf::Color::Green);
			break;
		}
	}

	//m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//m_text.setCharacterSize(20);
	//m_text.setColor(sf::Color::White);
	//m_text.setString(dico_crew[type]);
}