#ifndef SFMENU_H_INCLUDED
#define SFMENU_H_INCLUDED

#include "Scene.h"

class SFMenuPanel : public SFPanel
{
public:
	SFMenuPanel(sf::Vector2f size, SFPanelTypes panel_type, size_t options, Ship* playerShip);
	~SFMenuPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;

	void SFMenuPanel::SetSelectedOptionIndex(int index) override;
	int GetSelectedOptionIndex() override;

	size_t m_options;
	sf::Text* m_options_text;
	GameObject m_arrow;
	int m_selected_option_index;
};

class SFOneActionPanel : public SFMenuPanel
{
public:
	SFOneActionPanel(sf::Vector2f size, Ship* playerShip);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;

	Directions m_direction;
};

class SFPortalPanel : public SFMenuPanel
{
public:
	SFPortalPanel(sf::Vector2f size, Ship* playerShip);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;

	Directions m_direction;
};

class SFShopPanel : public SFMenuPanel
{
public:
	SFShopPanel(sf::Vector2f size, Ship* playerShip);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
};

#endif // SFMENU_H_INCLUDED