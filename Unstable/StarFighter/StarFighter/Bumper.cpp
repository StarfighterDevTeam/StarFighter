#include "Bumper.h"

extern Game* CurrentGame;

using namespace sf;

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

	//pixel array creation
	const unsigned int W = size.x;
	const unsigned int H = size.y;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	ostringstream ss;
	if (type == OnlyBlueTeamThrough)
	{
		ss << "blue";
		collider_type = BumperBlueObject;
		for (int i = 0; i < W*H * 4; i += 4)
		{
			pixels[i] = 0;			// R
			pixels[i + 1] = 0;		// G
			pixels[i + 2] = 255;	// B
			if ((i / 4) % W == 0 || (i / 4) % W == (W - 1)) // A
			{
				pixels[i + 3] = BUMPER_OUTSIDE_GLOW_ALPHA;
			}
			else
			{
				pixels[i + 3] = 255;
			}
		}
	}
	else if (type == OnlyRedTeamThrough)
	{
		ss << "red";
		collider_type = BumperRedObject;
		for (int i = 0; i < W*H * 4; i += 4)
		{
			pixels[i] = 255;		// R
			pixels[i + 1] = 0;		// G
			pixels[i + 2] = 0;		// B
			if ((i / 4) % W == 0 || (i / 4) % W == (W - 1)) // A
			{
				pixels[i + 3] = BUMPER_OUTSIDE_GLOW_ALPHA;
			}
			else
			{
				pixels[i + 3] = 255;
			}
		}
	}
	else
	{
		ss << "green";
		collider_type = BumperGreenObject;
		for (int i = 0; i < W * H * 4; i += 4)
		{
			pixels[i] = 0;			// R
			pixels[i + 1] = 255;	// G
			pixels[i + 2] = 0;		// B
			if ((i / 4) % W == 0 || (i / 4) % W == (W-1)) // A
			{
				pixels[i + 3] = BUMPER_OUTSIDE_GLOW_ALPHA;
			}
			else
			{
				pixels[i + 3] = 255;
			}
		}
	}

	if (size.x > size.y)
	{
		ss << "_H_" << size.x;
	}
	else if (size.x < size.y)
	{
		ss << "_V_" << size.y;
	}
	else
	{
		ss << "_D"; // not supported
	}
	
	std::string s = ss.str();

	Init(position, sf::Vector2f(0, 0), s, sf::Vector2f(W, H), 1, 1, pixels);
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