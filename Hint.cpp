#include "Hint.h"
#include <QPainter>
#include <QBrush>
#include <QFont>
#include <QRadialGradient>
#include <cmath>

Hint::Hint(QPoint position, int sequenceIndex, const QString &message, bool treasure)
    : GameObject(position, 18),
    sequenceIndex(sequenceIndex),
    message(message),
    active(false),
    collected(false),
    Treasure(treasure),
    pulsePhase(0.0f)
{}

int     Hint::getSequenceIndex() const { return sequenceIndex; }
bool    Hint::isActive()         const { return active; }
bool    Hint::isCollected()      const { return collected; }
QString Hint::getMessage()       const { return message; }
bool    Hint::isTreasure()       const { return Treasure; }

void Hint::activate() {
    if (!collected) { active = true; pulsePhase = 0.0f; }
}

void Hint::collect() {
    if (active && !collected) { active = false; collected = true; }
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
    float pulse = static_cast<float>(size) + 4.0f * std::sin(pulsePhase);
    int r = static_cast<int>(pulse);

    painter.save();
    // Y. Route to the right visual based on whether this is the final treasure
    if (Treasure)
        drawTreasureChest(painter, QPoint(sx, sy), pulse);
    else
        drawClue(painter, QPoint(sx, sy), r);
    painter.restore();
}

// Y. Original clue orb drawing extracted into its own method
void Hint::drawClue(QPainter &painter, QPoint screen, int r) const {
    painter.setBrush(QBrush(QColor(255, 215, 0, 120)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(screen, r + 4, r + 4);

    painter.setBrush(QBrush(QColor(255, 215, 0)));
    painter.setPen(QPen(QColor(180, 140, 0), 2));
    painter.drawEllipse(screen, r, r);

    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(r);
    painter.setFont(font);
    painter.setPen(Qt::darkYellow);
    painter.drawText(QRect(screen.x() - r, screen.y() - r, r * 2, r * 2),
                     Qt::AlignCenter, "?");
}

// Y. New treasure chest visual drawn for the final hint of every level
void Hint::drawTreasureChest(QPainter &painter, QPoint screen, float pulse) const {
    int sx = screen.x();
    int sy = screen.y();
    int w  = 44 + static_cast<int>(4 * std::sin(pulse * 2));
    int h  = 30 + static_cast<int>(2 * std::sin(pulse * 2));

    // Radiant golden glow
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

    // Orbiting sparkle stars
    painter.setPen(QPen(QColor(255, 240, 120, 200), 1));
    for (int i = 0; i < 6; ++i) {
        float angle = pulse + i * (static_cast<float>(M_PI) / 3.0f);
        int ex = sx + static_cast<int>((w * 0.8f) * std::cos(angle));
        int ey = sy + static_cast<int>((w * 0.8f) * std::sin(angle));
        painter.drawLine(ex - 3, ey, ex + 3, ey);
        painter.drawLine(ex, ey - 3, ex, ey + 3);
    }
}
