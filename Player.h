#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <QString>
#include <Qt>

class Player : public GameObject {
public:
    Player(const QString &name = "Player",
           QPoint startPos = QPoint(0, 0),
           int size = 20,
           int speed = 5);

    QString getName() const;
    void setName(const QString &name);

    float getOxygenLevel() const;
    void resetOxygen();
    void depleteOxygen(float amount);

    void setMapBounds(int w, int h);

    void setMoving(Qt::Key key, bool moving);

    void update() override;
    void draw(QPainter &painter, QPoint cameraOffset) override;

private:
    QString name;
    float oxygenLevel;
    int speed;

    bool movingUp;
    bool movingDown;
    bool movingLeft;
    bool movingRight;

    int mapWidth;
    int mapHeight;
};

#endif
