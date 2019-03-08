#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "Warship.h"

#define RESOURCES_INTERFACE_PANEL_SIZE_Y		40
#define RESOURCES_INTERFACE_OFFSET_X			30
#define RESOURCES_INTERFACE_SIZE_X				110
#define RESOURCES_INTERFACE_SIZE_Y				30
#define RESOURCES_ICON_SIZE						30
#define RESOURCES_CREWFIDELITY_SIZE_X			250
#define RESOURCES_CREWFIDELITY_SIZE_Y			12

struct ResourcesInterface
{
	ResourcesInterface();
	~ResourcesInterface();

	Warship* m_warship;

	GameEntity* m_panel;
	GameEntity* m_resources[NB_RESOURCES_TYPES];
	sf::Text m_text_royalnavy;

	void Init(Warship* ship);
	void Destroy();

	void Update();
	void Draw(sf::RenderTexture& screen);
};

#endif //RESOURCES_H_INCLUDED