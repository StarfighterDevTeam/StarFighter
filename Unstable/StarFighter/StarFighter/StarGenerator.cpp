#include "StarGenerator.h"

extern Game* CurrentGame;

using namespace sf;

StarGenerator::StarGenerator()
{
	m_generator_clock = 0.f;
	m_generator_threshold = RandomizeFloatBetweenValues(sf::Vector2f(0.1, 0.5)) * 0.5;
}

void StarGenerator::Update(sf::Time deltaTime)
{
	m_generator_clock += deltaTime.asSeconds();

	if (m_generator_clock > m_generator_threshold)
	{
		//randomization
		//float speed = RandomizeFloatBetweenValues(sf::Vector2f(50, 400));
		float position_x = RandomizeFloatBetweenValues(sf::Vector2f(0, 1));
		float position_y = RandomizeFloatBetweenValues(sf::Vector2f(0, 1));
		Uint8 r = RandomizeIntBetweenValues(0, 5) > 0 ? 0 : 255;
		//Uint8 g = RandomizeIntBetweenValues(0, 1) > 0 ? 0 : 255;
		Uint8 b = RandomizeIntBetweenValues(0, 1) > 0 && r > 0 ? 0 : 255;
		sf::Color color = sf::Color(r, 0, b, 150);
		float size = RandomizeFloatBetweenValues(sf::Vector2f(4, 12));
		float pulse_periodicity = RandomizeFloatBetweenValues(sf::Vector2f(0.5, 2));

		//spawn
		Star* star = new Star(sf::Vector2f(position_x * (*CurrentGame).m_map_size.x, position_y * (*CurrentGame).m_map_size.y), sf::Vector2f(0, 0), color, sf::Vector2f(size, size), pulse_periodicity);
		(*CurrentGame).addToScene(star, StarLayer, BackgroundObject);

		//reset
		m_generator_clock = 0.f;
		m_generator_threshold = RandomizeFloatBetweenValues(sf::Vector2f(0.1, 0.5));
	}
}

Star* StarGenerator::CreateStar()
{
	//randomization
	//float speed = RandomizeFloatBetweenValues(sf::Vector2f(50, 400));
	float position_x = RandomizeFloatBetweenValues(sf::Vector2f(-0.5, 0.5));
	float position_y = RandomizeFloatBetweenValues(sf::Vector2f(-0.5, 0.5));
	Uint8 r = RandomizeIntBetweenValues(0, 5) > 0 ? 0 : 255;
	//Uint8 g = RandomizeIntBetweenValues(0, 1) > 0 ? 0 : 255;
	Uint8 b = RandomizeIntBetweenValues(0, 1) > 0 && r > 0 ? 0 : 255;
	sf::Color color = sf::Color(r, 0, b, 150);
	float size = RandomizeFloatBetweenValues(sf::Vector2f(4, 12));
	float pulse_periodicity = RandomizeFloatBetweenValues(sf::Vector2f(0.5, 2));

	//spawn
	Star* star = new Star(sf::Vector2f(position_x * (*CurrentGame).m_map_size.x, position_y * (*CurrentGame).m_map_size.y), sf::Vector2f(0, 0), color, sf::Vector2f(size, size), pulse_periodicity);
	(*CurrentGame).addToScene(star, StarLayer, BackgroundObject);

	return star;
}