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
    void    refillOxygen(float amount);
    void    setMapBounds(int w, int h);
    void    setMoving(Qt::Key key, bool moving);
    bool    isMoving() const;
    void    update() override;
    void    draw(QPainter &painter, QPoint cameraOffset) override;

private:
    QString name;
    float   oxygenLevel;
    int     speed;
    bool    movingUp, movingDown, movingLeft, movingRight;
    int     mapWidth, mapHeight;

    // Trail effect
    struct TrailPoint { QPoint pos; int age; };
    QList<TrailPoint> trail;
    int frameCount;

    // Direction for drawing diver facing
    float facingAngle; // radians
};
#endif
