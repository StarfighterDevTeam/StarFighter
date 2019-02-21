#ifndef CREWINTERFACE_H_INCLUDED
#define CREWINTERFACE_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "CrewMember.h"
#include "Weapon.h"

#define CREWINTERFACE_SIZE_X				130.f
#define CREWINTERFACE_SIZE_Y				400.f
#define CREWINTERFACE_PORTRAIT_SIZE			80.f
#define CREWINTERFACE_LIFEBAR_SIZE_X		50.f
#define CREWINTERFACE_LIFEBAR_SIZE_Y		10.f
#define CREWINTERFACE_SKILLBAR_SIZE_X		70.f
#define CREWINTERFACE_TEXT_OFFSET_X			8.f

struct CrewInterface
{
	CrewInterface();
	~CrewInterface();

	void Init(CrewMember* crew);
	void Destroy();

	CrewMember* m_crew;
	sf::Vector2f m_position;

	void Update();
	void Draw(sf::RenderTexture& screen);
	void SetPosition(sf::Vector2f position);

	GameEntity* m_panel;
	sf::Text m_display_name;
	GameEntity* m_portrait;
	sf::Text m_type_text;
	sf::Text m_race_text;
	GameEntity* m_lifebar;
	sf::Text m_status_text;
	sf::Text m_skillbar_names[NB_CREW_SKILLS];
	GameEntity* m_skillbars[NB_CREW_SKILLS];
};

#endif //CREWINTERFACE_H_INCLUDED