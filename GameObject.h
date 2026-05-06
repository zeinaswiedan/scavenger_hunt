#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QPoint>
#include <QPainter>

class GameObject {
public:
    GameObject(QPoint position = QPoint(0, 0), int size = 10)
        : position(position), size(size) {}

    virtual ~GameObject() = default;

    QPoint getPosition() const { return position; }
    void setPosition(QPoint pos) { position = pos; }

    int getSize() const { return size; }

    virtual void update() = 0;
    virtual void draw(QPainter &painter, QPoint cameraOffset) = 0;

protected:
    QPoint position;
    int size;
};

#endif
