#ifndef HUNTITEM_H
#define HUNTITEM_H

#include <QObject>

class HuntItem : public QObject
{
    Q_OBJECT
public:
    explicit HuntItem(QObject *parent = nullptr);

signals:
};

#endif // HUNTITEM_H
