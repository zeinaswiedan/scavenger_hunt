#include "gamemanager.hpp"
#include "huntitem.hpp"

GameManager::GameManager() {
    score = 0;
    collectedItems = 0;
}

void GameManager::collectItem(HuntItem *item) {
    if (!item->isCollected()) {
        score += item->getPoints();
        collectedItems++;
        item->collect();
    }
}
