#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "Warship.h"

struct ResourcesInterface
{
	ResourcesInterface();
	~ResourcesInterface();

	Warship* m_warship;

	GameEntity* m_panel;
	GameEntity* m_resources[NB_RESOURCES_TYPES];
	sf::Text m_text_royalnavy;

	GameEntity* m_save_button;
	GameEntity* m_load_button;

	void Init(Warship* ship);
	void Destroy();

	void Update();
	void Draw(sf::RenderTexture& screen);
};

#endif //RESOURCES_H_INCLUDED