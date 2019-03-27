#ifndef CREWUNBOARDINTERFACE_H_INCLUDED
#define CREWUNBOARDINTERFACE_H_INCLUDED

#include "Game.h"
#include "Ship.h"
#include "CrewInterface.h"
#include "PrisonersChoiceInterface.h"

#define	CREWUNBOARDINTERFACE_SIZE_X				PRISONERSCHOICEINTERFACE_SIZE_X
#define CREWUNBOARDINTERFACE_SIZE_Y				PRISONERSCHOICEINTERFACE_SIZE_Y

#define UNBOARD_CREW_CARD_OFFSET				OVERBOARD_CREW_CARD_OFFSET

struct CrewUnboardInterface
{
	CrewUnboardInterface();
	~CrewUnboardInterface();

	void Init(Ship* ship, Island* island);
	void Destroy();

	bool Update(sf::Time deltaTime);
	void Draw(sf::RenderTexture& screen);

	bool AddCrewToInterface(CrewMember* crew);
	void RemoveCrewFromInterface(CrewMember* crew);

	int m_slots_avaible;
	vector<CrewMember*> m_unboarded;
	vector<GameEntity*> m_crew_slots;
	Ship* m_ship;
	Island* m_island;

	CrewMember* m_hovered;

	struct CrewInterface m_crew_interface;

	GameEntity* m_panel;
	sf::Text m_narrative_text;
	sf::Text m_crew_slot_text;

	Choice m_choices[4];
};

#endif //CREWUNBOARDINTERFACE_H_INCLUDED