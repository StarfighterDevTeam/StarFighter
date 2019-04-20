#ifndef CHOICE_H_INCLUDED
#define CHOICE_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

#define CHOICE_PANEL_SIZE_X			600
#define CHOICE_PANEL_SIZE_Y			150
#define CHOICE_VIDEO_SIZE_X			280

#define CHOICE_GAUGE_SIZE_X			240
#define CHOICE_GAUGE_SIZE_Y			10

#define RESOURCES_INTERFACE_PANEL_SIZE_Y			40
#define RESOURCES_INTERFACE_OFFSET_X				30
#define RESOURCES_INTERFACE_STOCK_SIZE_X					150
#define RESOURCES_INTERFACE_STOCK_SIZE_Y					30
#define RESOURCES_CREWFIDELITY_SIZE_X				250
#define RESOURCES_CREWFIDELITY_SIZE_Y				12

#define RESOURCES_BUTTON_SIZE_X						100
#define RESOURCES_BUTTON_SIZE_Y						24

#define RESOURCES_ICON_SIZE							30
#define CHOICES_REWARDS_OFFSET_X						120
#define CHOICES_COSTS_OFFSET_X						80

struct DMS_Coord;

enum ChoicesData
{
	Choice_ID,
	Choice_Text,
	Choice_Picturename,
	Choice_CostGold,
	Choice_CostFish,
	Choice_CostMech,
	Choice_CostFidelity,
	Choice_CostDays,
	Choice_Skill,
	Choice_ValueMax,
	Choice_Reward1,
	Choice_Reward1_Proba,
	Choice_Reward2,
	Choice_Reward2_Proba,
	Choice_Reward3,
	Choice_Reward3_Proba,
	//Choice_RewardGold,
	//Choice_RewardFish,
	//Choice_RewardMech,
	//Choice_RewardDays,
	//Choice_RewardFidelity,
	//Choice_RewardCrew,
	//Choice_RewardText,
	NB_CHOICE_DATA,
};

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
	Reward_Text,
	NB_REWARD_DATA,
};

#define NB_CHOICE_REWARDS_MAX				((NB_CHOICE_DATA - Choice_Reward1) / 2)

struct Reward
{
public:
	Reward();
	~Reward();
	vector<pair<ResourceType, int> > m_resources;
	string m_string;
	DMS_Coord* m_DMS_location;
};

struct Choice
{
public:
	Choice();
	~Choice();

	void Init(int index, int choiceID, string text = "", string portrait_filename = "");
	void Destroy();

	bool Update(int gauge_value);
	void Draw(sf::RenderTexture& screen);
	void SetPosition(sf::Vector2f position);
	int RandomizeRewardID();

	sf::Vector2f m_position;

	GameEntity* m_panel;
	GameEntity* m_picture;
	sf::Text m_title;
	sf::Text m_body;

	GameEntity* m_gauge;
	string m_gauge_string;
	int m_gauge_value_max;
	int m_gauge_value;
	int m_skill;
	int m_cost[NB_RESOURCES_TYPES];

	vector<GameEntity*> m_costs_displayed;
	pair<int, int> m_rewardsID[NB_CHOICE_REWARDS_MAX];
	//string m_reward_string;
	//int m_reward_resources[NB_RESOURCES_TYPES];
	//int m_reward_crew;
};

#endif //CHOICE_H_INCLUDED