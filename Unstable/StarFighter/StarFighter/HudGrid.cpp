#include "HudGrid.h"

ObjectGrid::ObjectGrid()
{
	this->focus = sf::Vector2i(-1, -1);
}

ObjectGrid::ObjectGrid(sf::Vector2f position, sf::Vector2i squares, bool fill_with_fake)
{
	for (int i = 0; i < squares.x; i++)
	{
		for (int j = 0; j < squares.y; j++)
		{
			if (fill_with_fake)
			{
				Independant* empty_slot = new Independant(sf::Vector2f(0, 0), sf::Vector2f(0, 0), EMPTYSLOT_FILENAME, sf::Vector2f(GRID_SLOT_SIZE, GRID_SLOT_SIZE),
					sf::Vector2f(GRID_SLOT_SIZE / 2, GRID_SLOT_SIZE / 2), 1, EMPTYSLOT_ANIMATION_NUMBER);

				empty_slot->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + position.x + (j * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + position.y + (i * GRID_SLOT_SIZE)));

				grid[i][j] = empty_slot;
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

bool ObjectGrid::insertObject(Independant& object, int index, bool overwrite_existing)
{
	//if no specific index is provided, we look for the first empty slot...
	if (index < 0)
	{
		for (int i = 0; i < this->squares.x; i++)
		{
			for (int j = 0; j < this->squares.y; j++)
			{
				if (grid[i][j] == NULL || overwrite_existing)
				{
					if (overwrite_existing)
					{
						delete grid[i][j];
					}
					grid[i][j] = &object;
					grid[i][j]->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + this->position.x + (j * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + this->position.y + (i * GRID_SLOT_SIZE)));

					return true;
				}
			}
		}
		//no free slot found
		return false;
	}
	//...otherwise we head directly to the requested index
	else
	{
		int r = index % squares.y;
		int l = index / squares.y;

		//case: doesn't fit the number of slots available in the interface
		if (l > squares.x)
		{
			LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: Trying to add an equipment to the HUD (shipGrid) for an equipment type that has no slot planned.\n");
			return false;
		}
		//case: the requested slot is free
		else if (grid[l][r] == NULL || overwrite_existing)
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
			LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: Trying to add an equipement on an existing ship equipment slot.\n");
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
	for (int i = 0; i < this->squares.x; i++)
	{
		for (int j = 0; j < this->squares.y; j++)
		{
			if (grid[i][j] != NULL)
			{
				offscreen.draw(*grid[i][j]);
			}
		}
	}
}

int ObjectGrid::isCursorColling(Independant& cursor)
{
	if (!cursor.visible)
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

bool ObjectGrid::HighlightCell(int index)
{
	int r = index % squares.y;
	int l = index / squares.y;

	//case: error in index calculation
	if (l > squares.x)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: Trying to highlight a grid cell index that doesn't exist.\n");
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
	Independant* tmp_ptr = getCellPointerFromIntIndex(index);
	if (tmp_ptr != NULL)
	{
		delete tmp_ptr;
		tmp_ptr = NULL;
		return true;
	}
	tmp_ptr = NULL;
	LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: Trying to garbage an empty grid cell.\n");
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

Independant* ObjectGrid::getCellPointerFromIntIndex(int index)
{
	int r = index % squares.y;
	int l = index / squares.y;

	if (l > squares.x)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: Trying to get a pointer from a grid cell index that doesn't exist.\n");
	}

	return this->grid[l][r];

}

void ObjectGrid::setCellPointerForIntIndex(int index, Independant* independant)
{
	int r = index % squares.y;
	int l = index / squares.y;

	if (l > squares.x)
	{
		LOGGER_WRITE(Logger::Priority::DEBUG, "<!> Error: Trying to get a pointer from a grid cell index that doesn't exist.\n");
	}
	this->grid[l][r] = independant;
	if (independant != NULL)
	{
		this->grid[l][r]->setPosition(sf::Vector2f((GRID_SLOT_SIZE / 2) + this->position.x + (r * GRID_SLOT_SIZE), (GRID_SLOT_SIZE / 2) + this->position.y + (l * GRID_SLOT_SIZE)));
	}
}