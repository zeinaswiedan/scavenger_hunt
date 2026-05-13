
#pragma once
#include "Player.h"
#include "Hint.h"
#include "Shark.h"
#include "GameStateManager.h"
#include <QList>
#include <QString>
#include <QPoint>
#include "LevelConfig.h"
#include "LevelData.h"

    class Game {
    public:
        Game(int screenWidth, int screenHeight);
        ~Game();

        void start(const QString &name, int levelNum);
        void reset();
        void update();
        void tickOxygen();
        void setPlayerMoving(Qt::Key key, bool pressed);

        QPoint  getCameraOffset()     const;
        QString getCurrentMessage()   const;
        bool    hasActiveMessage()    const;
        void    dismissMessage();

        Player            *getPlayer()           const;
        QList<Shark *>     getSharks()           const;
        QList<Hint *>      getHints()            const;
        GameStateManager  *getStateManager()     const;
        int                getCurrentHintIndex() const;
        void               loadLevel(int levelNumber);
        int                getCurrentLevel()     const;
        int                getMapWidth()         const;
        int                getMapHeight()        const;
        QString            getLevelName()        const;
        QString            getIntroText()        const;
        int                getTorchRadius()      const;
        const LevelConfig& getActiveConfig() const;

        // Y. Added score and elapsed-time getters for the HUD
        int  getScore()          const;
        int  getElapsedSeconds() const;

    private:
        void spawnHints();
        void updateCamera();
        void checkCollisions();
        bool checkWinCondition();
        bool checkLoseCondition();
        void onHintCollected(int index);

        Player           *player;
        QList<Shark *>    sharks;
        QList<Hint *>     hints;
        GameStateManager *stateManager;
        QString           playerName;

        int    mapWidth, mapHeight, screenWidth, screenHeight;
        QPoint cameraOffset;
        int    currentHintIndex;
        QString activeMessage;
        bool    messageActive;

        float oxygenDrainRate;
        int   collectionRadius;
        int   currentLevel;
        LevelConfig activeConfig;

        // Y. Track score (points per clue) and elapsed seconds (for the timer)
        int score;
        int elapsedSeconds;
    };

