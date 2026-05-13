#ifndef PLAYER_H
#define PLAYER_H
#include "GameObject.h"
#include <QString>
#include <Qt>
#include <QList>

class Player : public GameObject {
public:
    Player(const QString &name = "Player",
           QPoint startPos = QPoint(0, 0),
           int size  = 20,
           int speed = 4);

    QString getName()        const;
    void    setName(const QString &name);
    float   getOxygenLevel() const;
    void    resetOxygen();
    void    depleteOxygen(float amount);
    // Y. Added refillOxygen so oxygen bubble pickups can top the player back up
    void    refillOxygen(float amount);
    void    setMapBounds(int w, int h);
    void    setMoving(Qt::Key key, bool moving);
    // Y. Added isMoving helper used by the swim-trail logic
    bool    isMoving() const;
    void    update() override;
    void    draw(QPainter &painter, QPoint cameraOffset) override;

private:
    QString name;
    float   oxygenLevel;
    int     speed;
    bool    movingUp, movingDown, movingLeft, movingRight;
    int     mapWidth, mapHeight;

    // Y. Trail effect: store recent positions and fade them out as bubble dots
    struct TrailPoint { QPoint pos; int age; };
    QList<TrailPoint> trail;
    int frameCount;

    // Y. Track heading so the diver sprite faces the direction of travel
    float facingAngle; // radians
};
#endif
