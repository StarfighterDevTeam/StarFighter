#include "Bumper.h"

extern Game* CurrentGame;

using namespace sf;

// ----------------SHIP ---------------

Bumper::Bumper()
{

}

Bumper::Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_type = type;
}

Bumper::Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size) : GameObject(position, sf::Vector2f(0, 0), textureName, size)
{
	m_type = type;
}

Bumper::Bumper(BumperType type, sf::Vector2f position, sf::Vector2f size)
{
	m_type = type;

	const unsigned int W = size.x;
	const unsigned int H = size.y;

	//sf::Texture texture;
	//texture.create(W, H);

	sf::Uint8* pixels = new sf::Uint8[W*H * 4];

	for (int i = 0; i < W*H * 4; i += 4) 
	{
		pixels[i] = 0;			// R
		pixels[i + 1] = 255;	// G
		pixels[i + 2] = 0;		// B
		pixels[i + 3] = 100;	// A
	}

	//texture.update(pixels);

	//Init(position, sf::Vector2f(0, 0), &texture);
	Init(position, sf::Vector2f(0, 0), "test", sf::Vector2f(W, H), 1, 1, pixels);

	// ...

	collider_type = BumperRedObject;
}
/*
void Bumper::Init()
{
	if (m_type == OnlyBlueTeamThrough)
	{
		collider_type = BumperBlueObject;
	}
	else if (m_type == OnlyRedTeamThrough)
	{
		collider_type = BumperRedObject;
	}
	else//if (m_type == OnlyPlayersThrough)
	{
		collider_type = BumperGreenObject;
	}
}
*/

Bumper::~Bumper()
{
	
}

void Bumper::update(sf::Time deltaTime)
{
	AnimatedSprite::update(deltaTime);
}