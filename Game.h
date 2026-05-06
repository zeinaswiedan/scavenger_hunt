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
    	void tickOxygen();
        void setPlayerMoving(Qt::Key key, bool pressed);

   	QPoint getCameraOffset() const;

    	QString getCurrentMessage() const;
    	bool    hasActiveMessage()  const;
    	void    dismissMessage();

    	Player           *getPlayer()       const;
    	QList<Hint *>     getHints()        const;
    	GameStateManager *getStateManager() const;
    	int           getCurrentHintIndex() const;

private:
    	void spawnHints();
    	void updateCamera();
    	void checkCollisions();
    	bool checkWinCondition();
    	bool checkLoseCondition();

	void onHintCollected(int index);

	Player           *player;
    	QList<Hint *>     hints;
    	GameStateManager *stateManager;

    	int mapWidth;
    	int mapHeight;
    	int screenWidth;
    	int screenHeight;

    	QPoint cameraOffset;
       	int     currentHintIndex;
    	QString activeMessage;
    	bool    messageActive;
	float oxygenDrainRate;
	int collectionRadius;
};
