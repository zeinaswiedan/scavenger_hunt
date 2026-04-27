#include "mapscene.hpp"
#include "huntitem.hpp"

MapScene::MapScene(QGraphicsScene *scene)
    : scene(scene) {}

void MapScene::generateMap() {
    HuntItem *gem = new HuntItem("Gem", 10);
    gem->setPos(200, 150);

    HuntItem *key = new HuntItem("Key", 20);
    key->setPos(400, 300);

    scene->addItem(gem);
    scene->addItem(key);

    items.push_back(gem);
    items.push_back(key);
}
