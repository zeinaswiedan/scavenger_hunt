#ifndef SHARK_H
#define SHARK_H
#include "GameObject.h"

class Shark : public GameObject {
public:
    // Y. Added default speed=2 (slower than before) and wobble phase for animation
    Shark(QPoint startPos, int size = 30, int speed = 2);

    void setTarget(QPoint playerPos);
    void update() override;
    void draw(QPainter &painter, QPoint cameraOffset) override;
    bool collidesWith(QPoint playerPos, int playerSize);

private:
    QPoint targetPosition;
    int    speed;
    // Y. Track heading angle so the body rotates to face movement direction
    float  angle;
    // Y. Tail-wag phase drives the animated fin oscillation
    float  wobble;

    static constexpr float WOBBLE_SPEED = 0.18f;
    static constexpr float TURN_RATE    = 0.05f;  // radians per frame
};
#endif
