#ifndef CHOICE_H_INCLUDED
#define CHOICE_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

#define CHOICE_PANEL_SIZE_X			600
#define CHOICE_PANEL_SIZE_Y			150
#define CHOICE_VIDEO_SIZE_X			280

#define CHOICE_GAUGE_SIZE_X			240
#define CHOICE_GAUGE_SIZE_Y			10

#define RESOURCES_INTERFACE_PANEL_SIZE_Y		40
#define RESOURCES_INTERFACE_OFFSET_X			30
#define RESOURCES_INTERFACE_SIZE_X				110
#define RESOURCES_INTERFACE_SIZE_Y				30
#define RESOURCES_CREWFIDELITY_SIZE_X			250
#define RESOURCES_CREWFIDELITY_SIZE_Y			12

#define RESOURCES_BUTTON_SIZE_X					100
#define RESOURCES_BUTTON_SIZE_Y					24

#define RESOURCES_ICON_SIZE						30
#define CHOICES_REWARDS_OFFSET_X				80

enum ChoicesData
{
	Choice_ID,
	Choice_Text,
	Choice_Picturename,
	Choice_Skill,
	Choice_ValueMax,
	Choice_RewardGold,
	Choice_RewardFish,
	Choice_RewardMech,
	Choice_RewardDays,
	Choice_RewardFidelity,
	Choice_RewardCrew,
	Choice_RewardText,
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

	GameEntity* m_rewards[3];
	string m_reward_string;
	int m_reward_resources[NB_RESOURCES_TYPES];
	int m_reward_crew;
};

#endif //CHOICE_H_INCLUDED