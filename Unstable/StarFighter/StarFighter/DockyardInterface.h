#ifndef DOCKYARDINTERFACE_H_INCLUDED
#define DOCKYARDINTERFACE_H_INCLUDED

#include "Game.h"
#include "Ship.h"
#include "CrewUnboardInterface.h"

#define DOCKYARDINTERFACE_SIZE_X		CREWUNBOARDINTERFACE_SIZE_X
#define DOCKYARDINTERFACE_SIZE_Y		CREWUNBOARDINTERFACE_SIZE_Y

struct DockyardInterface
{
	DockyardInterface();
	~DockyardInterface();

	void Init(Ship* ship, Location* location);
	void Destroy();

	/*Choice*/void Update(sf::Time deltaTime);
	void Draw(sf::RenderTexture& screen);

	Ship* m_ship;
	Location* m_location;
	GameEntity* m_panel;
	sf::Text m_narrative_text;
};

#endif //DOCKYARDINTERFACE_H_INCLUDED