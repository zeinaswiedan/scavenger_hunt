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
    explicit GameWidget(const QString &playerName, int levelNumber,
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
    void drawBackground(QPainter &p);
    void drawMapObjects(QPainter &p);
    void drawHints(QPainter &p);
    void drawPlayer(QPainter &p);
    void drawSharks(QPainter &p);
    void drawDarkness(QPainter &p);
    void drawHUD(QPainter &p);
    void drawMiniMap(QPainter &p);
    void drawMessagePopup(QPainter &p);
    void drawEndOverlay(QPainter &p);

    void drawSunkenShip(QPainter &p, int x, int y, float scale = 1.0f);
    void drawRuinPillar(QPainter &p, int x, int y, int h);
    void drawCoral(QPainter &p, int x, int y, QColor color);
    void drawRock(QPainter &p, int x, int y, int size);
    void drawSeaweed(QPainter &p, int x, int y, int h);

    // Y. Extracted O2 bar and warning vignette into helpers to keep drawHUD readable
    void drawOxygenBar(QPainter &p, float oxygen);
    void drawOxygenWarning(QPainter &p, float oxygen);

    Game   *game;
    QTimer *gameTimer;
    QTimer *secondTimer;

    // Y. Counts frames while O2 < 25% to drive the pulsing red vignette
    int warningFlash;
};
