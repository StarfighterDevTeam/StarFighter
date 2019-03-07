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

enum Resource_Meta
{
	Resource_Gold,
	Resource_Fish,
	Resource_Mech,
	Resource_Fidelity,
	Resource_Days,
	NB_RESOURCES_TYPES,
};

struct ResourcesInterface
{
	ResourcesInterface();
	~ResourcesInterface();

	Warship* m_warship;

	GameEntity* m_panel;
	GameEntity* m_resources[NB_RESOURCES_TYPES];

	void Init(Warship* ship);
	void Destroy();

	void Update();
	void Draw(sf::RenderTexture& screen);
};

#endif //RESOURCES_H_INCLUDED