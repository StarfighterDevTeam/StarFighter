#ifndef PRISONERSCHOICEINTERFACE_H_INCLUDED
#define PRISONERSCHOICEINTERFACE_H_INCLUDED

#include "Game.h"
#include "Ship.h"
#include "CrewInterface.h"
#include "Choice.h"

#define	PRISONERSCHOICEINTERFACE_SIZE_X							900
#define PRISONERSCHOICEINTERFACE_SIZE_Y							800

struct PrisonersChoiceInterface
{
	PrisonersChoiceInterface();
	~PrisonersChoiceInterface();

	void Init(Ship* ship, Ship* enemy_ship);
	void Destroy();

	void Update(sf::Time deltaTime);
	void Draw(sf::RenderTexture& screen);

	vector<CrewMember*> m_crew;
	CrewMember* m_crew_selected;
	CrewMember* m_crew_hovered;
	struct CrewInterface m_crew_interface;

	Ship* m_ship;
	GameEntity* m_panel;
	sf::Text m_narrative_text;

	int m_current_choice;
	Choice m_choices[3];
};

#endif //PRISONERSCHOICEINTERFACE_H_INCLUDED