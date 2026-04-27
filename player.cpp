#include "player.h"
#include <QBrush>
#include "clue.h"
#include <typeinfo>

Player::Player()
{
    health = 100;

    // make player look different (blue)
    setBrush(Qt::blue);
}


void Player::moveUp()
{
    moveBy(0, -10);
    QList<QGraphicsItem *> colliding = collidingItems();

    for (QGraphicsItem *item : colliding)
    {
        if (typeid(*item) == typeid(Clue))
        {
            delete item;

            health -= 10;

            if (health <= 0)
            {
                setBrush(QBrush(Qt::gray));
            }
        }
    }
}

void Player::moveDown()
{
    moveBy(0, 10);
    QList<QGraphicsItem *> colliding = collidingItems();

    for (QGraphicsItem *item : colliding)
    {
        if (typeid(*item) == typeid(Clue))
        {
            delete item;

            health -= 10;

            if (health <= 0)
            {
                setBrush(QBrush(Qt::gray));
            }
        }
    }
}

void Player::moveLeft()
{
    moveBy(-10, 0);
    QList<QGraphicsItem *> colliding = collidingItems();

    for (QGraphicsItem *item : colliding)
    {
        if (typeid(*item) == typeid(Clue))
        {
            delete item;

            health -= 10;

            if (health <= 0)
            {
                setBrush(QBrush(Qt::gray));
            }
        }
    }
}

void Player::moveRight()
{
    moveBy(10, 0);
    QList<QGraphicsItem *> colliding = collidingItems();

    for (QGraphicsItem *item : colliding)
    {
        if (typeid(*item) == typeid(Clue))
        {
            delete item;

            health -= 10;

            if (health <= 0)
            {
                setBrush(QBrush(Qt::gray));
            }
        }
    }
}
