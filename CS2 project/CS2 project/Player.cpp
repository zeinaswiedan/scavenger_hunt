#include "Player.h"
#include <QBrush>

Player::Player() {
    setRect(0, 0, 40, 40);
    setBrush(Qt::blue);
    setPos(50, 50);

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Player::keyPressEvent(QKeyEvent *event) {
    const int step = 10;

    if (event->key() == Qt::Key_Left)
        moveBy(-step, 0);

    else if (event->key() == Qt::Key_Right)
        moveBy(step, 0);

    else if (event->key() == Qt::Key_Up)
        moveBy(0, -step);

    else if (event->key() == Qt::Key_Down)
        moveBy(0, step);
}
