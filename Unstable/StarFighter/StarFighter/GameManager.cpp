#include "GameManager.h"

void GameManager::PushState(GameState& state, Player player)
{
	state.GameMgr = this;

	// set current state
	States.push(&state);
	States.top()->Initialize(player);
}

void GameManager::SetState(GameState& state, Player player)
{
	state.GameMgr = this;

	// Delete the actual current state (if any)
	PopState();

	// Add the new state
	PushState(state, player);
}

void GameManager::PopState()
{
	if (!States.empty())
	{
		States.top()->Release();
		States.pop();
	}
}