#include "CombatInterface.h"

extern Game* CurrentGame;

CombatInterface::CombatInterface()
{
	//m_distance_line = NULL;
	//
	//for (int i = 0; i < 5; i++)
	//{
	//	m_distance_markers[i] = NULL;
	//}
	//
	//for (int i = 0; i < 2; i++)
	//{
	//	m_distance_ships[i] = NULL;
	//	m_ships_name[i] = NULL;
	//	m_ships_info[i] = NULL;
	//}
}

CombatInterface::~CombatInterface()
{
	delete m_lifebar;
	delete m_floodbar;
	delete m_crewbar;
	delete m_ship_name;
	delete m_ship_info;
	//delete m_distance_line;
	//
	//for (int i = 0; i < 5; i++)
	//{
	//	delete m_distance_markers[i];
	//}
	//
	//for (int i = 0; i < 2; i++)
	//{
	//	delete m_distance_ships[i];
	//	delete m_ships_name[i];
	//	delete m_ships_info[i];
	//}
}

void CombatInterface::Destroy()
{
	delete m_lifebar;
	delete m_floodbar;
	delete m_crewbar;
	delete m_ship_name;
	delete m_ship_info;
}

void CombatInterface::Init(Ship* ship)
{
	m_ship = ship;
	
	Texture* texture = TextureLoader::getInstance()->loadTexture("2D/crew_icon.png", 32, 32);

	//life bar
	float offset = ship->m_alliance == Alliance_Player ? COMBAT_LIFEBAR_OFFSET_X : COMBAT_LIFEBAR_ENEMY_OFFSET_X;
	m_lifebar = new GameEntity(UI_None);
	m_lifebar->m_shape_container.setSize(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape_container.setOrigin(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X * 0.5f, COMBAT_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape_container.setPosition(sf::Vector2f(offset + COMBAT_LIFEBAR_SIZE_X * 0.5, COMBAT_LIFEBAR_OFFSET_Y - COMBAT_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_lifebar->m_shape_container.setOutlineThickness(2.f);
	m_lifebar->m_shape_container.setOutlineColor(sf::Color::Black);
	sf::Vector2f lifebar_pos = m_lifebar->m_shape_container.getPosition();

	m_lifebar->m_shape.setSize(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape.setOrigin(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X * 0.5f, COMBAT_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape.setPosition(lifebar_pos);
	m_lifebar->m_shape.setFillColor(sf::Color::Green);

	m_lifebar->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_lifebar->m_text.setCharacterSize(14);
	m_lifebar->m_text.setColor(sf::Color::Black);

	m_lifebar->m_position = lifebar_pos;

	//flood bar
	m_floodbar = new GameEntity(UI_None);
	m_floodbar->m_shape_container.setSize(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));
	m_floodbar->m_shape_container.setOrigin(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X * 0.5f, COMBAT_FLOODBAR_SIZE_Y * 0.5f));
	m_floodbar->m_shape_container.setPosition(sf::Vector2f(offset + COMBAT_FLOODBAR_SIZE_X * 0.5, m_lifebar->m_shape_container.getPosition().y + (COMBAT_LIFEBAR_SIZE_Y + COMBAT_FLOODBAR_SIZE_Y + m_lifebar->m_shape_container.getOutlineThickness()) * 0.5f));
	m_floodbar->m_shape_container.setFillColor(sf::Color(100, 100, 100, 255));//dark grey
	m_floodbar->m_shape_container.setOutlineThickness(2.f);
	m_floodbar->m_shape_container.setOutlineColor(sf::Color::Black);
	sf::Vector2f floodbar_pos = m_floodbar->m_shape_container.getPosition();

	m_floodbar->m_shape.setSize(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));
	m_floodbar->m_shape.setOrigin(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X * 0.5f, COMBAT_FLOODBAR_SIZE_Y * 0.5f));
	m_floodbar->m_shape.setPosition(floodbar_pos);
	m_floodbar->m_shape.setFillColor(sf::Color(0, 100, 170, 255));//blue "water"

	m_floodbar->m_position = floodbar_pos;

	//crew bar
	m_crewbar = new GameEntity(UI_None);
	m_crewbar->setAnimation(texture, 1, 1);
	m_crewbar->setPosition(sf::Vector2f(offset + 16, floodbar_pos.y + COMBAT_LIFEBAR_SIZE_Y * 0.5f + 16));

	m_crewbar->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_crewbar->m_text.setCharacterSize(20);
	m_crewbar->m_text.setColor(sf::Color::White);
	m_crewbar->m_text.setPosition(sf::Vector2f(m_crewbar->getPosition().x + 20, m_crewbar->getPosition().y - 14));

	//enemy iname & info
	m_ship_name = new GameEntity(UI_None);
	m_ship_name->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_ship_name->m_text.setCharacterSize(20);
	m_ship_name->m_text.setColor(sf::Color::Black);
	m_ship_name->m_text.setString(m_ship->m_display_name);
	m_ship_name->m_text.setPosition(sf::Vector2f(offset + COMBAT_LIFEBAR_SIZE_X * 0.5f - m_ship_name->m_text.getGlobalBounds().width * 0.5f, COMBAT_ENEMY_INFO_OFFSET_Y));

	m_ship_info = new GameEntity(UI_None);
	m_ship_info->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_ship_info->m_text.setCharacterSize(18);
	m_ship_info->m_text.setStyle(sf::Text::Italic);
	m_ship_info->m_text.setColor(sf::Color::Black);
	m_ship_info->m_text.setString((*CurrentGame).m_dico_ship_class[m_ship->m_type]);
	m_ship_info->m_text.setPosition(sf::Vector2f(offset + COMBAT_LIFEBAR_SIZE_X * 0.5f - m_ship_info->m_text.getGlobalBounds().width * 0.5f, COMBAT_ENEMY_INFO_OFFSET_Y + m_ship_info->m_text.getCharacterSize() + 8.f));
	

	//distance interface
	//m_distance_line = new GameEntity(UI_None);
	//m_distance_line->m_shape.setSize(sf::Vector2f(COMBAT_DISTANCE_SIZE_X, 4.f));
	//m_distance_line->m_shape.setOrigin(sf::Vector2f(COMBAT_DISTANCE_SIZE_X * 0.5f, 4.f * 0.5f));
	//m_distance_line->m_shape.setPosition(sf::Vector2f(COMBAT_DISTANCE_OFFSET_X, COMBAT_DISTANCE_OFFSET_Y));
	//m_distance_line->m_shape.setFillColor(sf::Color::Black);

	//for (int i = 0; i < 5; i++)
	//{
	//	m_distance_markers[i] = new GameEntity(UI_None);
	//	m_distance_markers[i]->m_shape.setSize(sf::Vector2f(4.f, 12.f));
	//	m_distance_markers[i]->m_shape.setOrigin(sf::Vector2f(4.f * 0.5f, 12.f * 0.5f));
	//	m_distance_markers[i]->m_shape.setPosition(sf::Vector2f(COMBAT_DISTANCE_OFFSET_X - (COMBAT_DISTANCE_SIZE_X * 0.5f) + (COMBAT_DISTANCE_SIZE_X * 0.25 * i), COMBAT_DISTANCE_OFFSET_Y));
	//	m_distance_markers[i]->m_shape.setFillColor(sf::Color::Black);
	//}
	
	//for (int i = 0; i < 2; i++)
	//{
	//	m_distance_ships[i] = new GameEntity(UI_None);
	//
	//	if (i == 0)
	//	{
	//		sf::Texture* texture = TextureLoader::getInstance()->loadTexture(ship->m_textureName, (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);
	//		m_distance_ships[i]->setAnimation(texture, 1, 2);
	//		m_distance_ships[i]->setAnimationLine(0);
	//		m_distance_ships[i]->setRotation(90.f);
	//
	//		m_distance_ships[i]->setPosition(sf::Vector2f(COMBAT_DISTANCE_OFFSET_X - (COMBAT_DISTANCE_SIZE_X * 0.5f) - (WATERTILE_SIZE * 0.75f), COMBAT_DISTANCE_OFFSET_Y));
	//	}
	//	else if (i == 1)
	//	{
	//		sf::Texture* texture = TextureLoader::getInstance()->loadTexture(enemy_ship->m_textureName, (int)WATERTILE_SIZE, (int)WATERTILE_SIZE * 2);
	//		m_distance_ships[i]->setAnimation(texture, 1, 2);
	//		m_distance_ships[i]->setAnimationLine(1);
	//		m_distance_ships[i]->setRotation(-90.f);
	//
	//		m_distance_ships[i]->setPosition(sf::Vector2f(COMBAT_DISTANCE_OFFSET_X + (COMBAT_DISTANCE_SIZE_X * 0.5f), COMBAT_DISTANCE_OFFSET_Y));
	//
	//		m_distance_ships[i]->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	//		m_distance_ships[i]->m_text.setCharacterSize(20);
	//		m_distance_ships[i]->m_text.setColor(sf::Color::Black);
	//		m_distance_ships[i]->m_text.setPosition(m_distance_ships[i]->m_position);
	//	}
	//}
}

void CombatInterface::Update(sf::Time deltaTime)
{
	float threshold[3] = { 0.7, 0.5, 0.3 };

	//size of life bar
	int health = m_ship->m_health;
	int health_max = m_ship->m_health_max;
	Bound(health, sf::Vector2i(0, health_max));

	float life_ratio = 1.0f * health / health_max;

	m_lifebar->m_shape.setSize(sf::Vector2f(life_ratio * COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));

	//life bar color
	if (life_ratio >= threshold[1])
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Green);
	}
	else if (life_ratio >= threshold[2])
	{
		m_lifebar->m_shape.setFillColor(sf::Color(255, 127, 39, 255));//orange "damaged"
	}
	else
	{
		m_lifebar->m_shape.setFillColor(sf::Color::Red);
	}

	ostringstream ss_life;
	ss_life << health << "/" << health_max;
	m_lifebar->m_text.setString(ss_life.str());
	m_lifebar->m_text.SetPosition(sf::Vector2f(m_lifebar->m_position.x, m_lifebar->m_position.y - 3.f));

	//size of flood bar
	int flood = m_ship->m_flood;
	int flood_max = m_ship->m_flood_max;
	Bound(flood, sf::Vector2i(0, flood_max));

	float flood_ratio = 1.0f * flood / flood_max;

	m_floodbar->m_shape.setSize(sf::Vector2f(flood_ratio * COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));

	//crew
	ostringstream ss_crew;
	int crew_nb = m_ship->m_nb_crew;
	int crew_nb_max = m_ship->m_nb_crew_max;
	ss_crew << crew_nb << "/" << crew_nb_max;
	m_crewbar->m_text.setString(ss_crew.str());
	

	//distance
	//float distance_ratio = m_enemy_ship->m_distance_combat / DISTANCE_COMBAT_INIT;
	//m_distance_ships[1]->m_position = sf::Vector2f(COMBAT_DISTANCE_OFFSET_X + ((distance_ratio - 0.5f) * COMBAT_DISTANCE_SIZE_X), COMBAT_DISTANCE_OFFSET_Y);
	//m_distance_ships[1]->setPosition(m_distance_ships[1]->m_position);
	//ostringstream ss_distance;
	//ss_distance << m_enemy_ship->m_distance_combat << "m";
	//m_distance_ships[1]->m_text.setString(ss_distance.str());
	//m_distance_ships[1]->m_text.setPosition(sf::Vector2f(m_distance_ships[1]->m_position.x - m_distance_ships[1]->m_text.getGlobalBounds().width * 0.5f, m_distance_ships[1]->m_position.y - WATERTILE_SIZE));
}

void CombatInterface::Draw(sf::RenderTexture& screen)
{
	if (m_ship->m_is_fleeing == false)
	{
		//name & info
		m_ship_name->Draw(screen);
		m_ship_info->Draw(screen);

		//life bar
		m_crewbar->Draw(screen);
		m_floodbar->Draw(screen);
		m_lifebar->Draw(screen);
	}
	

	//distance
	//m_distance_line->Draw(screen);
	//
	//for (int i = 0; i < 5; i++)
	//{
	//	m_distance_markers[i]->Draw(screen);
	//}
	//
	//for (int i = 0; i < 2; i++)
	//{
	//	m_distance_ships[i]->Draw(screen);
	//}
}