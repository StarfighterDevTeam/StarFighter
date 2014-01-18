#include "Game.h"

void Game::init(RenderWindow* window)
{
	this->window = window;
	TextureLoader::getInstance ()->loadAll();
}


sf::RenderWindow* Game::getMainWindow()
{
	return this->window;
}

void Game::addToScene(Independant *object)
{
	this->sceneIndependants.push_back(object); 
}

void Game::updateScene(Time deltaTime)
{
	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
		(*(*it)).update(deltaTime);
		
	}
}

void Game::drawScene()
{
	for (std::list<Independant*>::iterator it = (this->sceneIndependants).begin(); it != (this->sceneIndependants).end(); it++)
	{
		this->window->draw((*(*it)));
	}
}


