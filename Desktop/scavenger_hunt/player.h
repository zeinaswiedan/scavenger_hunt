#ifndef PLAYER_H
#define PLAYER_H

#include "gameobject.h"

class Player : public GameObject
{
public:
    Player();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

private:
    int health;
};

#endif // PLAYER_H
