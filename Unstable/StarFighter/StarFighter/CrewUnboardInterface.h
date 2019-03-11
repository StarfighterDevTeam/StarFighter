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

	void Init(Ship* ship);
	void Destroy();

	void Update(sf::Time deltaTime, CrewMember* unboarded_crew);
	void Draw(sf::RenderTexture& screen);

	int m_slots_avaible;
	vector<CrewMember*> m_unboarded;
	vector<GameEntity*> m_crew_slots;
	Ship* m_ship;

	CrewMember* m_hovered;

	GameEntity* m_panel;
	sf::Text m_narrative_text;
	sf::Text m_crew_slot_text;
};

#endif //CREWUNBOARDINTERFACE_H_INCLUDED