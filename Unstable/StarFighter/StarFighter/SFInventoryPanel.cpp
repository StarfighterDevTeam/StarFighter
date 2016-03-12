#include "SFInventoryPanel.h"

extern Game* CurrentGame;

SFInventoryPanel::SFInventoryPanel(sf::Vector2f size, Ship* playerShip) : SFPanel(size, SFPanel_Inventory)
{
	m_disable_cursor_move = false;
	m_focused_item = NULL;
	m_focused_cell_index = sf::Vector2i(-1, -1);
	m_use_two_grids = false;
	m_playerShip = playerShip;
}

void SFInventoryPanel::Update(sf::Time deltaTime, sf::Vector2f inputs_directions)
{
	if (m_cursor.m_visible && !m_disable_cursor_move)
	{
		sf::Vector2f panel_size = getSize();
		sf::Vector2f panel_pos = getPosition();
		sf::Vector2f cursor_pos = m_cursor.getPosition();

		//move cursor
		m_cursor.m_speed.x = inputs_directions.x * HUD_CURSOR_SPEED;
		m_cursor.m_speed.y = inputs_directions.y * HUD_CURSOR_SPEED;

		//panel constraints
		if (m_cursor.getPosition().x < panel_pos.x - (panel_size.x / 2))
			m_cursor.setPosition(panel_pos.x - (panel_size.x / 2), cursor_pos.y);
		if (m_cursor.getPosition().x > panel_pos.x + (panel_size.x / 2))
			m_cursor.setPosition(panel_pos.x + (panel_size.x / 2), cursor_pos.y);
		if (m_cursor.getPosition().y < panel_pos.y - (panel_size.y / 2))
			m_cursor.setPosition(cursor_pos.x, panel_pos.y - (panel_size.y / 2));
		if (m_cursor.getPosition().y > panel_pos.y + (panel_size.y / 2))
			m_cursor.setPosition(cursor_pos.x, panel_pos.y + (panel_size.y / 2));

		//update hovered item and highlight feedbacks
		GetHoveredObjectInGrid(m_grid, m_fake_grid);
		if (m_use_two_grids)
		{
			GetHoveredObjectInGrid(m_grid2, m_fake_grid2);
		}
	}
}

void SFInventoryPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);

		m_fake_grid.Draw(screen);
		m_grid.Draw(screen);
		if (m_use_two_grids)
		{
			m_fake_grid2.Draw(screen);
			m_grid2.Draw(screen);
		}
	}
}

GameObject* SFInventoryPanel::GetHoveredObjectInGrid(ObjectGrid grid, ObjectGrid fake_grid)
{
	//reset previous highlight
	if (m_focused_cell_index.x >= 0 && m_focused_cell_index.y >= 0)
	{
		fake_grid.grid[m_focused_cell_index.x][m_focused_cell_index.y]->setAnimationLine(Slot_NormalState);
	}

	//getting hovered cell's index
	int hovered_index = fake_grid.isCursorColliding(m_cursor);
	if (hovered_index < 0)//case: no cell is hovered
	{
		m_focused_cell_index.x = -1;
		m_focused_cell_index.y = -1;
		m_focused_item = NULL;
		return m_focused_item;
	}
	else//case: cell is hovered
	{
		//update knowledge of focused index
		m_focused_cell_index.x = hovered_index % fake_grid.squares.x;
		m_focused_cell_index.y = hovered_index % fake_grid.squares.y;

		//highlight new focused cell
		fake_grid.SetCellHighlightState(hovered_index, Slot_HighlightState);

		//update focused item
		m_focused_item = grid.grid[m_focused_cell_index.x][m_focused_cell_index.y];

		return m_focused_item;
	}
}