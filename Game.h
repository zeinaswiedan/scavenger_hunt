#pragma once
#include "Player.h"
#include "Hint.h"
#include "GameStateManager.h"
#include <QList>
#include <QString>
#include <QPoint>

class Game {
public:
    Game(int mapWidth, int mapHeight, int screenWidth, int screenHeight);
    ~Game();

    void start(const QString &playerName);
    void reset();
    void update();
    void tickOxygen();       // called every second
    void setPlayerMoving(Qt::Key key, bool pressed);

    QPoint  getCameraOffset()    const;
    QString getCurrentMessage()  const;
    bool    hasActiveMessage()   const;
    void    dismissMessage();

    Player           *getPlayer()          const;
    QList<Hint *>     getHints()           const;
    GameStateManager *getStateManager()    const;
    int               getCurrentHintIndex() const;
    int               getScore()           const;
    int               getElapsedSeconds()  const;

private:
    void spawnHints();
    void updateCamera();
    void checkCollisions();
    bool checkWinCondition();
    bool checkLoseCondition();
    void onHintCollected(int index);
    void activateTreasureIfReady();

    Player           *player;
    QList<Hint *>     hints;      // clues only (index 0..N-1)
    Hint             *treasure;   // final treasure — separate
    GameStateManager *stateManager;

    int    mapWidth, mapHeight, screenWidth, screenHeight;
    QPoint cameraOffset;
    int    currentHintIndex;  // which clue is next (ignores oxygen bubbles)
    QString activeMessage;
    bool    messageActive;

    float oxygenDrainRate;   // % per second
    int   collectionRadius;

    int   score;
    int   elapsedSeconds;
    bool  treasureActive;
    bool  treasureCollected;
};
