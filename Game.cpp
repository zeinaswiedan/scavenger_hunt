#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <QDebug>

Game::Game(int mapWidth, int mapHeight, int screenWidth, int screenHeight)
    : mapWidth(mapWidth), mapHeight(mapHeight),
    screenWidth(screenWidth), screenHeight(screenHeight),
    currentHintIndex(0),
    cameraOffset(0, 0),
    activeMessage(""), messageActive(false),
    oxygenDrainRate(1.5f),
    collectionRadius(50),
    score(0), elapsedSeconds(0),
    treasureActive(false), treasureCollected(false),
    treasure(nullptr)
{
    stateManager = new GameStateManager();
    player = new Player("PlaceHolder", QPoint(mapWidth / 2, mapHeight / 2));
    player->setMapBounds(mapWidth, mapHeight);
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

Game::~Game() {
    delete player;
    delete stateManager;
    delete treasure;
    qDeleteAll(hints);
    hints.clear();
}

void Game::start(const QString &playerName) {
    player->setName(playerName);
    reset();
}

void Game::reset() {
    player->setPosition(QPoint(mapWidth / 2, mapHeight / 2));
    player->resetOxygen();
    qDeleteAll(hints);
    hints.clear();
    delete treasure;
    treasure = nullptr;
    spawnHints();
    currentHintIndex = 0;
    activeMessage    = "";
    messageActive    = false;
    cameraOffset     = QPoint(0, 0);
    score            = 0;
    elapsedSeconds   = 0;
    treasureActive   = false;
    treasureCollected= false;
    updateCamera();
    stateManager->setState(GameState::Playing);
}

void Game::spawnHints() {
    struct HintData { QPoint pos; QString message; HintType type; };

    QList<HintData> clueData = {
                                { QPoint(350,  450),  "The sea holds secrets... follow the broken anchor.",       HintType::Clue },
                                { QPoint(850,  720),  "Near the sunken mast, something gleams in the current.",   HintType::Clue },
                                { QPoint(1450, 520),  "The old hull hides a clue beneath its barnacled shadow.",  HintType::Clue },
                                { QPoint(1950, 950),  "Dive deeper — an ancient marker points the way...",        HintType::Clue },
                                };

    for (int i = 0; i < clueData.size(); ++i) {
        Hint *h = new Hint(i, clueData[i].message, clueData[i].pos, clueData[i].type);
        if (i == 0) h->activate();
        hints.append(h);
    }

    // Scatter oxygen bubble pickups around the map (always active)
    QList<QPoint> bubbleSpots = {
        QPoint(600,  300),
        QPoint(1100, 850),
        QPoint(1700, 400),
        QPoint(300,  1200),
        QPoint(2100, 1100),
        QPoint(900,  1350),
    };
    for (const QPoint &bp : bubbleSpots) {
        Hint *b = new Hint(-1, "+25% oxygen refill!", bp, HintType::OxygenBubble, 12);
        b->activate();
        hints.append(b);
    }

    // Create (but don't activate) the final treasure
    // Placed at a dramatic location — centre-deep map
    treasure = new Hint(clueData.size(),
                        "✦ THE GOLDEN IDOL OF THE DEEP ✦\n"
                        "You have found the legendary treasure that has been lost\n"
                        "for centuries beneath these waters. You are a legend!",
                        QPoint(1200, 1100),
                        HintType::Treasure, 28, 500);
}

void Game::activateTreasureIfReady() {
    if (treasureActive || treasureCollected) return;
    // Check all story clues collected
    bool allDone = true;
    for (Hint *h : hints) {
        if (h->getType() == HintType::Clue && !h->isCollected()) {
            allDone = false;
            break;
        }
    }
    if (allDone) {
        treasure->activate();
        treasureActive = true;
        // Pop a message hinting the treasure
        activeMessage = "All clues found! The treasure has been revealed on your sonar map!";
        messageActive = true;
    }
}

void Game::update() {
    if (!stateManager->isPlaying()) return;
    player->update();
    updateCamera();
    for (Hint *h : hints) h->update();
    if (treasure) treasure->update();
    checkCollisions();
    activateTreasureIfReady();
    elapsedSeconds; // tracked via tickOxygen timer
    if (checkWinCondition())       stateManager->setState(GameState::Win);
    else if (checkLoseCondition()) stateManager->setState(GameState::Lose);
}

void Game::updateCamera() {
    int cx = player->getPosition().x() - screenWidth  / 2;
    int cy = player->getPosition().y() - screenHeight / 2;
    cx = qMax(0, qMin(cx, mapWidth  - screenWidth));
    cy = qMax(0, qMin(cy, mapHeight - screenHeight));
    cameraOffset = QPoint(cx, cy);
}

QPoint Game::getCameraOffset() const { return cameraOffset; }

void Game::checkCollisions() {
    QPoint pp = player->getPosition();

    // Check oxygen bubbles (always active, re-activate after collection for reuse — no, just one-time)
    for (Hint *h : hints) {
        if (h->getType() == HintType::OxygenBubble && h->isActive()) {
            QPoint hp = h->getPosition();
            int dx = pp.x() - hp.x(), dy = pp.y() - hp.y();
            if (std::sqrt(dx*dx + dy*dy) < collectionRadius) {
                h->collect();
                player->refillOxygen(25.0f);
                score += 5;
                activeMessage = "O₂ bubble collected! +25% oxygen.";
                messageActive = true;
            }
        }
    }

    // Check active story clue
    if (currentHintIndex < hints.size()) {
        Hint *active = hints[currentHintIndex];
        if (active->getType() == HintType::Clue && active->isActive()) {
            QPoint hp = active->getPosition();
            int dx = pp.x() - hp.x(), dy = pp.y() - hp.y();
            if (std::sqrt(dx*dx + dy*dy) < collectionRadius)
                onHintCollected(currentHintIndex);
        }
    }

    // Check treasure
    if (treasure && treasure->isActive() && !treasureCollected) {
        QPoint hp = treasure->getPosition();
        int dx = pp.x() - hp.x(), dy = pp.y() - hp.y();
        if (std::sqrt(dx*dx + dy*dy) < collectionRadius + 20) {
            treasure->collect();
            treasureCollected = true;
            score += 500;
            activeMessage = treasure->getMessage();
            messageActive = true;
        }
    }
}

void Game::onHintCollected(int index) {
    hints[index]->collect();
    activeMessage  = hints[index]->getMessage();
    messageActive  = true;
    score         += 100;
    currentHintIndex++;
    // Advance to next *clue* (skip oxygen bubbles in sequential list)
    while (currentHintIndex < hints.size() &&
           hints[currentHintIndex]->getType() != HintType::Clue)
        currentHintIndex++;
    if (currentHintIndex < hints.size())
        hints[currentHintIndex]->activate();
}

QString Game::getCurrentMessage() const { return activeMessage; }
bool    Game::hasActiveMessage()  const { return messageActive; }

void Game::dismissMessage() {
    messageActive = false;
    activeMessage = "";
}

bool Game::checkWinCondition() {
    return treasureCollected;
}

bool Game::checkLoseCondition() {
    return player->getOxygenLevel() <= 0.0f;
}

void Game::tickOxygen() {
    if (!stateManager->isPlaying()) return;
    elapsedSeconds++;
    // Drain rate accelerates gently over time (max 3× after 3 minutes)
    float factor = 1.0f + qMin(static_cast<float>(elapsedSeconds) / 180.0f, 2.0f);
    player->depleteOxygen(oxygenDrainRate * factor);
}

void Game::setPlayerMoving(Qt::Key key, bool pressed) {
    player->setMoving(key, pressed);
}

Player           *Game::getPlayer()          const { return player; }
QList<Hint *>     Game::getHints()           const { return hints + (treasure ? QList<Hint*>{treasure} : QList<Hint*>{}); }
GameStateManager *Game::getStateManager()    const { return stateManager; }
int               Game::getCurrentHintIndex() const { return currentHintIndex; }
int               Game::getScore()           const { return score; }
int               Game::getElapsedSeconds()  const { return elapsedSeconds; }
