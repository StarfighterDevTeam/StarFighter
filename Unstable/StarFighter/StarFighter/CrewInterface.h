#ifndef CREWINTERFACE_H_INCLUDED
#define CREWINTERFACE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "CrewMember.h"

#define CREWINTERFACE_SIZE_X			120.f
#define CREWINTERFACE_SIZE_Y			350.f
#define CREWINTERFACE_PORTRAIT_SIZE		80.f
#define CREWINTERFACE_LIFEBAR_SIZE_X	50.f
#define CREWINTERFACE_LIFEBAR_SIZE_Y	10.f

struct CrewInterface
{
	CrewInterface();
	~CrewInterface();

	void Init(CrewMember* crew);
	CrewMember* m_crew;

	void Update();
	void Draw(sf::RenderTexture& screen);

	GameEntity* m_panel;
	sf::Text m_display_name;
	GameEntity* m_portrait;
	sf::Text m_type_text;
	sf::Text m_race_text;
	GameEntity* m_lifebar;
};

#endif //CREWINTERFACE_H_INCLUDED