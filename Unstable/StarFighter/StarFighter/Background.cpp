#include "Background.h"

Background::Background()
{
	speed.x = 0;
	speed.y = 0;
}

void Background::Init(int x, int y)
{
	static sf::Texture texture;
	if (!texture.loadFromFile(BACKGROUND_FILENAME, sf::IntRect(x, y, 800, 600)))
	{
		printf("error loading ship");
	}
	this->setTexture(texture);
}