#include "Player.h"
#include <QPainter>
#include <QBrush>
#include <cmath>

Player::Player(const QString &name, QPoint startPos, int size, int speed)
    : GameObject(startPos, size),
    name(name),
    oxygenLevel(100.0f),
    speed(speed),
    movingUp(false), movingDown(false),
    movingLeft(false), movingRight(false),
    mapWidth(800), mapHeight(600),
    frameCount(0),
    facingAngle(0.0f)
{}

QString Player::getName()        const { return name; }
void    Player::setName(const QString &n) { name = n; }
float   Player::getOxygenLevel() const { return oxygenLevel; }

void Player::depleteOxygen(float amount) {
    oxygenLevel -= amount;
    if (oxygenLevel < 0.0f) oxygenLevel = 0.0f;
}

void Player::refillOxygen(float amount) {
    oxygenLevel += amount;
    if (oxygenLevel > 100.0f) oxygenLevel = 100.0f;
}

void Player::resetOxygen() { oxygenLevel = 100.0f; }

void Player::setMapBounds(int w, int h) {
    mapWidth = w;
    mapHeight = h;
}

void Player::setMoving(Qt::Key key, bool moving) {
    switch (key) {
    case Qt::Key_Up:    case Qt::Key_W: movingUp    = moving; break;
    case Qt::Key_Down:  case Qt::Key_S: movingDown  = moving; break;
    case Qt::Key_Left:  case Qt::Key_A: movingLeft  = moving; break;
    case Qt::Key_Right: case Qt::Key_D: movingRight = moving; break;
    default: break;
    }
}

bool Player::isMoving() const {
    return movingUp || movingDown || movingLeft || movingRight;
}

void Player::update() {
    QPoint prev = position;

    if (movingUp)    position.setY(position.y() - speed);
    if (movingDown)  position.setY(position.y() + speed);
    if (movingLeft)  position.setX(position.x() - speed);
    if (movingRight) position.setX(position.x() + speed);

    // Clamp to map
    position.setX(qMax(0, qMin(position.x(), mapWidth  - size)));
    position.setY(qMax(0, qMin(position.y(), mapHeight - size)));

    // Update facing angle from movement direction
    int dx = position.x() - prev.x();
    int dy = position.y() - prev.y();
    if (dx != 0 || dy != 0)
        facingAngle = static_cast<float>(std::atan2(dy, dx));

    // Trail
    frameCount++;
    if (isMoving() && frameCount % 3 == 0)
        trail.prepend({ position, 0 });
    for (auto &t : trail) t.age++;
    while (!trail.isEmpty() && trail.last().age > 18)
        trail.removeLast();
}

void Player::draw(QPainter &painter, QPoint cameraOffset) {
    int sx = position.x() - cameraOffset.x();
    int sy = position.y() - cameraOffset.y();

    painter.save();

    // Draw bubble trail
    for (const auto &t : trail) {
        int tx = t.pos.x() - cameraOffset.x();
        int ty = t.pos.y() - cameraOffset.y();
        int alpha = qMax(0, 160 - t.age * 9);
        int bsize = qMax(1, 5 - t.age / 4);
        painter.setBrush(QColor(180, 230, 255, alpha));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPoint(tx, ty), bsize, bsize);
    }

    // Draw diver body oriented to movement
    painter.translate(sx, sy);
    painter.rotate(static_cast<double>(facingAngle) * 180.0 / M_PI);

    // Body (wetsuit — dark teal)
    painter.setBrush(QColor(20, 90, 110));
    painter.setPen(QPen(QColor(10, 60, 80), 1));
    painter.drawEllipse(QPoint(0, 0), size, static_cast<int>(size * 0.65));

    // Helmet dome
    painter.setBrush(QColor(180, 225, 240));
    painter.setPen(QPen(QColor(60, 160, 200), 1));
    painter.drawEllipse(QPoint(size / 2, 0), static_cast<int>(size * 0.45), static_cast<int>(size * 0.45));

    // Helmet visor tint
    painter.setBrush(QColor(100, 200, 255, 100));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPoint(size / 2 + 2, -1), static_cast<int>(size * 0.28), static_cast<int>(size * 0.28));

    // Oxygen tank on back
    painter.setBrush(QColor(200, 200, 210));
    painter.setPen(QPen(QColor(100, 100, 120), 1));
    painter.drawRoundedRect(-size - 8, -5, 10, 10, 3, 3);

    // Fins
    painter.setBrush(QColor(30, 170, 120));
    painter.setPen(Qt::NoPen);
    QPolygon finTop;
    finTop << QPoint(-size + 2, -size / 2)
           << QPoint(-size - 10, -size)
           << QPoint(-size / 2, -size / 3);
    painter.drawPolygon(finTop);
    QPolygon finBot;
    finBot << QPoint(-size + 2, size / 2)
           << QPoint(-size - 10, size)
           << QPoint(-size / 2, size / 3);
    painter.drawPolygon(finBot);

    painter.restore();

    // Name label above
    painter.setPen(QColor(200, 240, 255, 200));
    QFont f;
    f.setFamily("Courier New");
    f.setPointSize(8);
    f.setBold(true);
    painter.setFont(f);
    QFontMetrics fm(f);
    int tw = fm.horizontalAdvance(name);
    painter.drawText(sx - tw / 2, sy - size - 8, name);
}
