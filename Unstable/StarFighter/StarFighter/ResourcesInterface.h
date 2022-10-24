#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include "Game.h"

#include "GameEntity.h"
#include "Warship.h"

struct ResourcesInterface
{
public:
	ResourcesInterface();
	~ResourcesInterface();

	GameEntity* m_panel;
	Resource* m_resources[NB_RESOURCES_TYPES];
	GameEntity* m_resources_container[NB_RESOURCES_TYPES];
	sf::Text m_text_royalnavy;

	GameEntity* m_save_button;
	GameEntity* m_load_button;

	Resource* m_upkeep[NB_UPKEEP_COSTS];
	GameEntity* m_upkeep_container[NB_UPKEEP_COSTS];
	sf::Text m_text_upkeep;

	sf::Text m_text_holds;

	void Init(Warship* ship);
	void Destroy();

	void Update();
	void Draw(sf::RenderTexture& screen);

private:
	Warship* m_warship;
};

#endif //RESOURCES_H_INCLUDED