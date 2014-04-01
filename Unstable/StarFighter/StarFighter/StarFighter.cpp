#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	sf::RenderWindow*	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );
	glEnable(GL_TEXTURE_2D);
	window->setKeyRepeatEnabled(false);

	LOGGER_START(Logger::Priority::DEBUG, "");
	LOGGER_WRITE(Logger::Priority::DEBUG, "Starting game");

	sf::Music SpaceCowboys;
	//if (!SpaceCowboys.openFromFile("Assets/Music/SpaceCowboys.ogg"))
	if (!SpaceCowboys.openFromFile("Assets/Music/CrimeaDigital.ogg"))
	//if (!SpaceCowboys.openFromFile("Assets/Music/Rebecca.ogg"))
		return -1; // erreur
	SpaceCowboys.play();

	CurrentGame = new Game();
	srand (time(NULL));


	//default setting
	ShipModel* shipModelDefault;
	shipModelDefault = new ShipModel();

	Equipment* airbrakeDefault;
	airbrakeDefault = new Equipment();
	airbrakeDefault->Init(EquipmentType::Airbrake, sf::Vector2f (0,0), 0.0f ,sf::Vector2f (0,0), 0, 0, 0, AIRBRAKE_FILENAME);

	Equipment* engineDefault;
	engineDefault = new Equipment();
	engineDefault->Init(EquipmentType::Engine, sf::Vector2f (0,0), 0.0f , sf::Vector2f (0,0), 0, 0, 0, THRUSTER_FILENAME);

	Equipment* armorDefault;
	armorDefault = new Equipment();
	armorDefault->Init(EquipmentType::Armor, sf::Vector2f (0,0), 0.0f , sf::Vector2f (0,0), 100, 0, 0, ARMOR_FILENAME);

	Equipment* shieldDefault;
	shieldDefault = new Equipment();
	shieldDefault->Init(EquipmentType::Shield, sf::Vector2f (0,0), 0.0f , sf::Vector2f (0,0), 0, 100, 100, SHIELD_FILENAME);

	Equipment* stabsDefault;
	stabsDefault = new Equipment();
	stabsDefault->Init(EquipmentType::Stabs, sf::Vector2f (0,0), 0.0f , sf::Vector2f (20,20), 0, 0, 0, STABS_FILENAME);

	Equipment* moduleDefault;
	moduleDefault = new Equipment();
	moduleDefault->Init(EquipmentType::Module, sf::Vector2f (0,0), 0.0f , sf::Vector2f (0,0), 0, 0, 0, MODULE_FILENAME);

	//bonus setting
	Equipment* airbrakeZ;
	airbrakeZ = new Equipment();
	airbrakeZ->Init(EquipmentType::Airbrake, sf::Vector2f (0,0), 300.0f , sf::Vector2f (0,0), 0, 0, 0, AIRBRAKE_Z_FILENAME);

	Equipment* engineZ;
	engineZ = new Equipment();
	engineZ->Init(EquipmentType::Engine, sf::Vector2f (400,400), 0.0f , sf::Vector2f (0,0), 0, 0, 0, THRUSTER_FILENAME);

	Equipment* emptyEquipment;
	emptyEquipment = new Equipment();
	emptyEquipment->Init(EquipmentType::Empty, sf::Vector2f (0,0), 0.0f , sf::Vector2f (0,0), 0, 0, 0, EMPTYSLOT_FILENAME);


	//ship config
	ShipConfig* shipA;
	shipA = new ShipConfig();
	//hardcoded for now, to be built in constructor or in Init() function...
	shipA->ship_model = shipModelDefault;
	shipA->equipment[Empty] = emptyEquipment;
	shipA->equipment[Airbrake] = airbrakeDefault;
	shipA->equipment[Engine] = engineDefault;
	shipA->equipment[Stabs] = stabsDefault;
	shipA->equipment[Armor] = armorDefault;
	shipA->equipment[Shield] = shieldDefault;
	shipA->equipment[Module] = moduleDefault;
	//...until this point
	shipA->Init(SHIP_FILENAME, sf::Vector2f(SHIP_WIDTH,SHIP_HEIGHT), SHIP_NB_FRAMES);

	//update
	sf::Time dt;
	sf::Clock deltaClock;

	//to erase later...
	sf::Clock deltaClockKeyPressed;
	bool keyrepeat = false;
	///...until here (avoiding key repetition)

	//Loading Scene
	Scene level1("Assets/Scripts/Scenes/scene1.csv",shipA);
	level1.StartGame(window);

	while (window->isOpen())
	{
		//Sampling
		if(deltaClock.getElapsedTime().asMilliseconds() < LOOP_SAMPLING_MS)
			continue;

		//to erase later...
		if (deltaClockKeyPressed.getElapsedTime() > sf::seconds(1))
		{
			keyrepeat = false;
		}
		// ... until here (avoiding key repeatition)

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}

		//to refactor later...
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
		{
			if (!keyrepeat)
			{
				level1.GetPlayerShip()->ship_config.setEquipment(engineZ);
				printf ("Engine Z mounted.\n");
				keyrepeat = true;
				deltaClockKeyPressed.restart();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		{				
			if (!keyrepeat)
			{
				level1.GetPlayerShip()->ship_config.setEquipment(airbrakeZ);
				printf ("Airbrake Z mounted.\n");
				keyrepeat = true;
				deltaClockKeyPressed.restart();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
		{				
			if (!keyrepeat)
			{
				level1.GetPlayerShip()->ship_config.setEquipment(engineDefault);
				printf ("Engine default mounted.\n");
				keyrepeat = true;
				deltaClockKeyPressed.restart();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
		{			
			if (!keyrepeat)
			{
				level1.GetPlayerShip()->ship_config.setEquipment(airbrakeDefault);
				printf ("Airbrake default mounted.\n");
				keyrepeat = true;
				deltaClockKeyPressed.restart();
			}
		}
		//...until here (test equipment)

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			window->close();

		dt = deltaClock.restart();
		level1.Update(dt);

	}

	return 0;
}