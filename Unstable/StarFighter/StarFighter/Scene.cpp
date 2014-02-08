#include "Scene.h"


Scene::Scene(string name)
{

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
			Enemy* e = LoadEnemy((*it)[1],atof((*it)[2].c_str()));
			this->enemies.push_back(e);
		}
	}
}

Enemy* Scene::LoadEnemy(string name, float probability)
{
	for (std::list<vector<string>>::iterator it = (this->enemyConfig).begin(); it != (this->enemyConfig).end(); it++)
	{
		if((*it)[0].compare(name))
		{
			Enemy* e = new Enemy(sf::Vector2f(0,0),sf::Vector2f(0,stoi((*it)[10])),(*it)[6],sf::Vector2f(stoi((*it)[7]),stoi((*it)[8])),0);
			e->probability = probability;

			((Independant*)e)->armor = stoi((*it)[1]);
			((Independant*)e)->shield = ((Independant*)e)->shield_max = stoi((*it)[2]);
			((Independant*)e)->shield_regen = stoi((*it)[3]);
			((Independant*)e)->damage = stoi((*it)[4]);

			e->weapon = LoadWeapon((*it)[5],-1);

			return e;
		}
	}

	return NULL;
}

Weapon* Scene::LoadWeapon(string name, int fire_direction)
{
	for (std::list<vector<string>>::iterator it = (this->weaponConfig).begin(); it != (this->weaponConfig).end(); it++)
	{
		if((*it)[0].compare(name))
		{
			Weapon* weapon = new Weapon();

			weapon->fire_direction = Vector2i(0,fire_direction);
			weapon->rate_of_fire = atof((*it)[1].c_str());
			//weapon->damage = LASER_DAMAGE;
			weapon->ammunition = LoadAmmo((*it)[5]);
			return weapon;
		}
	}
	return NULL;
}

Ammo* Scene::LoadAmmo(string name)
{
	for (std::list<vector<string>>::iterator it = (this->ammoConfig).begin(); it != (this->ammoConfig).end(); it++)
	{
		return new Ammo(Vector2f(0,0), Vector2f(0,stoi((*it)[2])), (*it)[3], Vector2f(stoi((*it)[4]),stoi((*it)[4])), stoi((*it)[1]), 0);
	}
	return NULL;
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