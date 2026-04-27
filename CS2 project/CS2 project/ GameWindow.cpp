#include "gamewindow.hpp"
#include "player.hpp"
#include "gamemanager.hpp"
#include "mapscene.hpp"

GameWindow::GameWindow(QWidget *parent) : QMainWindow(parent) {
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);

    setCentralWidget(view);

    player = new Player();
    scene->addItem(player);
}

GameWindow::GameWindow(QWidget *parent) : QMainWindow(parent) {
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);

    setCentralWidget(view);

    manager = new GameManager();

    player = new Player();
    scene->addItem(player);
}

GameWindow::GameWindow(QWidget *parent) : QMainWindow(parent) {
    map = new MapScene(scene);
    map->generateMap();
}
