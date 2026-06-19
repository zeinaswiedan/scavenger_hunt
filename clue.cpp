#include "clue.h"
#include <QBrush>

Clue::Clue()
{
    setRect(0, 0, 40, 40);
    setBrush(QBrush(Qt::red));
}
