#include "CombatInterface.h"

extern Game* CurrentGame;

CombatInterface::CombatInterface()
{
	m_ship = NULL;
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
	m_lifebar = NULL;
	m_floodbar = NULL;
	m_crewbar = NULL;
	m_sonar = NULL;
	m_ship_name = NULL;
	m_ship_info = NULL;
}

CombatInterface::~CombatInterface()
{
	delete m_lifebar;
	delete m_floodbar;
	delete m_crewbar;
	delete m_sonar;
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
	delete m_sonar;
	delete m_ship_name;
	delete m_ship_info;
	m_lifebar = NULL;
	m_floodbar = NULL;
	m_crewbar = NULL;
	m_sonar = NULL;
	m_ship_name = NULL;
	m_ship_info = NULL;

	m_ship = NULL;
}

void CombatInterface::Init(Ship* ship, ShipAlliance alliance, string ship_name, ShipType ship_type)
{
	m_ship = ship;
	
	Texture* texture_crew = TextureLoader::getInstance()->loadTexture("2D/icon_crew.png", 30, 30);
	Texture* texture_sonar = TextureLoader::getInstance()->loadTexture("2D/icon_sonar.png", 30, 30);
	Texture* texture_combat = TextureLoader::getInstance()->loadTexture("2D/icon_combat.png", 30, 30);

	float offset = alliance == Alliance_Player ? COMBAT_LIFEBAR_OFFSET_X : COMBAT_LIFEBAR_ENEMY_OFFSET_X;

	//ship name & info
	m_ship_name = new GameEntity(UI_None);
	m_ship_name->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_ship_name->m_text.setCharacterSize(20);
	m_ship_name->m_text.setColor(sf::Color::Black);
	m_ship_name->m_text.setString(ship_name);
	m_ship_name->m_text.setPosition(sf::Vector2f(offset - m_ship_name->m_text.getGlobalBounds().width * 0.5f, COMBAT_INTERFACE_OFFSET_Y));

	m_ship_info = new GameEntity(UI_None);
	m_ship_info->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_ship_info->m_text.setCharacterSize(18);
	m_ship_info->m_text.setStyle(sf::Text::Italic);
	m_ship_info->m_text.setColor(sf::Color::Black);
	m_ship_info->m_text.setString((*CurrentGame).m_dico_ship_class[ship_type]);
	m_ship_info->m_text.setPosition(sf::Vector2f(offset - m_ship_info->m_text.getGlobalBounds().width * 0.5f, COMBAT_INTERFACE_OFFSET_Y + m_ship_info->m_text.getCharacterSize() + 8.f));
	
	//life bar
	m_lifebar = new GameEntity(UI_None);
	m_lifebar->m_shape_container.setSize(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape_container.setOrigin(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X * 0.5f, COMBAT_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape_container.setPosition(sf::Vector2f(offset, COMBAT_INTERFACE_OFFSET_Y + 50 + COMBAT_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_DarkGrey_Background]);
	m_lifebar->m_shape_container.setOutlineThickness(2.f);
	m_lifebar->m_shape_container.setOutlineColor(sf::Color::Black);
	sf::Vector2f lifebar_pos = m_lifebar->m_shape_container.getPosition();

	m_lifebar->m_shape.setSize(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X, COMBAT_LIFEBAR_SIZE_Y));
	m_lifebar->m_shape.setOrigin(sf::Vector2f(COMBAT_LIFEBAR_SIZE_X * 0.5f, COMBAT_LIFEBAR_SIZE_Y * 0.5f));
	m_lifebar->m_shape.setPosition(lifebar_pos);
	m_lifebar->m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_Green_Lifebar]);

	m_lifebar->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_lifebar->m_text.setCharacterSize(14);
	m_lifebar->m_text.setColor(sf::Color::Black);

	m_lifebar->m_position = lifebar_pos;

	//flood bar
	m_floodbar = new GameEntity(UI_None);
	m_floodbar->m_shape_container.setSize(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));
	m_floodbar->m_shape_container.setOrigin(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X * 0.5f, COMBAT_FLOODBAR_SIZE_Y * 0.5f));
	m_floodbar->m_shape_container.setPosition(sf::Vector2f(offset, m_lifebar->m_shape_container.getPosition().y + (COMBAT_LIFEBAR_SIZE_Y + COMBAT_FLOODBAR_SIZE_Y + m_lifebar->m_shape_container.getOutlineThickness()) * 0.5f));
	m_floodbar->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_DarkGrey_Background]);
	m_floodbar->m_shape_container.setOutlineThickness(2.f);
	m_floodbar->m_shape_container.setOutlineColor(sf::Color::Black);
	sf::Vector2f floodbar_pos = m_floodbar->m_shape_container.getPosition();

	m_floodbar->m_shape.setSize(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));
	m_floodbar->m_shape.setOrigin(sf::Vector2f(COMBAT_FLOODBAR_SIZE_X * 0.5f, COMBAT_FLOODBAR_SIZE_Y * 0.5f));
	m_floodbar->m_shape.setPosition(floodbar_pos);
	m_floodbar->m_shape.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Flood]);

	m_floodbar->m_position = floodbar_pos;

	//other infos
	const float spacing_x = 70.f;
	//crew bar
	m_crewbar = new GameEntity(UI_None);
	m_crewbar->setAnimation(texture_crew, 1, 1);
	m_crewbar->setPosition(sf::Vector2f(offset - COMBAT_LIFEBAR_SIZE_X * 0.5f + 16, floodbar_pos.y + COMBAT_LIFEBAR_SIZE_Y * 0.5f + 16));

	m_crewbar->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_crewbar->m_text.setCharacterSize(20);
	m_crewbar->m_text.setColor(sf::Color::White);
	m_crewbar->m_text.setPosition(sf::Vector2f(m_crewbar->getPosition().x + m_crewbar->m_size.x * 0.5 + 4, m_crewbar->getPosition().y - 14));

	//combat strength
	m_combatStrength = new GameEntity(UI_Combat);
	m_combatStrength->setAnimation(texture_combat, 1, 1);
	m_combatStrength->setPosition(sf::Vector2f(m_crewbar->m_text.getPosition().x + spacing_x + m_combatStrength->m_size.x * 0.5, m_crewbar->getPosition().y));

	m_combatStrength->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_combatStrength->m_text.setCharacterSize(20);
	m_combatStrength->m_text.setColor(sf::Color::White);
	m_combatStrength->m_text.setPosition(sf::Vector2f(m_combatStrength->getPosition().x + m_combatStrength->m_size.x * 0.5 + 2, m_crewbar->m_text.getPosition().y));

	//sonar range
	m_sonar = new GameEntity(UI_Sonar);
	m_sonar->setAnimation(texture_sonar, 1, 1);
	m_sonar->setPosition(sf::Vector2f(m_combatStrength->m_text.getPosition().x + spacing_x + m_sonar->m_size.x * 0.5, m_crewbar->getPosition().y));

	m_sonar->m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_sonar->m_text.setCharacterSize(20);
	m_sonar->m_text.setColor(sf::Color::White);
	m_sonar->m_text.setPosition(sf::Vector2f(m_sonar->getPosition().x + m_sonar->m_size.x * 0.5 + 2, m_crewbar->m_text.getPosition().y));

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

void CombatInterface::Update(int health, int health_max, int flood, int flood_max, int nb_crew, int nb_crew_max, int sonar, int estimatedCombatStrength)
{
	if (m_ship == NULL)
	{
		return;
	}

	float threshold[3] = { 0.7, 0.5, 0.3 };

	//size of life bar
	Bound(health, 0, health_max);

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
	Bound(flood, 0, flood_max);
	float flood_ratio = 1.0f * flood / flood_max;
	m_floodbar->m_shape.setSize(sf::Vector2f(flood_ratio * COMBAT_FLOODBAR_SIZE_X, COMBAT_FLOODBAR_SIZE_Y));

	//crew
	ostringstream ss_crew;
	ss_crew << nb_crew << "/" << nb_crew_max;
	m_crewbar->m_text.setString(ss_crew.str());

	//sonar
	if (sonar >= 0)
	{
		ostringstream ss_sonar;
		ss_sonar << sonar << "m";
		m_sonar->m_text.setString(ss_sonar.str());
	}

	//esimated combat strength
	{
		ostringstream ss_combat_strenth;
		ss_combat_strenth << estimatedCombatStrength;
		m_combatStrength->m_text.setString(ss_combat_strenth.str());
	}
	
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
	//name & info
	m_ship_name->Draw(screen);
	m_ship_info->Draw(screen);

	//life bars
	m_floodbar->Draw(screen);
	m_lifebar->Draw(screen);

	//other infos
	m_crewbar->Draw(screen);
	m_combatStrength->Draw(screen);
	m_sonar->Draw(screen);
	
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