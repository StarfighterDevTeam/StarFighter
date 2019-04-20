#ifndef REWARDINTERFACE_H_INCLUDED
#define REWARDINTERFACE_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Choice.h"
#include "Ship.h"

#define REWARD_PANEL_SIZE_X			500
#define REWARD_PANEL_SIZE_Y			160
#define REWARD_INTERFACE_OFFSET_X	20

struct RewardInterface
{
	RewardInterface();
	~RewardInterface();

	Ship* m_ship;
	Reward* m_reward;
	sf::Text m_text;
	sf::Text m_text_DMS_location;

	GameEntity* m_panel;
	vector<GameEntity*> m_resources_displayed;

	void Init(Ship* ship, Reward* reward);
	void Destroy();

	bool Update();
	void Draw(sf::RenderTexture& screen);

	GameEntity* m_ok_button;
};

#endif //REWARDINTERFACE_H_INCLUDED