#ifndef CREWOVERBOARDINTERFACE_H_INCLUDED
#define CREWOVERBOARDINTERFACE_H_INCLUDED

#include "Game.h"
#include "Ship.h"
#include "CrewInterface.h"

#define	CREWOVERBOARDINTERFACE_SIZE_X				500
#define CREWOVERBOARDINTERFACE_SIZE_Y				190
#define CREWOVERBOARDINTERFACE_OFFSET_X				100
#define CREWOVERBOARDINTERFACE_OFFSET_Y				16

#define OVERBOARD_DROWNINGBAR_SIZE_X				380.f
#define OVERBOARD_DROWNINGBAR_SIZE_Y				20.f

#define OVERBOARD_DROWNING_TIME						4.f

struct CrewOverboardInterface
{
	CrewOverboardInterface();
	~CrewOverboardInterface();

	void Init(Ship* ship, Ship* enemy_ship);
	void Destroy();

	void Update(sf::Time deltaTime);
	void Draw(sf::RenderTexture& screen);

	int m_slots_avaible;
	vector<CrewMember*> m_rescued;
	vector<GameEntity*> m_crew_slots;
	Ship* m_enemy_ship;

	GameEntity* m_panel;
	GameEntity* m_drowning_bar;
	sf::Text m_narrative_text;

	float m_drowning_timer;
};

#endif //CREWOVERBOARDINTERFACE_H_INCLUDED