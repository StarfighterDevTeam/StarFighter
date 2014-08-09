#ifndef GAMEMANAGER_H_INCLUDED
#define GAMEMANAGER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include "Globals.h"
#include "GameState.h"
#include "Player.h"


class GameManager
{
public:

	/// Get the current state
	GameState* GetCurrentState() { return (!States.empty()) ? States.top() : NULL; };

	/// Add a new state ( become the current one )
	void PushState(GameState& state, Player player);

	/// Set the current state
	void SetState(GameState& state, Player player);

	/// Delete the current state
	void PopState();

private:

	// State manager for game states
	std::stack<GameState*> States;
};



#endif // GAMEMANAGER_H_INCLUDED