#include "HudGrid.h"

ObjectGrid::ObjectGrid()
{
	focus = sf::Vector2i(-1, -1);
	m_visible = true;
}

ObjectGrid::ObjectGrid(sf::Vector2f position, sf::Vector2i squares, bool fill_with_fake, bool fill_with_grey, bool fill_with_colored_background)
{
	m_visible = true;

	for (int i = 0; i < squares.x; i++)
	{
		for (int j = 0; j < squares.y; j++)
		{
			if (fill_with_fake)
			{
				GameObject* empty_slot = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), EMPTYSLOT_FILENAME, sf::Vector2f(GRID_SLOT_SIZE, GRID_SLOT_SIZE),
					sf::Vector2f(GRID_SLOT_SIZE / 2, GRID_SLOT_SIZE / 2), 1, Slot_NBVALFeedbackStates);

				empty_slot->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + position.x + (j * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + position.y + (i * GRID_SLOT_SIZE)));

				grid[i][j] = empty_slot;
			}
			else if (fill_with_grey)
			{
				GameObject* grey_slot = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), EMPTYSLOT_FILENAME, sf::Vector2f(GRID_SLOT_SIZE, GRID_SLOT_SIZE),
					sf::Vector2f(GRID_SLOT_SIZE / 2, GRID_SLOT_SIZE / 2), 1, Slot_NBVALFeedbackStates);

				grey_slot->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + position.x + (j * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + position.y + (i * GRID_SLOT_SIZE)));

				grid[i][j] = grey_slot;
				grey_slot->m_visible = false;
				grey_slot->setAnimationLine(2);
			}
			else if (fill_with_colored_background)
			{
				GameObject* colored_slot = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), QUALITY_BACKGROUND, sf::Vector2f(GRID_SLOT_SIZE, GRID_SLOT_SIZE),
					sf::Vector2f(GRID_SLOT_SIZE / 2, GRID_SLOT_SIZE / 2), 1, NBVAL_ItemQuality);

				colored_slot->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + position.x + (j * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + position.y + (i * GRID_SLOT_SIZE)));

				grid[i][j] = colored_slot;
				colored_slot->m_visible = false;
			}
			else
			{
				grid[i][j] = NULL;
			}
		}
	}

	this->position = position;
	this->squares = squares;
	this->focus = sf::Vector2i(-1, -1);
}

int ObjectGrid::getFocusIntIndex()
{
	return (focus.y + (focus.x * this->squares.y));
}

int ObjectGrid::GetIntIndex(sf::Vector2i index)
{
	return index.y + (index.x * squares.y);
}

bool ObjectGrid::insertObject(GameObject& object, int index, bool overwrite_existing)
{
	//if no specific index is provided, we look for the first empty slot...
	if (index < 0)
	{
		for (int i = 0; i < this->squares.x; i++)
		{
			for (int j = 0; j < this->squares.y; j++)
			{
				if (grid[i][j] == NULL)
				{
					grid[i][j] = &object;
					grid[i][j]->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + this->position.x + (j * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + this->position.y + (i * GRID_SLOT_SIZE)));
					return true;
				}
			}
		}
		//overwrite last item if no free cell found
		if (overwrite_existing)
		{
			delete grid[squares.x - 1][squares.y - 1];
			grid[squares.x - 1][squares.y - 1] = &object;
			return true;
		}
		//no free slot found
		return false;
	}
	//...otherwise we head directly to the requested index
	else
	{
		if (index > squares.x * squares.y - 1)
		{
			return false;
		}

		int r = index % squares.y;
		int l = index / squares.y;

		//case: doesn't fit the number of slots available in the interface
		//case: the requested slot is free
		if (grid[l][r] == NULL || overwrite_existing)
		{
			if (overwrite_existing)
			{
				delete grid[l][r];
			}
			grid[l][r] = &object;
			grid[l][r]->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + this->position.x + (r * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + this->position.y + (l * GRID_SLOT_SIZE)));

			return true;
		}
		else
		{
			LOGGER_WRITE(Logger::DEBUG, "<!> Error: Trying to add an equipement on an existing ship equipment slot.\n");
			return false;
		}
	}
}

void ObjectGrid::SetGridPosition(sf::Vector2f position)
{
	for (int i = 0; i < squares.x; i++)
	{
		for (int j = 0; j < squares.y; j++)
		{
			if (grid[i][j] != NULL)
			{
				grid[i][j]->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + position.x + (j * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + position.y + (i * GRID_SLOT_SIZE)));
			}
		}
	}

	this->position = position;
}

void ObjectGrid::Draw(sf::RenderTexture& offscreen)
{
	if (m_visible)
	{
		for (int i = 0; i < this->squares.x; i++)
		{
			for (int j = 0; j < this->squares.y; j++)
			{
				if (grid[i][j] && grid[i][j]->m_visible)
				{
					sf::Text item_level;
					item_level.setCharacterSize(14);
					offscreen.draw(*grid[i][j]);
				}
			}
		}
	}
}

int ObjectGrid::isCursorColliding(GameObject& cursor)
{
	if (!cursor.m_visible)
	{
		return -1;
	}
	else
	{
		//1st test
		sf::FloatRect gridBounds(sf::Vector2f(grid[0][0]->getPosition().x - GRID_SLOT_SIZE / 2, grid[0][0]->getPosition().y - GRID_SLOT_SIZE / 2),
			sf::Vector2f(GRID_SLOT_SIZE * squares.y, GRID_SLOT_SIZE * squares.x));

		if (!gridBounds.contains(cursor.getPosition().x, cursor.getPosition().y))
		{
			return -1;
		}

		//2nd test
		for (size_t i = 0; i < this->squares.x; i++)
		{
			for (size_t j = 0; j < this->squares.y; j++)
			{
				if (grid[i][j] != NULL)
				{
					sf::FloatRect rect_ = grid[i][j]->getGlobalBounds();
					sf::IntRect bounds(sf::IntRect((int)rect_.left, (int)rect_.top, (int)rect_.width, (int)rect_.height));
					if (bounds.contains(cursor.getPosition().x, cursor.getPosition().y))
					{
						int index_ = j + (i * this->squares.y);
						return index_;
					}
				}
			}
		}
		return -1;
	}
}

bool ObjectGrid::SetCellHighlightState(int index, SlotFeedbackStates highlight_state)
{
	if (index < 0 || index >= squares.x * squares.y)
	{
		return false;
	}

	int r = index % squares.y;
	int l = index / squares.y;

	if (!grid[l][r])
	{
		return false;
	}

	grid[l][r]->setAnimationLine(highlight_state);
	focus = sf::Vector2i(r, l);
	return true;
	
}

bool ObjectGrid::HighlightCell(int index)
{
	int r = index % squares.y;
	int l = index / squares.y;

	//case: error in index calculation
	if (l > squares.x)
	{
		LOGGER_WRITE(Logger::DEBUG, "<!> Error: Trying to highlight a grid cell index that doesn't exist.\n");
		return false;
	}
	//case: the requested slot is free
	else
	{
		grid[l][r]->setAnimationLine(Slot_HighlightState);
		this->focus = sf::Vector2i(r, l);
		return true;
	}
}

bool ObjectGrid::GarbageCell(int index)
{
	int r = index % squares.y;
	int l = index / squares.y;

	if (l > squares.x)
	{
		LOGGER_WRITE(Logger::DEBUG, "<!> Error: Trying to garbage a pointer from a grid cell index that doesn't exist.\n");
	}

	if (grid[l][r])
	{
		delete grid[l][r];
		grid[l][r] = NULL;
		return true;
	}

	return false;
}

bool ObjectGrid::CleanFocus()
{
	if (focus.x != -1 || focus.y != -1)
	{
		grid[focus.y][focus.x]->setAnimationLine(Slot_NormalState);
		focus = sf::Vector2i(-1, -1);
		return true;
	}
	return false;
}

void ObjectGrid::ClearGrid()
{
	for (int i = 0; i < this->squares.x; i++)
	{
		for (int j = 0; j < this->squares.y; j++)
		{
			if (grid[i][j] != NULL)
			{
				delete grid[i][j];
				grid[i][j] = NULL;
			}
		}
	}
}

void ObjectGrid::ClearHighlight()
{
	for (int i = 0; i < squares.x; i++)
	{
		for (int j = 0; j < squares.y; j++)
		{
			if (grid[i][j])
			{
				grid[i][j]->setAnimationLine(Slot_NormalState);
			}
		}
	}
}

bool ObjectGrid::SwapObjectsBetweenGrids(ObjectGrid& grid, ObjectGrid& grid2, int index1, int index2)
{
	if (grid.getCellPointerFromIntIndex(index1) != NULL)
	{
		GameObject* tmpObj1 = grid.getCellPointerFromIntIndex(index1);
		//Equipement > Ship
		grid.setCellPointerForIntIndex(index1, grid2.getCellPointerFromIntIndex(index2));
		//Ship > equipement
		grid2.setCellPointerForIntIndex(index2, tmpObj1);
		tmpObj1 = NULL;
	}
	else
	{
		//Equipement > Ship
		grid.setCellPointerForIntIndex(index1, grid2.getCellPointerFromIntIndex(index2));
		//Equipment = NULL
		grid2.setCellPointerForIntIndex(index2, NULL);
	}

	return true;
}

GameObject* ObjectGrid::getCellPointerFromIntIndex(int index)
{
	int r = index % squares.y;
	int l = index / squares.y;

	if (l > squares.x)
	{
		LOGGER_WRITE(Logger::DEBUG, "<!> Error: Trying to get a pointer from a grid cell index that doesn't exist.\n");
	}

	return this->grid[l][r];

}

void ObjectGrid::setCellPointerForIntIndex(int index, GameObject* GameObject)
{
	if (index < 0)
	{
		for (int i = 0; i < this->squares.x; i++)
		{
			for (int j = 0; j < this->squares.y; j++)
			{
				if (grid[i][j] == NULL)
				{
					grid[i][j] = GameObject;
					return;
				}
			}
		}
		return;
	}

	int r = index % squares.y;
	int l = index / squares.y;

	if (l > squares.x)
	{
		LOGGER_WRITE(Logger::DEBUG, "<!> Error: Trying to get a pointer from a grid cell index that doesn't exist.\n");
	}
	this->grid[l][r] = GameObject;
	if (GameObject != NULL)
	{
		this->grid[l][r]->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + this->position.x + (r * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + this->position.y + (l * GRID_SLOT_SIZE)));
	}
}

//v2 ---------------------------------

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

void GridElement::SetObject(GameObject* object, EquipmentQuality quality)
{
	m_object = object;
	if (m_object != NULL)
		m_object->setPosition(getPosition());
	m_quality_overlay->setAnimationLine((int)quality);
	m_quality_overlay->m_visible = m_object != NULL;
}

void GridElement::Draw(sf::RenderTexture& offscreen)
{
	offscreen.draw(*this);

	if (m_object != NULL)
		m_object->Draw(offscreen);

	m_quality_overlay->Draw(offscreen);
	m_grey_overlay->Draw(offscreen);
}

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

int Grid::InsertObject(GameObject* object, int index, EquipmentQuality quality, bool force_overwrite)
{
	if (m_first_empty_slot_index < 0 && force_overwrite == false)//grid is full, can't insert new item
		return -1;//error code

	int index_returned = index;
	if (index < 0)//look for the first empty slot
	{
		m_elements[m_first_empty_slot_index]->SetObject(object, quality);
		index_returned = m_first_empty_slot_index;
	}
	else if (index < m_nb_squares.x * m_nb_squares.y)
	{
		if (m_elements[index]->m_object == NULL || force_overwrite == true)
			m_elements[index]->SetObject(object, quality);//try inserting at the requested index
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