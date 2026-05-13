#include "Shark.h"
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QLinearGradient>
#include <cmath>

Shark::Shark(QPoint startPos, int size, int speed)
    : GameObject(startPos, size),
    speed(speed),
    targetPosition(startPos),
    angle(0.0f),
    wobble(0.0f)
{}

void Shark::setTarget(QPoint playerPos) {
    targetPosition = playerPos;
}

// Y. Replaced straight-line snap movement with smooth turning via angle lerp
void Shark::update() {
    wobble += WOBBLE_SPEED;
    if (wobble > 2.0f * static_cast<float>(M_PI))
        wobble -= 2.0f * static_cast<float>(M_PI);

    float dx = static_cast<float>(targetPosition.x() - position.x());
    float dy = static_cast<float>(targetPosition.y() - position.y());
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.0f) return;

    float desired = std::atan2(dy, dx);
    float diff = desired - angle;
    while (diff >  static_cast<float>(M_PI)) diff -= 2.0f * static_cast<float>(M_PI);
    while (diff < -static_cast<float>(M_PI)) diff += 2.0f * static_cast<float>(M_PI);
    angle += qBound(-TURN_RATE, diff, TURN_RATE);

    position.setX(static_cast<int>(position.x() + speed * std::cos(angle)));
    position.setY(static_cast<int>(position.y() + speed * std::sin(angle)));
}

// Y. Replaced red circle with a detailed shark body: streamlined hull,
//    dorsal fin, pectoral fins, animated tail, eye with glint, gills,
//    and a danger glow that intensifies as it closes in on the player
void Shark::draw(QPainter &painter, QPoint cameraOffset) {
    int sx = position.x() - cameraOffset.x();
    int sy = position.y() - cameraOffset.y();

    // Off-screen cull
    if (sx < -120 || sx > 920 || sy < -120 || sy > 720) return;

    painter.save();
    painter.translate(sx, sy);
    painter.rotate(static_cast<double>(angle) * 180.0 / M_PI);

    float wag = std::sin(wobble) * 6.0f;

    // Tail
    painter.setBrush(QColor(70, 80, 95));
    painter.setPen(Qt::NoPen);
    QPainterPath tail;
    tail.moveTo(-size, static_cast<int>(wag));
    tail.lineTo(-size - size * 0.7,  -size / 2);
    tail.lineTo(-size - size * 0.9,  static_cast<int>(wag));
    tail.lineTo(-size - size * 0.7,   size / 2);
    tail.closeSubpath();
    painter.drawPath(tail);

    // Body gradient (dark back, pale belly)
    QLinearGradient bodyGrad(-size, -size / 2, -size, size / 2);
    bodyGrad.setColorAt(0.0, QColor(80, 95, 115));
    bodyGrad.setColorAt(0.5, QColor(100, 115, 135));
    bodyGrad.setColorAt(1.0, QColor(200, 200, 210));
    painter.setBrush(bodyGrad);
    painter.setPen(QPen(QColor(50, 60, 75), 1));
    QPainterPath body;
    body.moveTo(size, 0);
    body.cubicTo( size * 0.6, -size * 0.45,
                 -size * 0.2, -size * 0.42,
                 -size,        static_cast<int>(wag * 0.3));
    body.cubicTo(-size * 0.2,  size * 0.42,
                 size * 0.6,  size * 0.45,
                 size, 0);
    painter.drawPath(body);

    // Dorsal fin
    painter.setBrush(QColor(65, 78, 95));
    painter.setPen(Qt::NoPen);
    QPainterPath dorsal;
    dorsal.moveTo( size * 0.1,  -size * 0.35);
    dorsal.lineTo(-size * 0.2,  -size * 0.85);
    dorsal.lineTo(-size * 0.55, -size * 0.38);
    dorsal.closeSubpath();
    painter.drawPath(dorsal);

    // Pectoral fins
    painter.setBrush(QColor(72, 85, 102));
    QPainterPath finL;
    finL.moveTo( size * 0.05,  size * 0.32);
    finL.lineTo(-size * 0.3,   size * 0.75);
    finL.lineTo(-size * 0.6,   size * 0.38);
    finL.closeSubpath();
    painter.drawPath(finL);
    QPainterPath finR;
    finR.moveTo( size * 0.05, -size * 0.32);
    finR.lineTo(-size * 0.3,  -size * 0.75);
    finR.lineTo(-size * 0.6,  -size * 0.38);
    finR.closeSubpath();
    painter.drawPath(finR);

    // Eye
    painter.setBrush(QColor(10, 10, 15));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(QPointF(size * 0.62, -size * 0.14), 3.5, 3.0);
    painter.setBrush(QColor(200, 220, 255, 160));
    painter.drawEllipse(QPointF(size * 0.64, -size * 0.18), 1.2, 1.2);

    // Gills
    painter.setPen(QPen(QColor(55, 65, 80, 180), 1));
    for (int i = 0; i < 3; ++i) {
        float gx = size * (0.35f - i * 0.12f);
        painter.drawArc(QRectF(gx - 2, -size * 0.3, 5, size * 0.28), -30*16, 120*16);
    }

    painter.restore();

    // Y. Red danger glow visible in world space — fades in as shark approaches
    float dx2 = static_cast<float>(targetPosition.x() - position.x());
    float dy2 = static_cast<float>(targetPosition.y() - position.y());
    float dist = std::sqrt(dx2 * dx2 + dy2 * dy2);
    if (dist < 400.0f) {
        int alpha = static_cast<int>(45 * (1.0f - dist / 400.0f));
        QRadialGradient danger(QPointF(sx, sy), size * 2.5);
        danger.setColorAt(0.0, QColor(180, 30, 30, alpha));
        danger.setColorAt(1.0, QColor(180, 30, 30, 0));
        painter.setBrush(danger);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QPointF(sx, sy), size * 2.5, size * 2.5);
    }
}

bool Shark::collidesWith(QPoint playerPos, int playerSize) {
    int dx = playerPos.x() - position.x();
    int dy = playerPos.y() - position.y();
    int minDist = size + playerSize;
    return (dx * dx + dy * dy) <= (minDist * minDist);
}
