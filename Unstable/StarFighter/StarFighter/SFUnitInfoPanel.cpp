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
		ss_title << entity->m_display_name;
	}
	else if (playerShip->m_selected_object)
	{
		entity = (StockEntity*)playerShip->m_selected_object;
		ss_title << entity->m_display_name;
	}
	m_unit = entity;

	if (!m_unit)
	{
		return;
	}
	
	m_title_text.setString(ss_title.str());

	//text
	ostringstream ss_text;

	ostringstream ss_ore;
	string contains_ore;
	ostringstream ss_fuel;
	string contains_fuel;

	//ore stocked
	for (map<string, size_t>::iterator i = entity->m_ores_stocked.begin(); i != entity->m_ores_stocked.end(); i++)
	{
		ss_ore << "\n" << i->first << ": " << i->second;
	}
	contains_ore = ss_ore.str();
	if (!contains_ore.empty())
	{
		ss_text << "Ore: " << entity->m_stock << " / " << entity->GetStockMax() << contains_ore;
	}

	//fuel
	if (entity->m_collider_type == StarshipObject)
	{
		Starship* starship = (Starship*)entity;
		for (map<string, size_t>::iterator i = entity->m_fuel_tanks.begin(); i != entity->m_fuel_tanks.end(); i++)
		{
			ss_fuel << "\n" << i->first << ": " << i->second + starship->m_fuel_assigned[i->first];
		}
	}
	else
	{
		for (map<string, size_t>::iterator i = entity->m_fuel_tanks.begin(); i != entity->m_fuel_tanks.end(); i++)
		{
			ss_fuel << "\n" << i->first << ": " << i->second;
		}
	}
	
	contains_fuel = ss_fuel.str();
	if (!contains_fuel.empty())
	{
		if (!contains_ore.empty())
		{
			ss_text << "\n\n";
		}
		ss_text << "Fuel: " << entity->m_fuel << " / " << entity->GetFuelMax() << contains_fuel;
	}

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

	//drill stats
	if (entity->m_collider_type == StarshipObject)
	{
		Starship* starship = (Starship*)entity;
		ss_text << "\n\nDrill attempts: " << starship->m_current_drill_attempts << "/" << starship->m_nb_drills;
		ss_text << "\nStatus: ";
		switch (starship->m_state)
		{
			case StarshipState_Idle:
			{
				if (starship->m_target_location)
				{
					ss_text << "waiting for fuel";
				}
				else
				{
					ss_text << "waiting for orders";
				}
				
				break;
			}
			case StarshipState_Drilling:
			{
				ss_text << "drilling...";
				break;
			}
			case StarshipState_Searching:
			{
				ss_text << "searching drill location";
				break;
			}
			case StarshipState_Unloading:
			{
				ss_text << "unloading";
				break;
			}
			case StarshipState_MovingToLocation:
			{
				ss_text << "moving to location";
				break;
			}
			case StarshipState_Extracting:
			{
				ss_text << "extracting: " << starship->m_ore_found->m_display_name;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	//distance
	if (playerShip->m_selected_object && playerShip->m_selected_object->m_collider_type == StarshipObject)
	{
		Starship* starship = (Starship*)playerShip->m_selected_object;
		GameObject* destination = entity != starship && entity->m_collider_type == LocationObject ? (GameObject*)entity : starship->m_target_location;

		size_t distance = GameObject::GetLightYearsBetweenObjects(playerShip->m_selected_object, destination);
		ss_text << "\n\nDistance: ";

		if (distance > 0)
		{
			ss_text << distance;
			if (playerShip->m_selected_object->m_collider_type == StarshipObject)
			{
				if (distance > 0)
				{
					Starship* starship = (Starship*)playerShip->m_selected_object;
					size_t distance_return = GameObject::GetLightYearsBetweenObjects(destination, starship->m_base_location);
					size_t propulsion_required = entity->CanSupplyFuel() ? distance : distance + distance_return;//prepare for a back and forth if destination cannot supply fuel
					size_t propulsion_missing = starship->m_propulsion + starship->m_propulsion_assigned > propulsion_required ? 0 : propulsion_required - starship->m_propulsion - starship->m_propulsion_assigned;
					ss_text << "\nPropulsion available: ";
					if (propulsion_missing == 0)
					{
						ss_text << starship->m_propulsion + starship->m_propulsion_assigned;
					}
					else
					{
						ss_text << propulsion_missing - starship->LoadRequiredPropulsion(starship->m_base_location, propulsion_missing, true);
					}
					ss_text << " / " << propulsion_required << " required";

					if (!starship->m_arrived_at_distination)
					{
						string current_fuel = starship->GetBestAssignedPropulsionAvailable();
						if (!current_fuel.empty())
						{
							ss_text << "\nCurrent fuel used: " << current_fuel << " (" << starship->m_fuel_assigned[current_fuel] << ")";
						}
					}
				}
			}
		}
		else
		{
			ss_text << "current location";
		}
	}

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

	sf::Vector2f position = sf::Vector2f(size.x / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().x / 2 + playerShip->getPosition().x, size.y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + playerShip->getPosition().y);
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
	sf::Vector2f position_new = sf::Vector2f(getPosition().x, getSize().y / 2 + getOutlineThickness() - (*CurrentGame).m_mainScreen.getSize().y / 2 + m_playerShip->getPosition().y);
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