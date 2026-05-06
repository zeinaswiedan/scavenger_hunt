#include "GameStateManager.h"

GameStateManager::GameStateManager() : state(GameState::Menu) {}
void GameStateManager::setState(GameState newState){state = newState;}
GameState GameStateManager::getState() const{ return state; }
bool GameStateManager::isPlaying() const { return state == GameState::Playing;}
bool GameStateManager::isPaused()  const { return state == GameState::Paused;}
bool GameStateManager::isOver() const {
	return state == GameState::Win || state == GameState::Lose;
}
