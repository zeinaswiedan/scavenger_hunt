#include "Player.h"
#include <QPainter>
#include <QBrush>

Player::Player(const QString &name, QPoint startPos, int size, int speed)
    : GameObject(startPos, size),
      name(name),
      oxygenLevel(100.0f),
      speed(speed),
      movingUp(false),
      movingDown(false),
      movingLeft(false),
      movingRight(false),
      mapWidth(800),
      mapHeight(600)
{}


QString Player::getName() const {
    return name;
}

void Player::setName(const QString &n) {
    name = n;
}


float Player::getOxygenLevel() const {
    return oxygenLevel;
}

void Player::depleteOxygen(float amount) {
    oxygenLevel -= amount;
    if (oxygenLevel < 0.0f)
        oxygenLevel = 0.0f;
}

void Player::resetOxygen(){
	oxygenLevel = 100.0f;
}


void Player::setMapBounds(int w, int h) {
    mapWidth = w;
    mapHeight = h;
}

void Player::setMoving(Qt::Key key, bool moving) {
    switch (key) {
    case Qt::Key_Up:    movingUp    = moving; break;
    case Qt::Key_Down:  movingDown  = moving; break;
    case Qt::Key_Left:  movingLeft  = moving; break;
    case Qt::Key_Right: movingRight = moving; break;
    default: break;
    }
}


void Player::update() {
    if (movingUp)    position.setY(position.y() - speed);
    if (movingDown)  position.setY(position.y() + speed);
    if (movingLeft)  position.setX(position.x() - speed);
    if (movingRight) position.setX(position.x() + speed);

    if (position.x() < 0) position.setX(0);
    if (position.y() < 0) position.setY(0);
    if (position.x() > mapWidth  - size) position.setX(mapWidth  - size);
    if (position.y() > mapHeight - size) position.setY(mapHeight - size);
}


void Player::draw(QPainter &painter, QPoint cameraOffset) {
    int screenX = position.x() - cameraOffset.x();
    int screenY = position.y() - cameraOffset.y();
    QPoint screenPos(screenX, screenY);

    painter.save();

    painter.setBrush(QBrush(Qt::cyan));
    painter.setPen(Qt::darkCyan);
    painter.drawEllipse(screenPos, size, size);

    painter.setPen(Qt::white);
    painter.drawText(screenX - size, screenY - size - 4, name);

    painter.restore();
}
