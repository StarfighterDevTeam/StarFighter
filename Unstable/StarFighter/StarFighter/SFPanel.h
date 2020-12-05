#ifndef SFPANEL_H_INCLUDED
#define SFPANEL_H_INCLUDED

#include "SFTextPop.h"

class Ship;
class Dialog;
class SFItemStatsPanel;

enum TradeGrids
{
	Trade_EquippedGrid,
	Trade_StashGrid,
	Trade_ShopGrid,
	NBVAL_TradeGrids//3
};

enum ActionButtons
{
	ActionButton_A,
	ActionButton_X,
	ActionButton_Y,
	ActionButton_B,
	NBVAL_ActionButtons,//4
};

enum FocusedItemStates
{
	FocusedItem_Equip,
	FocusedItem_Desequip,
	FocusedItem_Buy,
	FocusedItem_SellOrEquip,
	FocusedItem_SellOrDesequip,
};

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

class SFPanel : public SFRectangle
{
public:
	SFPanel(){};
	SFPanel(sf::Vector2f size, SFPanelTypes panel_type);
	~SFPanel();
	virtual void Update(sf::Time deltaTime, sf::Vector2f inputs_directions);
	virtual void Draw(sf::RenderTexture& screen);
	static bool IsCursorCollidingWithRectangle(GameObject& cursor, RectangleShape& object);
	
	sf::Text m_title_text;
	sf::Text m_text;
	bool m_visible;
	SFPanelTypes m_panel_type;

	SFActionBox* m_actions;

	Ship* m_playerShip;

	virtual void SetSelectedOptionIndex(int index);
	virtual int GetSelectedOptionIndex();
	virtual GameObject* GetCursor();
	virtual GameObject* GetFocusedItem();
	virtual void SetFocusedItem(GameObject* item);

	virtual Grid* GetGrid_v2(int grid);
	virtual sf::Vector2i GetFocusedIndex();
	virtual int GetFocusedIntIndex();
	virtual int GetFocusedGrid();
	virtual bool GetPrioritaryFeedback();
	virtual void SetPrioritaryFeedback(bool has_priotiary_feedback);
	virtual string GetTeleportationDestination();
	virtual int GetTeleportationCost();
	virtual void SetMapViewOffset(sf::Vector2f offset);
	virtual SFItemStatsPanel* GetItemStatsPanel();
	virtual void SetItemStatsPanel(SFItemStatsPanel* panel);
	virtual float GetDuration();
	virtual float GetDurationTimer();
	virtual int GetItemsStatsPanelIndex();
	virtual void SetItemsStatsPanelIndex(int index);
	
	virtual Dialog* GetDialog();

	//v2
	virtual GridElement* GetHighlightedElement();
	virtual void SetHighlightedElement(GridElement* element);
	virtual void SetCursorVisible_v2(bool visible);
};

#endif // SFPANEL_H_INCLUDED