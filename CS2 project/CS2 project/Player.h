#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QKeyEvent>

class Player : public QObject, public QGraphicsRectItem {
    Q_OBJECT

public:
    Player();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif
