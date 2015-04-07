#ifndef HUD_H_INCLUDED
#define HUD_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Globals.h"
#include "Independant.h"
#include "Includes/SimpleCollision.hpp"

#define ARMOR_BAR_SIZE_X		20
#define ARMOR_BAR_SIZE_Y		200

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
};

class ObjectGrid
{
public:
	ObjectGrid();
	ObjectGrid(sf::Vector2f position, sf::Vector2i squares, bool fill_with_fake = false);
	bool insertObject(Independant& object, int index = -1);
	Independant* grid[EQUIPMENT_GRID_NB_LINES][EQUIPMENT_GRID_NB_ROWS];
	void Draw(sf::RenderTexture& offscreen);
	sf::Vector2f position;
	sf::Vector2i squares;
	sf::Vector2i focus;
	int isCursorColling(Independant& cursor);
	bool HighlightCell(int index);
	bool CleanFocus();
	Independant* getCellPointerFromIntIndex(int index);
};

class PlayerHud
{
public:
	PlayerHud();
	void Init(int m_armor, int m_shield);
	void Update(int m_armor, int m_shield, int m_money, int m_graze_count, int m_hazard_level, std::string scene_name, sf::Time deltaTime, bool hub);
	void Draw(sf::RenderTexture& offscreen);

	ObjectGrid fakeEquipmentGrid;
	ObjectGrid equipmentGrid;
	ObjectGrid fakeShipGrid;
	ObjectGrid shipGrid;

	sf::RectangleShape backgroundColor;
	sf::RectangleShape armorBar;
	sf::RectangleShape shieldBar;
	//sf::Text ShipGridTitle;
	//sf::Text EquipmentGridTitle;
	sf::Text Money;
	sf::Text GrazeScore;
	sf::Text SceneName;
	sf::Font* font;
	sf::Font* font2;
	sf::Text* framerate;

	Independant* hud_cursor;

	bool max_hazard_level_reached;
	bool has_shield;
};

#endif // HUD_H_INCLUDED