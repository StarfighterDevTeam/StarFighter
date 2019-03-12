#ifndef CHOICE_H_INCLUDED
#define CHOICE_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"

#define CHOICE_PANEL_SIZE_X			600
#define CHOICE_PANEL_SIZE_Y			150
#define CHOICE_VIDEO_SIZE_X			280

#define CHOICE_GAUGE_SIZE_X			240
#define CHOICE_GAUGE_SIZE_Y			10

struct Choice
{
	Choice();
	~Choice();

	void Init(int index, string text, string portrait_filename);
	void Destroy();

	bool Update();
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
};

#endif //CHOICE_H_INCLUDED