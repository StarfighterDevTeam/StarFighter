#ifndef TEXTURELOADER_H_INCLUDED
#define TEXTURELOADER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <string>
#include <map>
#include <tuple>
#include "Assets.h"
#include "TextUtils.h"
#include "Logger.h"

//NB: this is a singleton
class TextureLoader
{
private:
	// Constructeur/destructeur
	TextureLoader ()
		: _value (0) { }
	~TextureLoader () { }

	//Loaded textures with counter
	std::map<std::string, sf::Texture*> _loadedTextures;

public:
	// Interface publique
	sf::Texture* loadTexture (std::string filename, int sizeX, int sizeY)
	{ 
		//Do we already have this loaded ?
		std::map<std::string, sf::Texture*>::iterator it = this->_loadedTextures.find(filename);

		if(it != this->_loadedTextures.end())
		{
			//element found;
			return it->second;	//Return the texture*
		}

		//Not found, load the texture
		sf::Texture* texture = new sf::Texture;
		if (!(*texture).loadFromFile(filename, sf::IntRect(0, 0, sizeX, sizeY)))
		{
			throw invalid_argument(TextUtils::format("TextureLoad error: Unable to load texture from file '%s'",(char*)filename.c_str()));
		}

		//Add the texture
		this->_loadedTextures[filename] = texture;
		LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("Loading texture from file '%s'", (char*)filename.c_str()));
		return texture;
	}

	void unloadTexture (std::string filename)
	{ 
		//Do we already have this loaded ?
		std::map<std::string, sf::Texture*>::iterator it = this->_loadedTextures.find(filename);

		if(it != this->_loadedTextures.end())
		{
			//element found;
			sf::Texture* texture = it->second;
			//Removing it from the array
			this->_loadedTextures.erase (filename);     
			//Cleaning memory
			delete texture;
		}
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