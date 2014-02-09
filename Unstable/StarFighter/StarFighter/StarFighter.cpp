#include "StarFighter.h"

Game* CurrentGame;

int main()
{
	sf::RenderWindow*	window = new sf::RenderWindow( sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter" );
	glEnable(GL_TEXTURE_2D);
	window->setKeyRepeatEnabled(false);

	sf::Music Rebecca;
	if (!Rebecca.openFromFile("Assets/Music/Rebecca.ogg"))
		return -1; // erreur
	//Rebecca.play();

	CurrentGame = new Game();
	//(*CurrentGame).init(window);

	//adding background
	//Independant* bg = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,+10),BACKGROUND_FILENAME,Vector2f(800,1800),Vector2f(0,1800-WINDOW_RESOLUTION_Y));
	//bg->setVisible(true);
	//(*CurrentGame).addToScene(bg,LayerType::BackgroundLayer,IndependantType::Background);

	srand (time(NULL));

	

	//default setting
	ShipModel* shipModelDefault;
	shipModelDefault = new ShipModel();

	Equipment* airbrakeDefault;
	airbrakeDefault = new Equipment();
	airbrakeDefault->Init(EquipmentType::Airbrake, sf::Vector2f (0,0), 0.0f , 0, 0, 0, AIRBRAKE_FILENAME);

	Equipment* engineDefault;
	engineDefault = new Equipment();
	engineDefault->Init(EquipmentType::Engine, sf::Vector2f (0,0), 0.0f , 0, 0, 0, AIRBRAKE_FILENAME);

	Equipment* armorDefault;
	armorDefault = new Equipment();
	armorDefault->Init(EquipmentType::Armor, sf::Vector2f (0,0), 0.0f , 100, 0, 0, AIRBRAKE_FILENAME);

	Equipment* shieldDefault;
	shieldDefault = new Equipment();
	shieldDefault->Init(EquipmentType::Shield, sf::Vector2f (0,0), 0.0f , 0, 100, 300, AIRBRAKE_FILENAME);

	Equipment* stabsDefault;
	stabsDefault = new Equipment();
	stabsDefault->Init(EquipmentType::Stabs, sf::Vector2f (0,0), 0.0f , 0, 0, 0, AIRBRAKE_FILENAME);

	//bonus setting
	Equipment* airbrakeZ;
	airbrakeZ = new Equipment();
	airbrakeZ->Init(EquipmentType::Airbrake, sf::Vector2f (0,0), 300.0f , 0, 0, 0, AIRBRAKE_FILENAME);

	Equipment* engineZ;
	engineZ = new Equipment();
	engineZ->Init(EquipmentType::Engine, sf::Vector2f (400,400), 0.0f , 0, 0, 0, AIRBRAKE_FILENAME);

	Equipment* emptyEquipment;
	emptyEquipment = new Equipment();
	emptyEquipment->Init(EquipmentType::Empty, sf::Vector2f (0,0), 0.0f , 0, 0, 0, AIRBRAKE_FILENAME);

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
	//...until this point
	shipA->Init(sf::Vector2f(SHIP_MAX_SPEED_X, SHIP_MAX_SPEED_Y), SHIP_DECCELERATION_COEF, SHIP_FILENAME, sf::Vector2f(SHIP_WIDTH,SHIP_HEIGHT), SHIP_NB_FRAMES);

	//Ship
	//Ship myShip(Vector2f(400,500),*shipA);
	//(*CurrentGame).addToScene(&myShip,LayerType::PlayerShipLayer, IndependantType::PlayerShip);

	//update
	sf::Time dt;
	sf::Clock deltaClock;

	//to erase later...
	sf::Clock deltaClockKeyPressed;
	bool keyrepeat = false;
	///...until here (avoiding key repetition)

	

	bool enemy_spawn = true;

	Scene level1("Assets/Scripts/Scenes/scene1.csv",shipA);
	level1.StartGame(window);

	int enemyGeneration = 0;
	while (window->isOpen())
	{

		//to erase later...
		if (deltaClockKeyPressed.getElapsedTime() > sf::seconds(1))
		{
			keyrepeat = false;
		}
		// ... until here (avoiding key repeatition)


		bool moving = false;
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

		//(*CurrentGame).updateScene(dt);
		//myShip.ship_hud.update(myShip.getIndependantArmor(), myShip.getIndependantShield());
		//myShip.Update(dt);

		//display
		//window->clear();

		//(*CurrentGame).drawScene();
		//window->draw(myShip.ship_hud.armorBar);
		//window->draw(myShip.ship_hud.shieldBar);
		//window->draw(hud.armorBar);
		//window->draw(hud.shieldBar);
		//window->draw(myShip);

 		//window->display();
	}

	return 0;
}