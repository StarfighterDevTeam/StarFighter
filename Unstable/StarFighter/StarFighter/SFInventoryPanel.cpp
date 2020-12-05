#include "SFInventoryPanel.h"

extern Game* CurrentGame;

//INVENTORY PANEL
SFInventoryPanel::SFInventoryPanel(sf::Vector2f size, Ship* playerShip, SFPanelTypes panel_type) : SFPanel(size, panel_type)
{
	m_playerShip = playerShip;
	m_item_stats_panel = NULL;
	m_item_stats_panel_compare = NULL;
	m_has_prioritary_feedback = false;

	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_cursor = GameObject(sf::Vector2f(INTERACTION_PANEL_MARGIN_SIDES + (EQUIPMENT_GRID_SLOT_SIZE / 2), SHIP_GRID_OFFSET_POS_Y + (EQUIPMENT_GRID_SLOT_SIZE / 2)),
		sf::Vector2f(0, 0), HUD_CURSOR_TEXTURE_NAME, sf::Vector2f(HUD_CURSOR_WIDTH, HUD_CURSOR_HEIGHT), sf::Vector2f(HUD_CURSOR_WIDTH / 2, HUD_CURSOR_HEIGHT / 2), 1, (Cursor_Focus8_8 + 1));

	m_cursor.m_visible = false;

	//size and position
	setSize(size);
	setOrigin(size.x / 2, size.y / 2);
	setPosition(sf::Vector2f(SCENE_SIZE_X / 2, SCENE_SIZE_Y / 2));

	//v2
	for (int i = 0; i < NBVAL_TradeGrids; i++)
		m_grids_v2[i] = NULL;

	m_highlighted_element = NULL;
}

SFInventoryPanel::~SFInventoryPanel()
{
	if (m_item_stats_panel)
	{
		delete m_item_stats_panel;
		m_item_stats_panel = NULL;
	}
	if (m_item_stats_panel_compare)
	{
		delete m_item_stats_panel_compare;
		m_item_stats_panel_compare = NULL;
	}

	//v2
	if (m_panel_type != SFPanel_Trade)//trade panel grids will be destroyed in ~Shop destructor
		for (int i = 0; i < NBVAL_TradeGrids; i++)
			delete m_grids_v2[i];
}

void SFInventoryPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	getCursorCollision(m_cursor);
}

void SFInventoryPanel::DrawItemLevels(sf::RenderTexture& screen)
{
	for (int i = 0; i < NBVAL_TradeGrids; i++)
		if (m_grids_v2[i] != NULL)
			for (GridElement* element : m_grids_v2[i]->m_elements)
				if (element->m_object != NULL)
				{
					sf::Text item_level;
					item_level.setCharacterSize(11);

					if (m_cursor.m_visible == true)
						item_level.setColor(sf::Color(255, 255, 255, 255));
					else
						item_level.setColor(sf::Color(255, 255, 255, GHOST_ALPHA_VALUE / 2));

					int level = element->m_object->m_equipment_loot != NULL ? element->m_object->m_equipment_loot->m_level : element->m_object->m_weapon_loot->m_level;
					
					item_level.setFont(*(*CurrentGame).m_font[Font_Terminator]);
					ostringstream ss;
					ss << level;
					item_level.setString(ss.str());
					item_level.setPosition(sf::Vector2f(element->getPosition().x + GRID_SLOT_SIZE * 0.5 - item_level.getGlobalBounds().width - 4, element->getPosition().y + GRID_SLOT_SIZE * 0.5 - item_level.getCharacterSize() - 4));
					screen.draw(item_level);
				}
}

void SFInventoryPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		for (int i = 0; i < NBVAL_TradeGrids; i++)
			if (m_grids_v2[i] != NULL)
					m_grids_v2[i]->Draw(screen);

		DrawItemLevels(screen);

		screen.draw(m_title_text);
		if (m_cursor.m_visible)
		{
			screen.draw(m_cursor);
		}
	
		if (m_actions)
		{
			m_actions->Draw(screen);
		}
	}
}


bool SFInventoryPanel::GetPrioritaryFeedback()
{
	return m_has_prioritary_feedback;
}

void SFInventoryPanel::SetItemStatsPanel(SFItemStatsPanel* panel)
{
	m_item_stats_panel = panel;
}

void SFInventoryPanel::SetPrioritaryFeedback(bool has_priotiary_feedback)
{
	m_has_prioritary_feedback = has_priotiary_feedback;
}


int SFInventoryPanel::GetItemsStatsPanelIndex()
{
	if (m_item_stats_panel)
	{
		return m_item_stats_panel->m_selected_option_index;
	}
	else
	{
		return -1;
	}
}

void SFInventoryPanel::SetItemsStatsPanelIndex(int index)
{
	if (m_item_stats_panel)
	{
		m_item_stats_panel->m_selected_option_index = index;
	}
}

void SFInventoryPanel::SetCursorVisible_v2(bool visible)
{
	//default reposition
	if (m_cursor.m_visible == false && visible == true)
	{
		TradeGrids grid_type = m_panel_type == SFPanel_HUD ? Trade_EquippedGrid : Trade_ShopGrid;
		bool found = false;
		for (GridElement* element : m_grids_v2[grid_type]->m_elements)
		{
			if (element->m_object != NULL)
			{
				m_cursor.setPosition(element->getPosition());
				found = true;
				break;
			}
		}

		if (found == false && grid_type == Trade_EquippedGrid)
		{
			for (GridElement* element : m_grids_v2[Trade_StashGrid]->m_elements)
			{
				if (element->m_object != NULL)
				{
					m_cursor.setPosition(element->getPosition());
					found = true;
					break;
				}
			}
		}

		if (found == false)
			m_cursor.setPosition(m_grids_v2[grid_type]->m_elements[0]->getPosition());
	}

	//switch visibility
	m_cursor.m_visible = visible;

	//update grey overlay
	if (m_panel_type == SFPanel_HUD)
	{
		for (int i = 0; i < NBVAL_TradeGrids; i++)
			if (m_grids_v2[i] != NULL)
				for (GridElement* element : m_grids_v2[i]->m_elements)
					element->m_grey_overlay->m_visible = (m_cursor.m_visible == false);
	}
}

void SFInventoryPanel::CreateGrids(SFPanel* panel, sf::Vector2f position0, sf::Vector2f position1, sf::Vector2f position2)
{
	//Equipped grid
	m_grids_v2[Trade_EquippedGrid] = new Grid(position0, sf::Vector2i(NBVAL_Equipment + 1, 1), panel);

	for (GridElement* element : m_grids_v2[Trade_EquippedGrid]->m_elements)
		element->setAnimationLine(Slot_EngineFake + element->m_index);

	//Stash grid
	m_grids_v2[Trade_StashGrid] = new Grid(position1, sf::Vector2i(NBVAL_Equipment + 1, EQUIPMENT_GRID_NB_LINES), panel);

	//Shop grid
	if (m_panel_type == SFPanel_Trade && m_grids_v2[Trade_ShopGrid] == NULL)//check if not already created
	{
		m_grids_v2[Trade_EquippedGrid]->CloneGridContentFrom(m_playerShip->m_SFHudPanel->GetGrid_v2((int)Trade_EquippedGrid));

		m_grids_v2[Trade_StashGrid]->CloneGridContentFrom(m_playerShip->m_SFHudPanel->GetGrid_v2((int)Trade_StashGrid));
		
		m_grids_v2[Trade_ShopGrid] = m_playerShip->m_targetShop->m_grid_v2;
		m_grids_v2[Trade_ShopGrid]->SetPosition(position2);
		m_grids_v2[Trade_ShopGrid]->m_panel = this;
	}

	//Cursor
	SetCursorVisible_v2(m_panel_type != SFPanel_HUD);
}

GridElement* SFInventoryPanel::GetHighlightedElement()
{
	return m_highlighted_element;
}

void SFInventoryPanel::SetHighlightedElement(GridElement* element)
{
	m_highlighted_element = element;
}

Grid* SFInventoryPanel::GetGrid_v2(int grid)
{
	return m_grids_v2[grid];
}

GameObject* SFInventoryPanel::GetCursor()
{
	return &m_cursor;
}

GridElement* SFInventoryPanel::getCursorCollision(GameObject& cursor)
{
	//get element under cursor
	GridElement* element = NULL;
	for (int i = 0; i < NBVAL_TradeGrids; i++)
	{
		if (m_grids_v2[i] != NULL)
		{
			element = m_grids_v2[i]->GetCursorCollision(cursor);
			if (element != NULL)
				break;
		}
	}

	//update cursor state
	if (m_has_prioritary_feedback == false)
	{
		if (element == NULL)
			cursor.setAnimationLine(Cursor_NormalState);
		else if (element->m_object == NULL)
			cursor.setAnimationLine(Cursor_HighlightState);
		else
			cursor.setAnimationLine(Cursor_ActionState);
	}

	//V3
	if (element == NULL || element != m_highlighted_element)
	{
		if (m_highlighted_element != NULL)//reset previous highlight
			m_highlighted_element->setAnimationLine(m_highlighted_element->m_grid != m_grids_v2[Trade_EquippedGrid] ? Slot_NormalState : Slot_EngineFake + m_highlighted_element->m_index);

		m_highlighted_element = element;//assign new highlight

		delete m_item_stats_panel;
		delete m_item_stats_panel_compare;
		m_item_stats_panel = NULL;
		m_item_stats_panel_compare = NULL;

		if (element != NULL)//feedback new element highlight and item stats
		{
			element->setAnimationLine(Slot_HighlightState);

			FocusedItemStates item_state;
			if (m_panel_type == SFPanel_HUD)
			{
				if (element->m_grid == m_grids_v2[Trade_EquippedGrid])
					item_state = FocusedItem_Desequip;
				else
					item_state = FocusedItem_Equip;
			}
			else if (m_panel_type == SFPanel_Trade)
			{
				if (element->m_grid == m_grids_v2[Trade_EquippedGrid])
					item_state = FocusedItem_SellOrDesequip;
				else if (element->m_grid == m_grids_v2[Trade_StashGrid])
					item_state = FocusedItem_SellOrEquip;
				else
					item_state = FocusedItem_Buy;
			}

			if (element->m_object != NULL)//display item stats if any
			{
				m_item_stats_panel = new SFItemStatsPanel(element->m_object, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, item_state, NULL);

				//update compare panel
				int equip_type = Grid::GetEquipmentType(element->m_object);
				if (element->m_grid != m_grids_v2[Trade_EquippedGrid] && m_grids_v2[Trade_EquippedGrid]->m_elements[equip_type]->m_object != NULL)//compare only if it's not the equipped grid
					m_item_stats_panel_compare = new SFItemStatsPanel(element->m_object, sf::Vector2f(ITEM_STATS_PANEL_SIZE_X, ITEM_STATS_PANEL_SIZE_Y), m_playerShip, item_state, m_grids_v2[Trade_EquippedGrid]->m_elements[equip_type]->m_object);
			}
		}
	}
	else if (element->m_object == NULL && m_item_stats_panel != NULL)
	{
		delete m_item_stats_panel;
		delete m_item_stats_panel_compare;
		m_item_stats_panel = NULL;
		m_item_stats_panel_compare = NULL;
	}
	
	return element;
}