#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <QGraphicsScene>

class MapScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit MapScene(QObject *parent = nullptr);
};

#endif // MAPSCENE_H
