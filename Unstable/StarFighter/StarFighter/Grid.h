#ifndef GRID_H_INCLUDED
#define GRID_H_INCLUDED

#include "Equipment.h"

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

class Grid;//forward declaration
class GridElement : public GameObject
{
public:
	GridElement();
	~GridElement();
	void Draw(sf::RenderTexture& offscreen);
	void SetPosition(sf::Vector2f position);
	void SetObject(GameObject* object, bool delete_current_object = false);

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
	int InsertObject(GameObject* object, int index, bool force_overwrite);
	void CloneGridContentFrom(Grid* grid);
	void ClearGrid();
	void UpdateGreyMaskOnInsufficientCredits(int money);//buggé
	static int GetPrice(GameObject* capsule);
	static int GetEquipmentType(GameObject* capsule);

	sf::Vector2f m_position;
	sf::Vector2f m_size;
	sf::Vector2i m_nb_squares;
	int m_first_empty_slot_index;
	vector<GridElement*> m_elements;
	SFPanel* m_panel;
};

#endif // GRID_H_INCLUDED