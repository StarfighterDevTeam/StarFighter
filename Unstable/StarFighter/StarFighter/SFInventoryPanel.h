#ifndef SFINVENTORYPANEL_H_INCLUDED
#define SFINVENTORYPANEL_H_INCLUDED

#include "Scene.h"

class SFInventoryPanel : public SFPanel
{
public:
	SFInventoryPanel(sf::Vector2f size, Ship* playerShip);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	void DisplayItemStats();
	SFInventoryPanel* Clone();

	GameObject* GetHoveredObjectInGrid(ObjectGrid grid, ObjectGrid fake_grid);

	GameObject m_cursor;
	GameObject* m_focused_item;
	sf::Vector2i m_focused_cell_index;
	ObjectGrid m_grid;
	ObjectGrid m_fake_grid;
	ObjectGrid m_grid2;
	ObjectGrid m_fake_grid2;
	bool m_disable_cursor_move;
	bool m_use_two_grids;

	Ship* m_playerShip;
};

#endif // SFINVENTORYPANEL_H_INCLUDED