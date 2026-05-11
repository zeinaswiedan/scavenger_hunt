#pragma once
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QRandomGenerator>
#include <QFont>
#include "Game.h"

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(const QString &playerName,
                        int mapWidth  = 2400,
                        int mapHeight = 1600,
                        QWidget *parent = nullptr);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onGameTick();
    void onSecondTick();

private:
    // High-level draw passes
    void drawBackground(QPainter &p);
    void drawMapObjects(QPainter &p);
    void drawHints(QPainter &p);
    void drawPlayer(QPainter &p);
    void drawDarkness(QPainter &p);
    void drawHUD(QPainter &p);
    void drawMiniMap(QPainter &p);
    void drawMessagePopup(QPainter &p);
    void drawEndOverlay(QPainter &p);

    // Environment helpers
    void drawSunkenShip(QPainter &p, int x, int y, float scale = 1.0f);
    void drawRuinPillar(QPainter &p, int x, int y, int h);
    void drawCoral(QPainter &p, int x, int y, QColor color);
    void drawRock(QPainter &p, int x, int y, int size);
    void drawSeaweed(QPainter &p, int x, int y, int h);

    // HUD helpers
    void drawOxygenBar(QPainter &p, float oxygen);
    void drawOxygenWarning(QPainter &p, float oxygen);

    Game   *game;
    QTimer *gameTimer;
    QTimer *secondTimer;
    int     mapWidth;
    int     mapHeight;

    int     warningFlash;   // frame counter for low-O₂ flash

    static constexpr int TORCH_RADIUS = 200;
};
