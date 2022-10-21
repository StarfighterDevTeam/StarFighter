#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	(*CurrentGame).m_stellarmap = new StellarMap();

	//Loading scripts
	LoadCSVFiles();
	
	Player* player = new Player(sf::Vector2f(MAP_SIZE / 2, MAP_SIZE/2), sf::Vector2f(0, 0), "2D/Cursor.png", sf::Vector2f(32, 32), sf::Vector2f(16, 16), 1, 2);
	//player->m_visible = false;
	(*CurrentGame).m_player = player;
	(*CurrentGame).addToScene((*CurrentGame).m_player, PlayerLayer, PlayerObject);

	Planet* planet = new Planet(sf::Vector2f(MAP_SIZE / 2 - 300, MAP_SIZE/2 + 200), sf::Vector2f(0, 0), "2D/Planet1.png", sf::Vector2f(150, 150), sf::Vector2f(75, 75), 1);
	(*CurrentGame).addToScene(planet, LocationLayer, LocationObject);
	planet->m_ore_presence_rates["oil"] = 0.9f;
	planet->m_ore_presence_rates["deuterium"] = 0.1f;
	planet->Build("refinery", true);
	planet->Build("refinery", true);
	planet->Build("factory", true);
	planet->Build("ore_container", true);
	planet->Build("fuel_tanks", true);
	Starship* miner = planet->Produce("miner", true);
	miner->LoadFuelTank("oil", 10);
	planet->LoadInStock("oil", 150);
	planet->LoadInStock("deuterium", 30);
	planet->m_display_name = "Planète mère";
	planet->m_identified = true;

	Planet* planet2 = new Planet(sf::Vector2f(MAP_SIZE / 2 - 600, MAP_SIZE / 2 - 200), sf::Vector2f(0, 0), "2D/Planet1.png", sf::Vector2f(150, 150), sf::Vector2f(75, 75), 1);
	//(*CurrentGame).addToScene(planet2, LocationLayer, LocationObject);
	planet2->m_ore_presence_rates["oil"] = 0.5f;
	planet2->m_ore_presence_rates["deuterium"] = 0.5f;
	planet2->Build("refinery", true);
	planet2->Build("ore_container", true);
	planet2->Build("fuel_tanks", true);
	planet2->LoadInStock("oil", 50);
	planet2->LoadInStock("deuterium", 30);
	planet2->LoadInStock("iron", 30);
	planet2->LoadInStock("silver", 30);
	planet2->m_display_name = "Colonie";
	//planet2->Produce("probe", true);
	//planet2->Produce("probe", true);
	planet2->m_identified = false;

	OreField* ore_field = new OreField(sf::Vector2f(MAP_SIZE / 2 + 200, MAP_SIZE/2 + 200), sf::Vector2f(0, 0), "2D/Field1.png", sf::Vector2f(150, 150), sf::Vector2f(75, 75), 1);
	(*CurrentGame).addToScene(ore_field, LocationLayer, LocationObject);
	ore_field->m_ore_presence_rates["iron"] = 0.90f;
	ore_field->m_ore_presence_rates["silver"] = 0.10f;
	ore_field->m_display_name = "Champ de minerais";
	ore_field->m_identified = true;
	(*CurrentGame).m_stellarmap->SetZoneAsKnown(sf::Vector2u(4, 4));

	//OreField* ore_field2 = new OreField(sf::Vector2f(MAP_SIZE / 2 - 10, MAP_SIZE / 2 - 300), sf::Vector2f(0, 0), "2D/Field1.png", sf::Vector2f(150, 150), sf::Vector2f(75, 75), 1);
	//(*CurrentGame).addToScene(ore_field2, LocationLayer, LocationObject);
	//ore_field2->m_ore_presence_rates["gold"] = 0.30f;
	//ore_field2->m_ore_presence_rates["silver"] = 0.70f;
	//ore_field2->m_min_ore_weight = ore_field->GetLightestOreWeight();
	//ore_field2->m_display_name = "Champ de minerais rares";
	
	//Load saved file
	if (!Player::LoadPlayer(player))
	{
		//or create a new save file
		Player::SavePlayer(player);
	}

	//GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	//(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	(*CurrentGame).m_map_size = sf::Vector2f(MAP_SIZE, MAP_SIZE);
	(*CurrentGame).m_view.setCenter((*CurrentGame).m_player->getPosition());
	(*CurrentGame).m_player->SetControllerType(AllControlDevices);
}

void Gameloop::Update(sf::Time deltaTime)
{
	//move camera
	UpdateCamera(deltaTime);

	(*CurrentGame).updateScene(deltaTime);
	(*CurrentGame).m_stellarmap->update(deltaTime);

	//Create and destroy Unit info panel (HUD)
	//case 1: destroying a panel
	if ((*CurrentGame).m_player->m_is_asking_SFUnitPanel == false && (*CurrentGame).m_player->m_SFUnitInfoPanel)
	{
		DestroySFPanel(SFPanel_UnitInfoPanel, (*CurrentGame).m_player);
	}
	else if ((*CurrentGame).m_player->m_is_asking_SFUnitPanel == true)
	{
		//case 2: creating a panel
		if (!(*CurrentGame).m_player->m_SFUnitInfoPanel)
		{
			CreateSFPanel(SFPanel_UnitInfoPanel, (*CurrentGame).m_player);
		}
		//case 3: changing panel
		else// if ((*CurrentGame).m_player->m_SFUnitInfoPanel->m_panel_type != (*CurrentGame).m_player->m_is_asking_SFUnitPanel)
		{
			DestroySFPanel(SFPanel_UnitInfoPanel, (*CurrentGame).m_player);
			CreateSFPanel(SFPanel_UnitInfoPanel, (*CurrentGame).m_player);
		}
	}

	//Create and destroy Context info panel (HUD)
	//case 1: destroying a panel
	if ((*CurrentGame).m_player->m_is_asking_SFContextPanel_string.empty() && (*CurrentGame).m_player->m_SFContextInfoPanel)
	{
		DestroySFPanel(SFPanel_ContextInfoPanel, (*CurrentGame).m_player);
	}
	else if (!(*CurrentGame).m_player->m_is_asking_SFContextPanel_string.empty())
	{
		//case 2: creating a panel
		if (!(*CurrentGame).m_player->m_SFContextInfoPanel)
		{
			CreateSFPanel(SFPanel_ContextInfoPanel, (*CurrentGame).m_player);
		}
		//case 3: changing panel
		else// if ((*CurrentGame).m_player->m_SFUnitInfoPanel->m_panel_type != (*CurrentGame).m_player->m_is_asking_SFUnitPanel)
		{
			DestroySFPanel(SFPanel_ContextInfoPanel, (*CurrentGame).m_player);
			CreateSFPanel(SFPanel_ContextInfoPanel, (*CurrentGame).m_player);
		}
	}
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}

void Gameloop::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_player->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_player->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_view.getCenter().x;
	const float b = (*CurrentGame).m_view.getCenter().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

void Gameloop::DestroySFPanel(SFPanelTypes panel_type, Player* player)
{
	switch (panel_type)
	{
		case SFPanel_UnitInfoPanel:
		{
			(*CurrentGame).removeFromFeedbacks(player->m_SFUnitInfoPanel);
			delete player->m_SFUnitInfoPanel;
			player->m_SFUnitInfoPanel = NULL;
			break;
		}
		case SFPanel_ContextInfoPanel:
		{
			(*CurrentGame).removeFromFeedbacks(player->m_SFContextInfoPanel);
			delete player->m_SFContextInfoPanel;
			player->m_SFContextInfoPanel = NULL;
			break;
		}
	}
}

void Gameloop::CreateSFPanel(SFPanelTypes panel_type, Player* player)
{
	switch (panel_type)
	{
		case SFPanel_UnitInfoPanel:
		{
			player->m_SFUnitInfoPanel = new SFUnitInfoPanel(sf::Vector2f(SFPANEL_UNITINFO_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_UnitInfoPanel, player);
			(*CurrentGame).addToFeedbacks((*CurrentGame).m_player->m_SFUnitInfoPanel);
			break;
		}
		case SFPanel_ContextInfoPanel:
		{
			player->m_SFContextInfoPanel = new SFContextInfoPanel(sf::Vector2f(SFPANEL_CONTEXTINFO_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_ContextInfoPanel, player);
			(*CurrentGame).addToFeedbacks((*CurrentGame).m_player->m_SFContextInfoPanel);
			break;
		}
	}
	
}

void Gameloop::LoadCSVFiles()
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scripts.");

	//ore data
	vector<vector<string> > oreConfig = *(FileLoaderUtils::FileLoader(ORE_CSV_FILE));
	size_t oreVectorSize = oreConfig.size();
	for (size_t i = 0; i < oreVectorSize; i++)
	{
		(*CurrentGame).m_oreConfig.insert(std::map<string, vector<string> >::value_type(oreConfig[i][OreData_Name], oreConfig[i]));
	}
	oreConfig.clear();

	//building data
	vector<vector<string> > buildingConfig = *(FileLoaderUtils::FileLoader(BUILDING_CSV_FILE));
	size_t buildingVectorSize = buildingConfig.size();
	for (size_t i = 0; i < buildingVectorSize; i++)
	{
		(*CurrentGame).m_buildingConfig.insert(std::map<string, vector<string> >::value_type(buildingConfig[i][BuildingData_Name], buildingConfig[i]));
	}
	buildingConfig.clear();

	//starship data
	vector<vector<string> > starshipConfig = *(FileLoaderUtils::FileLoader(STARSHIP_CSV_FILE));
	size_t starshipVectorSize = starshipConfig.size();
	for (size_t i = 0; i < starshipVectorSize; i++)
	{
		(*CurrentGame).m_starshipConfig.insert(std::map<string, vector<string> >::value_type(starshipConfig[i][StarshipData_Name], starshipConfig[i]));
	}
	starshipConfig.clear();
}