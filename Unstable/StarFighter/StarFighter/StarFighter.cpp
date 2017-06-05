#include "StarFighter.h"

#ifdef WIN32
	#include "resource.h"
#endif

Game* CurrentGame;



int main()
{
	float win_ratio_expected = 0.4f;//calcul� en fonction de l'ELO des deux joueurs. Ici j'ai mis un 80% de gagner pour le joueur 1.
	srand(time(NULL));
	float match_result = ((double)rand() / (RAND_MAX));
	bool p1_wins_match = match_result <= win_ratio_expected;//on d�termine al�atoirement le gagnant du match, pour commencer : est-ce que le joueur 1 a gagn� le match ?

	int sets = 5;//nombre de sets max, selon les r�gles de la partie.
	int sets_to_win = sets / 2 + 1;
	int sets_winner_won = 0;
	int sets_loser_won = 0;
	int games_to_win = 6;//nombre de jeux � gagner dans le set, selon les r�gles de la partie.

	int games_p1[] = { 0, 0, 0, 0, 0 };//un tableau pour m�moriser le r�sultat de chaque set pour chaque joueur. J'ai fait un tableau de taille maximum (5 sets)
	int games_p2[5] = { 0, 0, 0, 0, 0 };

	int i = 0;//compteur de sets jou�s

	while (sets_winner_won < sets_to_win)//tant que le gagnant n'atteint pas le nombre de sets requis pour gagner le match...
	{
		int games_won_by_set_winner = games_to_win;//on fixe le score du gagnant du set comme �tant le nombre de jeux requis pour le gagner

		float set_result = ((double)rand() / (RAND_MAX));
		bool p1_wins_set = set_result <= win_ratio_expected;//on d�termine al�atoirement le gagnant du set : est-ce que le joueur 1 a gagn� le set ?
		if (sets_loser_won == sets_to_win - 1 && p1_wins_match != p1_wins_set)//on emp�che le perdant de gagner le set d�cisif en inversant le r�sultat du tirage al�atoire dans ce cas
		{
			set_result = 1 - set_result;
			p1_wins_set = p1_wins_match;
		}

		sets_winner_won += p1_wins_match ? p1_wins_set : !p1_wins_set;//ajouter un set � celui qui gagne le set
		sets_loser_won += p1_wins_match ? !p1_wins_set : p1_wins_set;

		float win_margin = abs(win_ratio_expected - set_result);//win_margin : mesure � quel point il a gagn� le set ; est-ce qu'il l'a �cras� ou pas (�cart par rapport au r�sultat attendu). En absolu car on connait d�j� le gagnant du set.
		int games_won_by_set_loser = games_to_win - (int)ceil(games_to_win * win_margin);//on d�termine le nombre de jeux marqu�s par le perdant du set au pro rata du win_margin.

		if (games_won_by_set_loser >= games_to_win - 1)//cas des victoires 7-5 ou 7-6, il faut donc donner un jeu de plus au gagnant
		{
			games_won_by_set_winner++;
		}

		games_p1[i] = p1_wins_set*games_won_by_set_winner + !p1_wins_set*games_won_by_set_loser;//on enregistre le r�sultat du set pour chaque joueur (si le joueur 1 a gagn� le set, on lui attribue le montant de jeux du gagnant du set, et vice-versa)
		games_p2[i] = !p1_wins_set*games_won_by_set_winner + p1_wins_set*games_won_by_set_loser;

		i++;
	}

	printf("score:\n%d %d %d %d %d\n%d %d %d %d %d", games_p1[0], games_p1[1], games_p1[2], games_p1[3], games_p1[4], games_p2[0], games_p2[1], games_p2[2], games_p2[3], games_p2[4]);//affichage du r�sultat final (j'affiche 0-0 pour les sets qui n'ont pas �t� jou�s pour faire vite, mais il ne faudra �videmment pas les afficher)

	//LOGGER_START(Logger::DEBUG, "");
	//
	////Load Configuration
	//LOGGER_WRITE(Logger::DEBUG, "Loading Configurations");
	//PREFS = new PrefsManager();
	//
	////Init SFML Window
	//LOGGER_WRITE(Logger::DEBUG, "Initializing SFML Window");
	sf::RenderWindow renderWindow(sf::VideoMode(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y), "Starfighter");
	renderWindow.setKeyRepeatEnabled(false);
	//
	////Refresh rate
	//renderWindow.setFramerateLimit(60);
	//
	////Icon
	sf::Image icon = sf::Image();
	icon.loadFromFile(makePath(ICON_SHIP_PNG));
	renderWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	//Title
	renderWindow.setTitle("StarFighter Engine");	

	//Main loop
	while (renderWindow.isOpen())
	{
		

		while (1)
		{

		}
		
		
	}

	return 0;
}

