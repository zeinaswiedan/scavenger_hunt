#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

class Player;
class GameManager;
class MapScene;

class GameWindow : public QMainWindow {
    Q_OBJECT

private:
    QGraphicsScene *scene;
    QGraphicsView *view;

    Player *player;
    GameManager *manager;
    MapScene *map;

public:
    GameWindow(QWidget *parent = nullptr);
};

#endif
