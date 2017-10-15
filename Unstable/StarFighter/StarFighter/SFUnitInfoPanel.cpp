#include "SFUnitInfoPanel.h"

extern Game* CurrentGame;

SFUnitInfoPanel::SFUnitInfoPanel(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip) : SFPanel(size, panel_type)
{
	m_playerShip = playerShip;
	m_title_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	m_text.setFont(*(*CurrentGame).m_font[Font_Arial]);

	//texts strings
	ostringstream ss_title;
	StockEntity* entity = NULL;
	if (playerShip->m_hovered_object)
	{
		entity = (StockEntity*)playerShip->m_hovered_object;
		if (entity->m_identified)
		{
			ss_title << entity->m_display_name;
		}
		else
		{
			ss_title << "???";
		}
		
	}
	else if (playerShip->m_selected_object)
	{
		entity = (StockEntity*)playerShip->m_selected_object;
		if (entity->m_identified)
		{
			ss_title << entity->m_display_name;
		}
		else
		{
			ss_title << "???";
		}
	}
	m_unit = entity;

	if (!m_unit)
	{
		return;
	}
	
	m_title_text.setString(ss_title.str());

	//text
	ostringstream ss_text;

	if (!entity->m_identified)
	{
		ss_text << "Unknown object";
	}
	else
	{
		//ore stocked
		ostringstream ss_ore;
		string contains_ore;

		for (map<string, size_t>::iterator i = entity->m_ore_stock.begin(); i != entity->m_ore_stock.end(); i++)
		{
			ss_ore << "\n" << i->first << ": " << i->second;
		}
		contains_ore = ss_ore.str();
		if (!contains_ore.empty())
		{
			ss_text << "Ore stock: " << entity->m_current_ore_stock << " / " << entity->GetOreMax();
			ss_text << contains_ore;//list of ores stocked
		}

		//fuel stocked
		ostringstream ss_fuel;
		string contains_fuel;

		for (map<string, size_t>::iterator i = entity->m_fuel_stock.begin(); i != entity->m_fuel_stock.end(); i++)
		{
			ss_fuel << "\n" << i->first << ": " << i->second;
		}
		contains_fuel = ss_fuel.str();
		if (!contains_fuel.empty())
		{
			ss_text << "\n\nFuel stock: " << entity->m_current_fuel_stock << " / " << entity->GetFuelMax();
			ss_text << contains_fuel;//list of ores stocked
		}

		//fuel stocked
		//if (entity->m_collider_type == StarshipObject)
		//{
		//	Starship* starship = (Starship*)entity;
		//	for (map<string, size_t>::iterator i = entity->m_fuel_stock.begin(); i != entity->m_fuel_stock.end(); i++)
		//	{
		//		//ss_fuel << "\n" << i->first << ": " << i->second + starship->m_fuel_assigned[i->first];
		//	}
		//}
		//else
		//{
		//	for (map<string, size_t>::iterator i = entity->m_fuel_stock.begin(); i != entity->m_fuel_stock.end(); i++)
		//	{
		//		ss_fuel << "\n" << i->first << ": " << i->second;
		//	}
		//}
		//
		//contains_fuel = ss_fuel.str();
		//if (!contains_fuel.empty())
		//{
		//	if (!contains_ore.empty())
		//	{
		//		ss_text << "\n\n";
		//	}
		//	ss_text << "Fuel: " << entity->m_current_fuel_stock << " / " << entity->GetFuelMax() << contains_fuel;
		//}
		
		//drill rates
		if (entity->m_collider_type == LocationObject)
		{
			ss_text << "\n\nRessources:";
			for (map<string, float>::iterator i = entity->m_ore_presence_rates.begin(); i != entity->m_ore_presence_rates.end(); i++)
			{
				ss_text << "\n" << i->first << ": " << i->second * 100 << ("%");
			}
		}

		//buildings
		if (entity->m_collider_type == LocationObject && entity->CanSupplyFuel())
		{
			Planet* planet = (Planet*)entity;
			size_t buildingsVectorSize = planet->m_buildings.size();
			ss_text << "\n\nBuildings: " << buildingsVectorSize << "/" << planet->m_building_slots;
			for (size_t i = 0; i < buildingsVectorSize; i++)
			{
				if (planet->m_buildings[i])
				{
					ss_text << "\n" << planet->m_buildings[i]->m_display_name;
				}
			}
		}

		//productions
		if (entity->m_collider_type == LocationObject && entity->CanSupplyFuel())
		{
			Planet* planet = (Planet*)entity;
			ss_text << "\n\nProductions:";
			for (map<string, size_t>::iterator i = planet->m_starship_productions.begin(); i != planet->m_starship_productions.end(); i++)
			{
				ss_text << "\n" << i->first << " (level " << i->second << ")";
			}
		}

		//ship stats
		if (entity->m_collider_type == StarshipObject)
		{
			Starship* starship = (Starship*)entity;
			
			ss_text << "\n\nMission: ";
			switch (starship->m_mission)
			{
				case StarshipMission_Idle:
				{
					ss_text << "Idle";
					break;
				}
					
				case StarshipMission_Drill:
				{
					ss_text << "Drill" << " (drill attempts : " << starship->m_drill_attempts << " / " << starship->m_nb_drills << ")";
					switch (starship->m_state)
					{
						case StarshipState_MovingToLocation:
						{
							ss_text << "\nMoving to task location";
							break;
						}

						case StarshipState_Drilling:
						{
							ss_text << "\nDrilling";
							break;
						}

						case StarshipState_Extracting:
						{
							ss_text << "\nExtracting";
							break;
						}

						case StarshipState_CarryToBase:
						{
							ss_text << "\nReturning to base location";
							break;
						}

						default:
							break;
					}
					break;
				}

				case StarshipMission_Scout:
				{
					sf::Vector2u zone_index = (*CurrentGame).m_stellarmap->GetZoneIndex(starship->m_current_destination_coordinates);
					ss_text << "Scout (target zone: " << zone_index.x << "-" << zone_index.y << ")";
					break;
				}
					
				case StarshipMission_Scan:
				{
					ss_text << "Scan";
					break;
				}
					
				default:
					break;
			}

			//fuel tank
			ss_text << "\n\nFuel tank:" << starship->m_fuel_tank.second << " / " << starship->m_fuel_tank_max;
			ss_text << "\n" << starship->m_fuel_tank.first << ": " << starship->m_fuel_tank.second;

			//	contains_fuel = ss_fuel.str();
			//if (!contains_fuel.empty())
			//{
			//	ss_text << "\n\nFuel stock: " << entity->m_current_fuel_stock << " / " << entity->GetFuelMax();
			//	ss_text << contains_fuel;//list of ores stocked
			//}
		}
	}

		//location
		//ss_text << "\n\nBase location: ";
		//if (starship->m_mission_base_location)
		//{
		//	ss_text << starship->m_mission_base_location->m_display_name;
		//}
		//else
		//{
		//	ss_text << "none";
		//}
		//
		//ss_text << "\nTask location: ";
		//if (starship->m_mission_task_location)
		//{
		//	ss_text << starship->m_mission_task_location->m_display_name;
		//}
		//else
		//{
		//	ss_text << "none";
		//}

	m_text.setString(ss_text.str());

	//options texts
	//m_actions = new SFActionBox((*CurrentGame).m_font[Font_Arial]);
	//m_actions->SetString("Enter", ActionButton_A);
	//m_actions->SetString("Hazard down", ActionButton_Y);
	//m_actions->SetString("Hazard up", ActionButton_X);
	//
	//m_actions_with_selection = new SFActionBoxWithSelection();
	//for (int i = 0; i < 4; i++)
	//{
	//	stringstream ss;
	//	ss << "Action " << i;
	//	m_actions_with_selection->AddOption(ss.str(), (*CurrentGame).m_font[Font_Arial]);
	//}

	sf::Vector2f position = sf::Vector2f(size.x / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().x / 2 + (*CurrentGame).m_view.getCenter().x, size.y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + (*CurrentGame).m_view.getCenter().y);
	//sf::Vector2f position = sf::Vector2f((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_view.getCenter().y);
	setPosition(position.x, position.y);

	//positioning of panel's content
	float text_height = 0;
	text_height += m_title_text.getGlobalBounds().height / 2;
	m_title_text.setPosition(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height);

	//texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	m_text.setPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//options texts
	text_height += INTERACTION_INTERBLOCK + m_title_text.getGlobalBounds().height;
	//m_actions->SetPosition(sf::Vector2f(getPosition().x - getSize().x / 2 + INTERACTION_PANEL_MARGIN_SIDES, getPosition().y - getSize().y / 2 + text_height));

	//dynamic size
	text_height += m_text.getGlobalBounds().height;
	setSize(sf::Vector2f(getSize().x, text_height));
	setOrigin(sf::Vector2f(getOrigin().x, text_height / 2));
	sf::Vector2f position_new = sf::Vector2f(getPosition().x, getSize().y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + (*CurrentGame).m_view.getCenter().y);
	setPosition(position_new.x, position_new.y);
}

SFUnitInfoPanel::~SFUnitInfoPanel()
{
	
}

void SFUnitInfoPanel::Update(sf::Time deltaTime)
{

}

void SFUnitInfoPanel::Draw(sf::RenderTexture& screen)
{
	if (m_visible)
	{
		SFPanel::Draw(screen);
		screen.draw(m_title_text);
		screen.draw(m_text);

		//if (m_actions)
		//{
		//	m_actions->Draw(screen);
		//}
	}
}

GameObject* SFUnitInfoPanel::GetUnit()
{
	return m_unit;
}