#include "Game.h"

void Game::init(RenderWindow* window)
{
	this->window = window;
	TextureLoader::getInstance ()->loadAll();
	for(int i =0; i< (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered));i++)
	{
		sceneIndependantsLayered[i] = new std::list<Independant*>;
	}
}


sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::addToScene(Independant *object, int layer /* 0 */)
{
	if(layer >= 0 && layer < (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered)))
	{
		(*(sceneIndependantsLayered[layer])).push_back(object); 
		this->sceneIndependants.push_back(object);
	}
	else
	{
		//throw ?
		printf("Erreur, mauvais layer, connard\n");
	}
}

void Game::updateScene(Time deltaTime)
{
	//Checking colisions
	colisionChecks(deltaTime);

	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
		(*(*it)).update(deltaTime);
	}
}

void Game::drawScene()
{
	for(int i =0; i< (sizeof(sceneIndependantsLayered)/sizeof(*sceneIndependantsLayered));i++)
	{
		for (std::list<Independant*>::iterator it = (*(this->sceneIndependantsLayered[i])).begin(); it != (*(this->sceneIndependantsLayered[i])).end(); it++)
		{
			if((*(*it)).visible)
				this->window->draw((*(*it)));
		}
	}
}

void Game::colisionChecks(Time deltaTime)
{
	for (std::list<Independant*>::iterator it1 = (this->sceneIndependants).begin(); it1 != std::prev((this->sceneIndependants).end()); it1++)
	{
		for (std::list<Independant*>::iterator it2 = std::next(it1); it2 != (this->sceneIndependants).end(); it2++)
		{

			//Bullets are invisible after impact
			if((*(*it1)).collide_with((*(*it2))))
			{
				if((*(*it1)).collider_type == 2 || (*(*it1)).collider_type == 3)
					(*(*it1)).setVisible(false);

				if((*(*it2)).collider_type == 2 || (*(*it2)).collider_type == 3)
					(*(*it2)).setVisible(false);

				printf("boom [%d vs %d]\n", (*(*it1)).collider_type, (*(*it2)).collider_type);
			}
		}
	}
}


