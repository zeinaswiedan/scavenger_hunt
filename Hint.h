#ifndef HINT_H
#define HINT_H
#include "GameObject.h"
#include <QString>

class Hint : public GameObject {
public:
    Hint(QPoint position, int sequenceIndex, const QString &message,
         bool isTreasure = false);

    int     getSequenceIndex() const;
    bool    isActive()         const;
    bool    isCollected()      const;
    QString getMessage()       const;
    bool    isTreasure()       const;

    void activate();
    void collect();
    void update() override;
    void draw(QPainter &painter, QPoint cameraOffset) override;

private:
    // Y. Split drawing into two helpers so the logic is easy to follow
    void drawClue(QPainter &painter, QPoint screen, int r) const;
    void drawTreasureChest(QPainter &painter, QPoint screen, float pulse) const;

    int     sequenceIndex;
    QString message;
    bool    active;
    bool    collected;
    bool    Treasure;
    float   pulsePhase;
    static constexpr float PULSE_SPEED = 0.08f;
};
#endif
