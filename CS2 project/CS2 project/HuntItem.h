#ifndef HUNTITEM_H
#define HUNTITEM_H

#include <QString>
#include <QGraphicsRectItem>

class HuntItem : public QGraphicsRectItem {
private:
    QString itemName;
    int points;
    bool collected;

public:
    HuntItem(QString name, int pts);

    int getPoints() const;
    bool isCollected() const;
    void collect();
};

#endif
