#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <vector>

class QGraphicsScene;
class HuntItem;

class MapScene {
private:
    QGraphicsScene *scene;
    std::vector<HuntItem*> items;

public:
    MapScene(QGraphicsScene *scene);

    void generateMap();
};

#endif
