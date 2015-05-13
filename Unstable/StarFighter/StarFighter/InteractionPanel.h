#ifndef INTERACTIONPANEL_H_INCLUDED
#define INTERACTIONPANEL_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Independant.h"
#include "HudGrid.h"

enum ShopOptions
{
	ShopHeal,
	ShopBuy,
	StellarMap,
	NBVAL_ShopOptions,
};

enum ShopMenus
{
	ShopMainMenu,
	ShopBuyMenu,
	NBVAL_ShopMenus,
};

struct InteractionPanel
{
public:
	InteractionPanel();
	void SetPanelPosition(sf::Vector2f position);
	sf::Vector2f GetShopBuyPanelSize();

	void UpdatePortalInteraction(int max_unlocked_hazard_level);
	void UpdateShopInteraction(sf::Time deltaTime);
	void Update(InteractionType interaction, int max_unlocked_hazard_level, sf::Time deltaTime);
	void InitCursorOnGrid();
	void Draw(sf::RenderTexture& screen);

	sf::RectangleShape m_panel;
	sf::Text m_text[NB_HAZARD_LEVELS];
	sf::Text m_textHelpNavigation;
	sf::Text m_textHelpBuy;
	sf::Text m_textDestination;
	sf::Text m_textShopOptions[NBVAL_ShopOptions];
	sf::Font* m_font;
	Independant* m_arrow;
	int m_selected_index;
	Directions m_direction;
	InteractionType m_currentInteractionType;
	ShopMenus m_currentShopMenu;

	ObjectGrid m_fakeShopGrid;
	ObjectGrid m_shopGrid;
	Independant* m_cursor;
	Independant* m_focused_item;
	sf::Vector2i m_focused_index;
};

#endif //INTERACTIONPANEL_H_INCLUDED