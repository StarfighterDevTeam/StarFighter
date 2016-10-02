#ifndef SFPANEL_H_INCLUDED
#define SFPANEL_H_INCLUDED

#include "SimpleCollision.hpp"
#include "Enums.h"

class SEShip;
class Dialog;
class SFItemStatsPanel;

class SFActionBox : public sf::RectangleShape
{
public:
	SFActionBox(sf::Font* font);
	void SetString(string button_name, ActionButtons button);
	void SetPosition(sf::Vector2f position);
	void Draw(sf::RenderTexture& screen);

	GameObject m_boxes[NBVAL_ActionButtons];
	sf::Text m_texts[NBVAL_ActionButtons];
};

class SFActionBoxWithSelection : public sf::RectangleShape
{
public:
	SFActionBoxWithSelection();
	~SFActionBoxWithSelection(){};
	void AddOption(string option_name, sf::Font* font);
	void SetPosition(sf::Vector2f position);
	void SetSelectedAction(int index);
	void Draw(sf::RenderTexture& screen);

	GameObject m_box;
	vector<sf::Text> m_texts;
	int m_selected_index;
};

class SFPanel : public sf::RectangleShape
{
public:
	SFPanel(){};
	SFPanel(sf::Vector2f size, SFPanelTypes panel_type);
	virtual ~SFPanel();
	virtual void Update(sf::Time deltaTime);
	virtual void Draw(sf::RenderTexture& screen);
	static bool IsCursorCollidingWithRectangle(GameObject& cursor, RectangleShape& object);
	
	sf::Text m_title_text;
	sf::Text m_text;
	bool m_visible;
	SFPanelTypes m_panel_type;
	SEShip* m_playerShip;
	SFActionBox* m_actions;
};

#endif // SFPANEL_H_INCLUDED