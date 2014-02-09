#ifndef TEXTURELOADER_H_INCLUDED
#define TEXTURELOADER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <tuple>
#include "Assets.h"
#include "Exceptions.h"

//NB: this is a singleton
class TextureLoader
{
private:
	// Constructeur/destructeur
	TextureLoader ()
		: _value (0) { }
	~TextureLoader () { }

	//Loaded textures with counter
	std::map<std::string, sf::Texture> _loadedTextures;

public:
	// Interface publique
	sf::Texture* TextureLoader::loadTexture (std::string filename, int sizeX, int sizeY)
	{ 
		//Do we already have this loaded ?
		std::map<std::string, sf::Texture>::iterator it = this->_loadedTextures.find(filename);

		if(it != this->_loadedTextures.end())
		{
			//element found;
			return &(it->second);	//Return the texture
		}

		//Not found, load the texture
		//Allocating memory
		sf::Texture* texture;
		texture = (sf::Texture *)malloc(sizeof (*texture));

		if (!(*texture).loadFromFile(filename, sf::IntRect(0, 0, sizeX, sizeY)))
		{
			throw invalid_argument(ExceptionUtils::getExceptionMessage(ExceptionLevel::Error,ExceptionUtils::format("TextureLoad error: Unable to load texture from file '%s'",filename)));
		}

		//Add the texture
		this->_loadedTextures[filename] = *texture;
		return texture;
	}

	void TextureLoader::unloadTexture (std::string filename)
	{ 
		//Do we already have this loaded ?
		std::map<std::string, sf::Texture>::iterator it = this->_loadedTextures.find(filename);

		if(it != this->_loadedTextures.end())
		{
			//element found;
			sf::Texture* texture = &(it->second);
			//Removing it from the array
			this->_loadedTextures.erase (filename);     
			//Cleaning memory
			free(texture);
		}
	}

	void TextureLoader::loadAll()
	{
		//Load all the Textures:
		this->getInstance()->loadTexture("Assets/2D/laser.png",4,12);
		this->getInstance()->loadTexture("Assets/2D/laser_blue.png",4,16);
		this->getInstance()->loadTexture("Assets/2D/player_sprite.png",192,64);
		this->getInstance()->loadTexture("Assets/2D/enemyX.png",32,32);
		this->getInstance()->loadTexture("Assets/2D/enemyY.png",32,32);
		this->getInstance()->loadTexture("Assets/2D/FX_explosion.png",1280,236);
		this->getInstance()->loadTexture("Assets/2D/background.png",800,1800);
		this->getInstance()->loadTexture("Assets/2D/AirBrake.png",64,64);
	}

	// Fonctions de création et destruction du singleton
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