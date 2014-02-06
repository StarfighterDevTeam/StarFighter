#ifndef TEXTURELOADER_H_INCLUDED
#define TEXTURELOADER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <tuple>
#include "Assets.h"

//NB: this is a singleton
class TextureLoader
{
private:
	// Constructeur/destructeur
	TextureLoader ()
		: _value (0) { }
	~TextureLoader () { }

	//Loaded textures with counter
	std::map<std::string, std::tuple<int, sf::Texture>> _loadedTextures;

public:
	// Interface publique
	sf::Texture* TextureLoader::loadTexture (std::string filename, int sizeX, int sizeY)
	{ 
		//Do we already have this loaded ?
		std::map<std::string, std::tuple<int, sf::Texture>>::iterator it = this->_loadedTextures.find(filename);

		if(it != this->_loadedTextures.end())
		{
			//element found;
			std::get<0>(it->second)++;		//Increment its counter
			return &(std::get<1>(it->second));	//Return the texture
		}

		//Not found, load the texture
		//Allocating memory
		sf::Texture *texture;
		texture = (sf::Texture *)malloc(sizeof (*texture));

		if (!(*texture).loadFromFile(filename, sf::IntRect(0, 0, sizeX, sizeY)))
		{
			printf("error loading texture");
			//TODO handle these
		}

		//Add the texture
		this->_loadedTextures[filename] = std::make_tuple(1,*texture);

		return texture;
	}

	void TextureLoader::unloadTexture (std::string filename)
	{ 
		//Do we already have this loaded ?
		std::map<std::string, std::tuple<int, sf::Texture>>::iterator it = this->_loadedTextures.find(filename);

		if(it != this->_loadedTextures.end())
		{
			//element found;
			std::get<0>(it->second)--;		//Decrement its counter
			if(std::get<0>(it->second) == 0)
			{
				sf::Texture *texture = &(std::get<1>(it->second));

				//Removing it from the array
				this->_loadedTextures.erase (filename);     
				//Cleaning memory
				free(texture);
			}

		}

	}

	void TextureLoader::loadAll()
	{
		//Load all the TExtures:
		this->getInstance()->loadTexture("Assets/2D/laser.png",4,12);
		this->getInstance()->loadTexture("Assets/2D/laser_blue.png",4,16);
		this->getInstance()->loadTexture("Assets/2D/player_sprite.png",192,64);
		this->getInstance()->loadTexture("Assets/2D/enemyX.png",32,32);
		this->getInstance()->loadTexture("Assets/2D/enemyY.png",32,32);
		this->getInstance()->loadTexture("Assets/2D/FX_explosion.png",1280,236);
		this->getInstance()->loadTexture("Assets/2D/background.png",800,1800);
		this->getInstance()->loadTexture("Assets/2D/AirBrake.png",64,64);
	}

	// Fonctions de cr�ation et destruction du singleton
	static TextureLoader *getInstance ()
	{
		if (NULL == _singleton)
		{
			_singleton =  new TextureLoader;
		}
		return _singleton;
	}

	static void kill ()
	{
		if (NULL != _singleton)
		{
			delete _singleton;
			_singleton = NULL;
		}
	}

private:
	int _value;
	static TextureLoader *_singleton;
};

#endif // TEXTURELOADER_H_INCLUDED