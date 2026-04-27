#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

class HuntItem;

class GameManager {
private:
    int score;
    int collectedItems;

public:
    GameManager();

    void collectItem(HuntItem *item);

    int getScore() const;
    int getCollectedItems() const;
};

#endif
