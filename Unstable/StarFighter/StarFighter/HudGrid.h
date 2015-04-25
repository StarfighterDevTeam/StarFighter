#ifndef HUDGRID_H_INCLUDED
#define HUDGRID_H_INCLUDED

#include "Independant.h"
#include <list>

enum HudGridsIndex
{
	HudGrid_NoFocus,
	HudGrid_ShipGrid,
	HudGrid_EquipmentGrid,
};

enum SlotFeedbackStates
{
	Slot_NormalState,
	Slot_HighlightState,
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
	ObjectGrid(sf::Vector2f position, sf::Vector2i squares, bool fill_with_fake = false);
	bool insertObject(Independant& object, int index = -1, bool overwrite_existing = false);
	Independant* grid[EQUIPMENT_GRID_NB_LINES][EQUIPMENT_GRID_NB_ROWS];
	void Draw(sf::RenderTexture& offscreen);
	sf::Vector2f position;
	sf::Vector2i squares;
	sf::Vector2i focus;
	int isCursorColling(Independant& cursor);
	bool HighlightCell(int index);
	bool GarbageCell(int index);
	bool CleanFocus();
	Independant* getCellPointerFromIntIndex(int index);
	int getFocusIntIndex();
	void ObjectGrid::setCellPointerForIntIndex(int index, Independant* independant);
	list<Independant*>* ObjectGrid::GetAllEquipments();
};

#endif // HUDGRID_H_INCLUDED