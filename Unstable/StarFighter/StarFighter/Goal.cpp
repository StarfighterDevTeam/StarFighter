#include "Goal.h"

extern Game* CurrentGame;

using namespace sf;

Goal::Goal()
{

}

Goal::Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber) : GameObject(position, sf::Vector2f(0, 0), textureName, size, origin, frameNumber, animationNumber)
{
	m_team = team;
}

Goal::Goal(Teams team, sf::Vector2f position, std::string textureName, sf::Vector2f size) : GameObject(position, sf::Vector2f(0, 0), textureName, size)
{
	m_team = team;
}

Goal::Goal(Teams team, sf::Vector2f position, sf::Vector2f size)
{
	m_team = team;

	//pixel array creation
	const unsigned int W = size.x;
	const unsigned int H = size.y;

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];

	ostringstream ss;
	if (m_team == BlueTeam)
	{
		ss << "blue_goal";
		collider_type = GoalBlueObject;
		
		for (int i = 0; i < W*H * 4; i += 4)
		{
			pixels[i] = 0;			// R
			pixels[i + 1] = 162;	// G
			pixels[i + 2] = 232;	// B
			pixels[i + 3] = 255;	//A
			
		}
	}
	else if (m_team == RedTeam)
	{
		ss << "red_goal";
		collider_type = GoalRedObject;
		
		for (int i = 0; i < W*H * 4; i += 4)
		{
			pixels[i] = 237;		// R
			pixels[i + 1] = 25;		// G
			pixels[i + 2] = 36;		// B
			pixels[i + 3] = 255;	// A
			
		}
	}
	else
	{
		ss << "green_goal";
		collider_type = BumperGreenObject;

		for (int i = 0; i < W*H * 4; i += 4)
		{
			pixels[i] = 0;			// R
			pixels[i + 1] = 255;	// G
			pixels[i + 2] = 0;		// B
			pixels[i + 3] = 255;	// A
			
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

Goal::~Goal()
{
	
}