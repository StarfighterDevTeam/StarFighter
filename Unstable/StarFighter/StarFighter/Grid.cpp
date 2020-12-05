#include "Grid.h"

//GRID ELEMENT
GridElement::GridElement() : GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), EMPTYSLOT_FILENAME, sf::Vector2f(GRID_SLOT_SIZE, GRID_SLOT_SIZE), sf::Vector2f(GRID_SLOT_SIZE * 0.5, GRID_SLOT_SIZE * 0.5),
	1, Slot_NBVALFeedbackStates)
{
	//quality overlay
	m_quality_overlay = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), QUALITY_BACKGROUND, sf::Vector2f(GRID_SLOT_SIZE, GRID_SLOT_SIZE), sf::Vector2f(GRID_SLOT_SIZE * 0.5, GRID_SLOT_SIZE * 0.5),
		1, NBVAL_ItemQuality);
	m_quality_overlay->m_visible = false;

	//grey overlay
	m_grey_overlay = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), EMPTYSLOT_FILENAME, sf::Vector2f(GRID_SLOT_SIZE, GRID_SLOT_SIZE), sf::Vector2f(GRID_SLOT_SIZE * 0.5, GRID_SLOT_SIZE * 0.5),
		1, Slot_NBVALFeedbackStates);
	
	m_grey_overlay->setAnimationLine(Slot_GreyState);
	m_grey_overlay->m_visible = false;

	//object
	m_object = NULL;
}

GridElement::~GridElement()
{
	delete m_quality_overlay;
	delete m_grey_overlay;
	m_object = NULL;
}

void GridElement::SetPosition(sf::Vector2f position)
{
	setPosition(position);
	m_grey_overlay->setPosition(position);
	m_quality_overlay->setPosition(position);
	if (m_object != NULL)
		m_object->setPosition(position);
}

void GridElement::SetObject(GameObject* object)
{
	m_object = object;
	if (m_object != NULL)
	{
		m_object->setPosition(getPosition());

		int quality = (int)Game::GetItemQualityClass(object->m_equipment_loot != NULL ? object->m_equipment_loot->m_quality : object->m_weapon_loot->m_quality);
		m_quality_overlay->setAnimationLine(quality);
	}
		
	m_quality_overlay->m_visible = m_object != NULL;
}

void GridElement::Draw(sf::RenderTexture& offscreen)
{
	offscreen.draw(*this);
	m_quality_overlay->Draw(offscreen);

	if (m_object != NULL)
		m_object->Draw(offscreen);

	m_grey_overlay->Draw(offscreen);
}

//GRID
Grid::Grid(sf::Vector2f position, sf::Vector2i nb_squares, SFPanel* panel)
{
	m_position = position;
	m_size = sf::Vector2f(nb_squares.x * GRID_SLOT_SIZE, nb_squares.y * GRID_SLOT_SIZE);
	m_nb_squares = nb_squares;

	for (int j = 0; j < nb_squares.y; j++)
		for (int i = 0; i < nb_squares.x; i++)
		{
			GridElement* element = new GridElement();
			element->m_index = i % nb_squares.x + j * nb_squares.x;
			element->SetPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + position.x + (i * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + position.y + (j * GRID_SLOT_SIZE)));
			element->m_grid = this;
			m_elements.push_back(element);
		}	

	m_first_empty_slot_index = 0;
	m_panel = panel;
}

void Grid::SetPosition(sf::Vector2f position)
{
	m_position = position;

	for (GridElement* element : m_elements)
		element->SetPosition(sf::Vector2f(position.x + GRID_SLOT_SIZE * (0.5 + element->m_index % m_nb_squares.x), position.y + (0.5 + element->m_index / m_nb_squares.x) * GRID_SLOT_SIZE));
}

Grid::~Grid()
{
	for (GridElement* element : m_elements)
		delete element;
}

void Grid::Draw(sf::RenderTexture& offscreen)
{
	for (GridElement* element : m_elements)
		element->Draw(offscreen);
}

int Grid::InsertObject(GameObject* object, int index, bool force_overwrite)
{
	if (m_first_empty_slot_index < 0 && force_overwrite == false)//grid is full, can't insert new item
		return -1;//error code

	int index_returned = index;
	if (index < 0)//look for the first empty slot
	{
		m_elements[m_first_empty_slot_index]->SetObject(object);
		index_returned = m_first_empty_slot_index;
	}
	else if (index < m_nb_squares.x * m_nb_squares.y)
	{
		if (m_elements[index]->m_object == NULL || force_overwrite == true)
			m_elements[index]->SetObject(object);//try inserting at the requested index
		else
			return -1;//slot already occupied
	}

	//update first empty slot index
	for (int i = 0; i <= m_nb_squares.x * m_nb_squares.y; i++)
	{
		if (i == m_nb_squares.x * m_nb_squares.y)//grid is now full
		{
			m_first_empty_slot_index = -1;
		}
		else if (m_elements[i]->m_object == NULL)
		{
			m_first_empty_slot_index = i;
			break;
		}
	}

	return index_returned;

	printf("ERROR: asking to insert an object outside of grid range\n");
	return -1;
}

GridElement* Grid::GetCursorCollision(GameObject& cursor)
{
	if (cursor.m_visible == false)
		return NULL;

	sf::Vector2f pos = cursor.getPosition();
	if (pos.x < m_position.x || pos.x > m_position.x + m_size.x || pos.y < m_position.y || pos.y > m_position.y + m_size.y)
		return NULL;
	
	pos -= m_position;

	int square_x = (int)(pos.x / GRID_SLOT_SIZE);
	int square_y = (int)(pos.y / GRID_SLOT_SIZE);
	int index = square_x % m_nb_squares.x + square_y * m_nb_squares.x;

	return m_elements[index];
}

void Grid::CloneGridContentFrom(Grid* grid)
{
	size_t vectorSize = grid->m_elements.size();
	if (vectorSize == m_elements.size())
	{
		for (size_t i = 0; i < vectorSize; i++)
		{
			delete m_elements[i]->m_object;
			if (grid->m_elements[i]->m_object != NULL)
			{
				m_elements[i]->m_object = grid->m_elements[i]->m_object->Clone();
				m_elements[i]->m_object->setPosition(m_elements[i]->getPosition());
				m_elements[i]->m_quality_overlay->m_visible = true;
				m_elements[i]->m_quality_overlay->setAnimationLine(grid->m_elements[i]->m_quality_overlay->m_currentAnimationIndex);
			}
			else
			{
				m_elements[i]->m_object = NULL;
				m_elements[i]->m_quality_overlay->m_visible = false;
			}
		}

		m_first_empty_slot_index = grid->m_first_empty_slot_index;
	}
}

void Grid::ClearGrid()
{
	for (GridElement* element : m_elements)
		element->SetObject(NULL);

	m_first_empty_slot_index = 0;
}

void Grid::UpdateGreyMaskOnInsufficientCredits(int money)
{
	for (GridElement* element : m_elements)
	{
		GameObject* item = element->m_object;
		element->m_grey_overlay->m_visible = Grid::GetPrice(item) > money || money == 0;
	}
}

int Grid::GetPrice(GameObject* capsule)
{
	if (capsule == NULL)
		return 0;

	int credits = capsule->m_equipment_loot != NULL ? capsule->m_equipment_loot->m_credits : capsule->m_weapon_loot->m_credits;
	float quality = capsule->m_equipment_loot != NULL ? capsule->m_equipment_loot->m_quality : capsule->m_weapon_loot->m_quality;

	return GameObject::GetPrice(credits, quality);
}

int Grid::GetEquipmentType(GameObject* capsule)
{
	if (capsule == NULL)
		return -1;

	return capsule->m_equipment_loot != NULL ? capsule->m_equipment_loot->m_equipmentType : NBVAL_Equipment;
}