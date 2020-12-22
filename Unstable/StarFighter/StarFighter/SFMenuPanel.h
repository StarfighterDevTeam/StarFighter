#ifndef SFMENU_H_INCLUDED
#define SFMENU_H_INCLUDED

#include "Scene.h"

class SFMenuPanel : public SFPanel
{
public:
	SFMenuPanel(sf::Vector2f size, SFPanelTypes panel_type, size_t options, Ship* playership);
	~SFMenuPanel();
	virtual void Update(sf::Time deltaTime, sf::Vector2f inputs_directions);
	virtual void Draw(sf::RenderTexture& screen);

	size_t m_options;
	SFActionBoxWithSelection* m_actions_with_selection;
};

class SFOneActionPanel : public SFMenuPanel
{
public:
	SFOneActionPanel(sf::Vector2f size, Ship* playership);
	~SFOneActionPanel(){};

	Directions m_direction;
};

class SFPortalPanel : public SFMenuPanel
{
public:
	SFPortalPanel(sf::Vector2f size, Ship* playership);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	void SetSelectedOptionIndex(int index) override;
	int GetSelectedOptionIndex() override;

	int m_selected_option_index;
	Directions m_direction;
};

class SFShopPanel : public SFMenuPanel
{
public:
	SFShopPanel(sf::Vector2f size, Ship* playership);
};

#endif // SFMENU_H_INCLUDED