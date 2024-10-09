#include "StarGenerator.h"

extern Game* CurrentGame;

using namespace sf;

StarGenerator::StarGenerator()
{
	m_generator_clock = 0.f;
	m_generator_threshold = RandomizeFloatBetweenValues(0.1f, 0.5f) * 0.5f;
}

void StarGenerator::Update(sf::Time deltaTime)
{
	m_generator_clock += deltaTime.asSeconds();

	if (m_generator_clock > m_generator_threshold)
	{
		//randomization
		//float speed = RandomizeFloatBetweenValues(sf::Vector2f(50, 400));
		float position_x = RandomizeFloatBetweenValues(0.f, 1.f);
		float position_y = RandomizeFloatBetweenValues(0.f, 1.f);
		Uint8 r = RandomizeIntBetweenValues(0, 5) > 0 ? 0 : 255;
		//Uint8 g = RandomizeIntBetweenValues(0, 1) > 0 ? 0 : 255;
		Uint8 b = RandomizeIntBetweenValues(0, 1) > 0 && r > 0 ? 0 : 255;
		sf::Color color = sf::Color(r, 0.f, b, 150.f);
		float size = RandomizeFloatBetweenValues(4.f, 12.f);
		float pulse_periodicity = RandomizeFloatBetweenValues(0.5f, 2.f);

		//spawn
		Star* star = new Star(sf::Vector2f(position_x * (*CurrentGame).m_map_size.x, position_y * (*CurrentGame).m_map_size.y), sf::Vector2f(0, 0), color, sf::Vector2f(size, size), pulse_periodicity);
		(*CurrentGame).addToScene(star, StarLayer, BackgroundObject, false);

		//reset
		m_generator_clock = 0.f;
		m_generator_threshold = RandomizeFloatBetweenValues(0.1, 0.5);
	}
}

Star* StarGenerator::CreateStar()
{
	//randomization
	//float speed = RandomizeFloatBetweenValues(sf::Vector2f(50, 400));
	float position_x = RandomizeFloatBetweenValues(-0.5f, 0.5f);
	float position_y = RandomizeFloatBetweenValues(-0.5f, 0.5f);
	Uint8 r = RandomizeIntBetweenValues(0, 5) > 0 ? 0 : 255;
	//Uint8 g = RandomizeIntBetweenValues(0, 1) > 0 ? 0 : 255;
	Uint8 b = RandomizeIntBetweenValues(0, 1) > 0 && r > 0 ? 0 : 255;
	sf::Color color = sf::Color(r, 0.f, b, 150.f);
	float size = RandomizeFloatBetweenValues(4.f, 12.f);
	float pulse_periodicity = RandomizeFloatBetweenValues(0.2f, 1.f);

	//spawn
	Star* star = new Star(sf::Vector2f(position_x * (*CurrentGame).m_map_size.x, position_y * (*CurrentGame).m_map_size.y), sf::Vector2f(0, 0), color, sf::Vector2f(size, size), pulse_periodicity);
	(*CurrentGame).addToScene(star, StarLayer, BackgroundObject, false);

	return star;
}

Star* StarGenerator::CreateStar(sf::Vector2i sector_index)
{
	Star* star = CreateStar();

	(*CurrentGame).SetStarSectorIndex(star, sector_index);

	star->m_position.x += 1.f * STAR_SECTOR_SIZE * RandomizeFloatBetweenValues(-0.5f, 0.5f);
	star->m_position.y -= 1.f * STAR_SECTOR_SIZE * RandomizeFloatBetweenValues(-0.5f, 0.5f);

	return star;
}