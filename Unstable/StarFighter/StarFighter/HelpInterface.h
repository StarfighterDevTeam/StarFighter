#ifndef HELP_H_INCLUDED
#define HELP_H_INCLUDED

#include "Game.h"
#include "GameEntity.h"
#include "Warship.h"

struct HelpInterface
{
	HelpInterface();
	~HelpInterface();

	GameEntity* m_panel;
	sf::Text m_text_title;
	sf::Text m_text_body;

	void Init(string& text_title, string& text_body, sf::Vector2f upper_left_anchor_position);
	void Destroy();
	//void Update();
	void Draw(sf::RenderTexture& screen);
};

#endif //HELP_H_INCLUDED