#ifndef REWARD_H_INCLUDED
#define REWARD_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Commodity.h"

struct DMS_Coord;

enum RewardData
{
	Reward_ID,
	Reward_Gold,
	Reward_Fish,
	Reward_Mech,
	Reward_Days,
	Reward_Fidelity,
	Reward_Crew,
	Reward_SecretWreck,
	Reward_Commodity,
	Reward_Combat,
	Reward_Text,
	NB_REWARD_DATA,
};

struct Reward
{
public:
	Reward();
	~Reward();
	vector<pair<ResourceType, int> > m_resources;
	string m_string;
	DMS_Coord* m_DMS_location;
	Ship* m_combat_ship;
	CommodityType m_commodity;
};

#endif //REWARD_H_INCLUDED