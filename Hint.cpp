#include "Hint.h"
#include <QPainter>
#include <QBrush>
#include <QFont>
#include <cmath>

Hint::Hint(int sequenceIndex,
           const QString &message,
           QPoint position,
           HintType type,
           int size,
           int pointValue)
    : GameObject(position, size),
    sequenceIndex(sequenceIndex),
    message(message),
    type(type),
    active(false),
    collected(false),
    pointValue(pointValue),
    pulsePhase(0.0f)
{}

int      Hint::getSequenceIndex() const { return sequenceIndex; }
bool     Hint::isActive()         const { return active; }
bool     Hint::isCollected()      const { return collected; }
QString  Hint::getMessage()       const { return message; }
HintType Hint::getType()          const { return type; }

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
    int sx = position.x() - cameraOffset.x();
    int sy = position.y() - cameraOffset.y();
    QPoint screen(sx, sy);

    float pulse = static_cast<float>(size) + 4.0f * std::sin(pulsePhase);
    int r = static_cast<int>(pulse);

    painter.save();
    switch (type) {
    case HintType::Clue:         drawClue(painter, screen, r);         break;
    case HintType::Treasure:     drawTreasure(painter, screen, pulse); break;
    case HintType::OxygenBubble: drawOxygenBubble(painter, screen, r); break;
    }
    painter.restore();
}

void Hint::drawClue(QPainter &painter, QPoint screen, int r) const {
    // Outer glow
    painter.setBrush(QBrush(QColor(255, 215, 0, 90)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(screen, r + 8, r + 8);

    // Main orb
    painter.setBrush(QBrush(QColor(255, 215, 0)));
    painter.setPen(QPen(QColor(180, 140, 0), 2));
    painter.drawEllipse(screen, r, r);

    // "?" label
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(r);
    painter.setFont(font);
    painter.setPen(Qt::darkYellow);
    painter.drawText(QRect(screen.x() - r, screen.y() - r, r * 2, r * 2),
                     Qt::AlignCenter, "?");
}

void Hint::drawTreasure(QPainter &painter, QPoint screen, float pulse) const {
    int sx = screen.x();
    int sy = screen.y();
    int w  = 44 + static_cast<int>(4 * std::sin(pulse * 2));
    int h  = 30 + static_cast<int>(2 * std::sin(pulse * 2));

    // Radiant glow
    QRadialGradient glow(screen, w + 20);
    glow.setColorAt(0.0, QColor(255, 210, 30, 100));
    glow.setColorAt(1.0, QColor(255, 210, 30, 0));
    painter.setBrush(glow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(screen, w + 20, w + 20);

    // Chest body
    painter.setBrush(QColor(120, 75, 20));
    painter.setPen(QPen(QColor(80, 50, 10), 2));
    painter.drawRoundedRect(sx - w / 2, sy, w, h, 4, 4);

    // Chest lid
    painter.setBrush(QColor(150, 95, 30));
    painter.setPen(QPen(QColor(80, 50, 10), 2));
    painter.drawRoundedRect(sx - w / 2, sy - h / 2, w, h / 2 + 4, 4, 4);

    // Gold bands
    painter.setPen(QPen(QColor(210, 170, 40), 3));
    painter.drawLine(sx - w / 2, sy + h / 3, sx + w / 2, sy + h / 3);
    painter.drawLine(sx - w / 2, sy - 2,     sx + w / 2, sy - 2);

    // Lock
    painter.setBrush(QColor(220, 180, 30));
    painter.setPen(QPen(QColor(150, 110, 10), 2));
    painter.drawEllipse(sx - 7, sy - 4, 14, 14);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(150, 110, 10), 2));
    painter.drawArc(sx - 5, sy - 10, 10, 12, 0, 180 * 16);

    // Sparkling stars
    painter.setPen(QPen(QColor(255, 240, 120, 200), 1));
    for (int i = 0; i < 6; ++i) {
        float angle = pulse + i * (M_PI / 3.0f);
        int ex = sx + static_cast<int>((w * 0.8f) * std::cos(angle));
        int ey = sy + static_cast<int>((w * 0.8f) * std::sin(angle));
        painter.drawLine(ex - 3, ey, ex + 3, ey);
        painter.drawLine(ex, ey - 3, ex, ey + 3);
    }
}

void Hint::drawOxygenBubble(QPainter &painter, QPoint screen, int r) const {
    // Outer shimmer
    QRadialGradient shimmer(screen, r + 10);
    shimmer.setColorAt(0.0, QColor(80, 200, 255, 60));
    shimmer.setColorAt(1.0, QColor(80, 200, 255, 0));
    painter.setBrush(shimmer);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(screen, r + 10, r + 10);

    // Bubble
    QRadialGradient bubble(QPointF(screen.x() - r / 3, screen.y() - r / 3), r * 1.2f);
    bubble.setColorAt(0.0, QColor(180, 235, 255, 220));
    bubble.setColorAt(0.6, QColor(60, 170, 230, 160));
    bubble.setColorAt(1.0, QColor(20, 100, 200, 120));
    painter.setBrush(bubble);
    painter.setPen(QPen(QColor(120, 200, 255, 180), 2));
    painter.drawEllipse(screen, r, r);

    // O₂ label
    painter.setPen(QColor(255, 255, 255, 220));
    QFont f = painter.font();
    f.setBold(true);
    f.setPixelSize(r - 2);
    painter.setFont(f);
    painter.drawText(QRect(screen.x() - r, screen.y() - r, r * 2, r * 2),
                     Qt::AlignCenter, "O₂");
}
