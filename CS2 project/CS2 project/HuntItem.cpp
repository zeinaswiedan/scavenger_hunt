#include "HuntItem.h"
#include <QBrush>

HuntItem::HuntItem(QString name, int pts)
    : itemName(name), points(pts), collected(false) {
    setRect(0, 0, 30, 30);
    setBrush(Qt::yellow);
}

int HuntItem::getPoints() const {
    return points;
}

bool HuntItem::isCollected() const {
    return collected;
}

void HuntItem::collect() {
    collected = true;
    hide();
}
