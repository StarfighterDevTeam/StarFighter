#ifndef HUDGRID_H_INCLUDED
#define HUDGRID_H_INCLUDED

#include "Includes/SimpleCollision.hpp"

enum SlotFeedbackStates
{
	Slot_NormalState,
	Slot_HighlightState,
	Slot_GreyState,
	Slot_EngineFake,
	Slot_ArmorFake,
	Slot_ShieldFake,
	Slot_ModuleFake,
	Slot_WeaponFake,
	Slot_NBVALFeedbackStates,
};

enum CursorFeedbackStates
{
	Cursor_NormalState,
	Cursor_HighlightState,
	Cursor_ActionState,
	Cursor_Focus1_8,
	Cursor_Focus2_8,
	Cursor_Focus3_8,
	Cursor_Focus4_8,
	Cursor_Focus5_8,
	Cursor_Focus6_8,
	Cursor_Focus7_8,
	Cursor_Focus8_8,
};

class ObjectGrid
{
public:
	ObjectGrid();
	ObjectGrid(sf::Vector2f position, sf::Vector2i squares, bool fill_with_fake = false, bool fill_with_grey = false, bool fill_with_colored_background = false);
	void SetGridPosition(sf::Vector2f position);
	bool insertObject(GameObject& object, int index = -1, bool overwrite_existing = false);
	GameObject* grid[EQUIPMENT_GRID_NB_LINES][EQUIPMENT_GRID_NB_ROWS];
	void Draw(sf::RenderTexture& offscreen);
	void Draw(sf::RenderTexture& offscreen, sf::Color background_color);
	sf::Vector2f position;
	sf::Vector2i squares;
	sf::Vector2i focus;
	int isCursorColliding(GameObject& cursor);
	bool HighlightCell(int index);
	bool SetCellHighlightState(int index, SlotFeedbackStates highlight_state);
	bool GarbageCell(int index);
	bool CleanFocus();
	GameObject* getCellPointerFromIntIndex(int index);
	int getFocusIntIndex();
	int GetIntIndex(sf::Vector2i index);
	void setCellPointerForIntIndex(int index, GameObject* GameObject);
	void ClearGrid();
	void ClearHighlight();
	static bool SwapObjectsBetweenGrids(ObjectGrid& grid1, ObjectGrid& grid2, int index1, int index2);

	bool m_visible;
};

class Grid;//forward declaration
class GridElement : public GameObject
{
public:
	GridElement();
	~GridElement();
	void Draw(sf::RenderTexture& offscreen);
	void SetPosition(sf::Vector2f position);
	void SetObject(GameObject* object, EquipmentQuality quality);

	//int m_credits;
	//int m_level;
	//float m_quality;
	GameObject* m_object;
	GameObject* m_quality_overlay;
	GameObject* m_grey_overlay;
	
	int m_index;
	Grid* m_grid;
};

class SFPanel;//forward declaration
class Grid
{
public:
	Grid(sf::Vector2f position, sf::Vector2i nb_squares, SFPanel* panel);
	~Grid();
	void Draw(sf::RenderTexture& offscreen);
	void SetPosition(sf::Vector2f position);
	GridElement* GetCursorCollision(GameObject& cursor);
	int InsertObject(GameObject* object, int index, EquipmentQuality quality, bool force_overwrite);
	void CloneGridContentFrom(Grid* grid);

	sf::Vector2f m_position;
	sf::Vector2f m_size;
	sf::Vector2i m_nb_squares;
	int m_first_empty_slot_index;
	vector<GridElement*> m_elements;
	SFPanel* m_panel;
};

#endif // HUDGRID_H_INCLUDED