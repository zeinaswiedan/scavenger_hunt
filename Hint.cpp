#include "Hint.h"
#include <QPainter>
#include <QBrush>
#include <QFont>
#include <cmath>

Hint::Hint(int sequenceIndex,
           const QString &message,
           QPoint position,
           int size,
           int pointValue)
    : GameObject(position, size),
      sequenceIndex(sequenceIndex),
      message(message),
      active(false),
      collected(false),
      pointValue(pointValue),
      pulsePhase(0.0f)
{}

int Hint::getSequenceIndex() const {
    return sequenceIndex;
}

bool Hint::isActive() const {
    return active;
}

bool Hint::isCollected() const {
    return collected;
}

QString Hint::getMessage() const {
    return message;
}


void Hint::activate() {
    if (!collected) {
        active = true;
        pulsePhase = 0.0f;
    }
}

void Hint::collect() {
    if (active && !collected) {
        active    = false;
        collected = true;
    }
}

void Hint::update() {
    if (!active) return;
    pulsePhase += PULSE_SPEED;
    if (pulsePhase > 2.0f * static_cast<float>(M_PI))
        pulsePhase -= 2.0f * static_cast<float>(M_PI);
}

void Hint::draw(QPainter &painter, QPoint cameraOffset) {
    if (!active) return;
    int screenX = position.x() - cameraOffset.x();
    int screenY = position.y() - cameraOffset.y();
    QPoint screenPos(screenX, screenY);

    painter.save();

    float pulse = static_cast<float>(size) + 4.0f * std::sin(pulsePhase);
    int r = static_cast<int>(pulse);

    QColor glow(255, 215, 0, 120);
    painter.setBrush(QBrush(glow));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(screenPos, r + 4, r + 4);

    painter.setBrush(QBrush(QColor(255, 215, 0)));
    painter.setPen(QPen(QColor(180, 140, 0), 2));
    painter.drawEllipse(screenPos, r, r);

    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(r);
    painter.setFont(font);
    painter.setPen(Qt::darkYellow);
    painter.drawText(
        QRect(screenX - r, screenY - r, r * 2, r * 2),
        Qt::AlignCenter,
        "?"
    );

    painter.restore();
}
