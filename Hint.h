#ifndef HINT_H
#define HINT_H
#include "GameObject.h"
#include <QString>

enum class HintType {
    Clue,       // normal sequential clue
    Treasure,   // final treasure chest — unlocked after all clues collected
    OxygenBubble// oxygen refill pickup
};

class Hint : public GameObject {
public:
    Hint(int sequenceIndex,
         const QString &message,
         QPoint position,
         HintType type  = HintType::Clue,
         int size       = 15,
         int pointValue = 10);

    int      getSequenceIndex() const;
    bool     isActive()         const;
    bool     isCollected()      const;
    QString  getMessage()       const;
    HintType getType()          const;

    void activate();
    void collect();
    void update() override;
    void draw(QPainter &painter, QPoint cameraOffset) override;

private:
    void drawClue(QPainter &painter, QPoint screen, int r) const;
    void drawTreasure(QPainter &painter, QPoint screen, float pulse) const;
    void drawOxygenBubble(QPainter &painter, QPoint screen, int r) const;

    int      sequenceIndex;
    QString  message;
    HintType type;
    bool     active;
    bool     collected;
    int      pointValue;
    float    pulsePhase;
    static constexpr float PULSE_SPEED = 0.08f;
};
#endif
