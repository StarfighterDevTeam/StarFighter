#include "Scene.h"

extern Game* CurrentGame;

Scene::Scene(string name, ShipConfig* shipConf)
{
	TextureLoader *loader;
	loader = TextureLoader::getInstance ();
	loader->loadAll();

	//Logger::Log(Logger::Priority::DEBUG,"Loading Scene");
	LOGGER_WRITE(Logger::Priority::DEBUG,"Loading Scene");

	try {

		this->config = *(FileLoader(name));
		this->enemyConfig = *(FileLoader(ENEMY_FILE));
		this->weaponConfig = *(FileLoader(WEAPON_FILE));
		this->ammoConfig = *(FileLoader(AMMO_FILE));

		for (std::list<vector<string>>::iterator it = (this->config).begin(); it != (this->config).end(); it++)
		{
			if((*it)[0].compare("bg") == 0)
			{
				this->bg = new Independant(sf::Vector2f(0,0),sf::Vector2f(0,+10),(*it)[1],Vector2f(stoi((*it)[2]),stoi((*it)[3])),Vector2f(0,stoi((*it)[3])-WINDOW_RESOLUTION_Y));
				bg->setVisible(true);
			}

			if((*it)[0].compare("enemy") == 0)
			{
				EnemyBase* e = LoadEnemy((*it)[1],atof((*it)[2].c_str()),stoi((*it)[3]));
				this->enemies.push_back(*e);
			}
		}

		//Loading font for framerate
		//TODO : refactor this
		sf::Font* font = new sf::Font();
		if (!font->loadFromFile("Assets/Fonts/arial.ttf"))
		{
			// error
			//TODO: font loader
		}
		this->framerate = new sf::Text("00", *font, 15);
		this->framerate->setColor(sf::Color::Yellow);
		this->framerate->setStyle(sf::Text::Bold);
		this->framerate->setPosition(WINDOW_RESOLUTION_X-80,WINDOW_RESOLUTION_Y-30);

	}
	catch( const std::exception & ex ) {

		//An error occured
		LOGGER_WRITE(Logger::Priority::LERROR,ex.what());
	}

	//Player ship
	this->playerShip = new Ship(Vector2f(400,500),*shipConf);
}

void Scene::StartGame(sf::RenderWindow*	window)
{
	this->mainWindow = window;
	(*CurrentGame).init(window);

	//bg
	(*CurrentGame).addToScene(bg,LayerType::BackgroundLayer,IndependantType::Background);

	//ship
	(*CurrentGame).addToScene(playerShip,LayerType::PlayerShipLayer, IndependantType::PlayerShip);
}

void Scene::Update(Time deltaTime)
{
	this->GenerateEnemies(deltaTime);

	(*CurrentGame).updateScene(deltaTime);
	mainWindow->clear();
	(*CurrentGame).drawScene();

	//TODO: refactor these
	mainWindow->draw(this->playerShip->ship_hud.armorBar);
	mainWindow->draw(this->playerShip->ship_hud.shieldBar);


	//Show framerate
	this->framerate->setString(TextUtils::format("fps=%.0f", 1 / (deltaTime.asMilliseconds() * 0.001)));
	mainWindow->draw(*(this->framerate));

	mainWindow->display();
}

Ship* Scene::GetPlayerShip()
{
	return this->playerShip;
}


EnemyBase* Scene::LoadEnemy(string name, float probability, int poolSize)
{
	for (std::list<vector<string>>::iterator it = (this->enemyConfig).begin(); it != (this->enemyConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			EnemyBase* base = new EnemyBase;
			base->enemy = new Enemy(sf::Vector2f(0,0),sf::Vector2f(0,stoi((*it)[11])),(*it)[7],sf::Vector2f(stoi((*it)[8]),stoi((*it)[9])));
			base->probability = probability;
			base->poolsize = poolSize;

			((Independant*)base->enemy)->armor = stoi((*it)[1]);
			((Independant*)base->enemy)->shield = ((Independant*)base->enemy)->shield_max = stoi((*it)[2]);
			((Independant*)base->enemy)->shield_regen = stoi((*it)[3]);
			((Independant*)base->enemy)->damage = stoi((*it)[4]);

			base->enemy->weapon = LoadWeapon((*it)[5], -1, LoadAmmo((*it)[6]));

			return base;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Enemy '%s'. Please check the config file",name));
}

Weapon* Scene::LoadWeapon(string name, int fire_direction, Ammo* ammo)
{
	for (std::list<vector<string>>::iterator it = (this->weaponConfig).begin(); it != (this->weaponConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			Weapon* weapon = new Weapon(ammo);
			weapon->fire_direction = Vector2i(0,fire_direction);
			weapon->rate_of_fire = atof((*it)[1].c_str());

			return weapon;
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Weapon '%s'. Please check the config file",name));

}

Ammo* Scene::LoadAmmo(string name)
{
	for (std::list<vector<string>>::iterator it = (this->ammoConfig).begin(); it != (this->ammoConfig).end(); it++)
	{
		if((*it)[0].compare(name) == 0)
		{
			return new Ammo(Vector2f(0,0), Vector2f(0,stoi((*it)[2])), (*it)[3], Vector2f(stoi((*it)[4]),stoi((*it)[4])), stoi((*it)[1]));
		}
	}

	throw invalid_argument(TextUtils::format("Config file error: Unable to find Ammo '%s'. Please check the config file",name));

}

list<vector<string>>* Scene::FileLoader(string name)
{

	list<vector<string>>* fileConfig = new list<vector<string>>;

	std::ifstream  data(name);

	std::string line;
	while(std::getline(data,line))
	{

		std::stringstream  lineStream(line);
		std::string        cell;

		std::vector<string> parameters;
		while(std::getline(lineStream,cell,','))
		{
			if(cell[0] == '#')
				goto loop_end;

			parameters.push_back(cell);
		}

		//TODO: check if not empty
		fileConfig->push_back(parameters);
loop_end:;
	}
	data.close();

	return fileConfig;
}

void Scene::GenerateEnemies(Time deltaTime)
{
	static double timer = 0;
	timer += deltaTime.asSeconds();
	if(timer > 1)
	{
		double intpart;
		timer = modf (timer , &intpart);

		double random_number = ((double) rand() / (RAND_MAX));

		for (std::list<EnemyBase>::iterator it = (this->enemies).begin(); it != (this->enemies).end(); it++)
		{
			if(it->probability > random_number && it->poolsize > 0)
			{
				//spawn (where on screen ?)
				Enemy* n = it->enemy->Clone();
				n->setPosition(rand() % WINDOW_RESOLUTION_X,-n->m_size.y*2);
				n->setVisible(true);
				it->poolsize--;

				LOGGER_WRITE(Logger::Priority::DEBUG, TextUtils::format("spawning enemy '%s' (x=%.1f) [pool=%d]",it->enemy->getName().c_str(),n->getPosition().x,it->poolsize));

				(*CurrentGame).addToScene((Independant*)n,LayerType::EnemyObjectLayer,IndependantType::EnemyObject);
			}
		}
	}

}