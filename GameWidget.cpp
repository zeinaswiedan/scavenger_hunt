#include "GameWidget.h"
#include <cmath>

GameWidget::GameWidget(const QString &playerName, int mapWidth, int mapHeight, QWidget *parent)
    : QWidget(parent),
    mapWidth(mapWidth), mapHeight(mapHeight),
    warningFlash(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(800, 600);
    game = new Game(mapWidth, mapHeight, 800, 600);
    game->start(playerName);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameWidget::onGameTick);
    gameTimer->start(16); // ~60 fps

    secondTimer = new QTimer(this);
    connect(secondTimer, &QTimer::timeout, this, &GameWidget::onSecondTick);
    secondTimer->start(1000);
}

GameWidget::~GameWidget() { delete game; }

void GameWidget::onGameTick() {
    game->update();
    if (game->getPlayer()->getOxygenLevel() < 25.0f) warningFlash++;
    else warningFlash = 0;
    update();
}

void GameWidget::onSecondTick() { game->tickOxygen(); }

// ── Input ───────────────────────────────────────────────────────────────────

void GameWidget::keyPressEvent(QKeyEvent *event) {
    if (game->getStateManager()->isOver()) {
        if (event->key() == Qt::Key_R) game->reset();
        return;
    }
    if (game->hasActiveMessage()) {
        if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
            game->dismissMessage();
            return;
        }
    }
    if (event->key() == Qt::Key_Escape) {
        GameState s = game->getStateManager()->getState();
        if (s == GameState::Playing) game->getStateManager()->setState(GameState::Paused);
        else if (s == GameState::Paused) game->getStateManager()->setState(GameState::Playing);
        return;
    }
    game->setPlayerMoving(static_cast<Qt::Key>(event->key()), true);
}

void GameWidget::keyReleaseEvent(QKeyEvent *event) {
    game->setPlayerMoving(static_cast<Qt::Key>(event->key()), false);
}

// ── Paint ───────────────────────────────────────────────────────────────────

void GameWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    drawBackground(p);
    drawMapObjects(p);
    drawHints(p);
    drawPlayer(p);
    drawDarkness(p);
    drawHUD(p);
    drawMiniMap(p);

    if (game->hasActiveMessage())   drawMessagePopup(p);
    if (game->getStateManager()->isOver() || game->getStateManager()->isPaused())
        drawEndOverlay(p);
}

// ── Background ───────────────────────────────────────────────────────────────

void GameWidget::drawBackground(QPainter &p) {
    QLinearGradient sea(0, 0, 0, height());
    sea.setColorAt(0.0, QColor(0, 50, 90));
    sea.setColorAt(0.4, QColor(0, 25, 55));
    sea.setColorAt(1.0, QColor(0, 5, 18));
    p.fillRect(rect(), sea);

    QRandomGenerator rng(99);
    p.setPen(QPen(QColor(255, 255, 255, 18), 1));
    for (int i = 0; i < 140; ++i)
        p.drawPoint(rng.bounded(width()), rng.bounded(height()));
}

// ── Map Objects ──────────────────────────────────────────────────────────────

void GameWidget::drawMapObjects(QPainter &p) {
    QPoint cam = game->getCameraOffset();

    drawSunkenShip(p, 400  - cam.x(), 900  - cam.y(), 1.0f);
    drawSunkenShip(p, 1600 - cam.x(), 500  - cam.y(), 1.3f);
    drawSunkenShip(p, 900  - cam.x(), 1200 - cam.y(), 0.8f);

    drawRuinPillar(p, 700  - cam.x(), 800  - cam.y(), 120);
    drawRuinPillar(p, 760  - cam.x(), 820  - cam.y(), 80);
    drawRuinPillar(p, 820  - cam.x(), 790  - cam.y(), 150);
    drawRuinPillar(p, 1200 - cam.x(), 400  - cam.y(), 100);
    drawRuinPillar(p, 1260 - cam.x(), 420  - cam.y(), 130);
    drawRuinPillar(p, 2000 - cam.x(), 1100 - cam.y(), 90);
    drawRuinPillar(p, 2060 - cam.x(), 1080 - cam.y(), 110);

    drawRock(p, 300  - cam.x(), 1000 - cam.y(), 60);
    drawRock(p, 1100 - cam.x(), 700  - cam.y(), 45);
    drawRock(p, 1800 - cam.x(), 1300 - cam.y(), 70);
    drawRock(p, 500  - cam.x(), 400  - cam.y(), 35);
    drawRock(p, 2100 - cam.x(), 600  - cam.y(), 55);

    drawCoral(p, 600  - cam.x(), 950  - cam.y(), QColor(200,  60,  90));
    drawCoral(p, 650  - cam.x(), 970  - cam.y(), QColor(220, 100,  50));
    drawCoral(p, 1000 - cam.x(), 1100 - cam.y(), QColor(180,  40, 120));
    drawCoral(p, 1500 - cam.x(), 800  - cam.y(), QColor(200,  70,  60));
    drawCoral(p, 1900 - cam.x(), 400  - cam.y(), QColor(210,  80,  40));

    drawSeaweed(p, 450  - cam.x(), 1050 - cam.y(), 80);
    drawSeaweed(p, 480  - cam.x(), 1040 - cam.y(), 60);
    drawSeaweed(p, 1300 - cam.x(), 650  - cam.y(), 90);
    drawSeaweed(p, 1750 - cam.x(), 1200 - cam.y(), 70);
    drawSeaweed(p, 850  - cam.x(), 500  - cam.y(), 100);
}

void GameWidget::drawSunkenShip(QPainter &p, int x, int y, float s) {
    auto sc = [s](int v) { return static_cast<int>(v * s); };
    p.setPen(QPen(QColor(70, 50, 30), 2));
    p.save();
    p.translate(x, y);
    p.rotate(8);

    p.setBrush(QColor(45, 32, 18));
    p.drawRect(0, 0, sc(220), sc(70));

    p.setBrush(QColor(38, 27, 14));
    p.drawRect(sc(60), -sc(35), sc(80), sc(38));

    p.setPen(QPen(QColor(55, 38, 20), sc(5)));
    p.drawLine(sc(110), 0, sc(110), -sc(110));

    p.setPen(QPen(QColor(100, 85, 60, 120), 1));
    p.setBrush(QColor(90, 75, 50, 80));
    QPolygon sail;
    sail << QPoint(sc(110), -sc(110))
         << QPoint(sc(160), -sc(70))
         << QPoint(sc(110), -sc(40));
    p.drawPolygon(sail);

    p.setPen(QPen(QColor(30, 20, 10), 2));
    p.setBrush(QColor(10, 15, 25));
    p.drawEllipse(sc(20),  sc(20), sc(18), sc(18));
    p.drawEllipse(sc(55),  sc(20), sc(18), sc(18));
    p.drawEllipse(sc(155), sc(20), sc(18), sc(18));
    p.drawEllipse(sc(190), sc(20), sc(18), sc(18));
    p.restore();

    p.setPen(QPen(QColor(60, 55, 50), 3));
    p.drawLine(x + sc(30), y + sc(70), x + sc(30), y + sc(110));
    p.drawEllipse(x + sc(20), y + sc(105), sc(20), sc(10));
}

void GameWidget::drawRuinPillar(QPainter &p, int x, int y, int h) {
    p.setBrush(QColor(65, 58, 72));
    p.setPen(QPen(QColor(80, 72, 88), 1));
    p.drawRect(x - 12, y, 24, h);
    p.drawRect(x - 16, y - 10, 32, 12);
    p.setPen(QPen(QColor(30, 25, 35, 180), 1));
    p.drawLine(x - 4, y + 10, x + 2, y + 40);
    p.drawLine(x + 5, y + 50, x,     y + 80);
}

void GameWidget::drawCoral(QPainter &p, int x, int y, QColor color) {
    p.setPen(Qt::NoPen);
    for (int i = 0; i < 5; ++i) {
        int bx = x + (i - 2) * 10;
        int bh = 20 + (i % 3) * 15;
        p.setBrush(color.lighter(90 + i * 8));
        p.drawEllipse(bx - 5, y - bh, 10, bh);
        p.setBrush(color.lighter(140));
        p.drawEllipse(bx - 5, y - bh - 6, 10, 10);
    }
}

void GameWidget::drawRock(QPainter &p, int x, int y, int size) {
    p.setBrush(QColor(50, 48, 55));
    p.setPen(QPen(QColor(70, 67, 75), 1));
    QPolygon rock;
    rock << QPoint(x, y)
         << QPoint(x + size, y + size / 4)
         << QPoint(x + size, y + size * 3 / 4)
         << QPoint(x + size / 2, y + size)
         << QPoint(x - size / 4, y + size * 3 / 4)
         << QPoint(x - size / 4, y + size / 4);
    p.drawPolygon(rock);
}

void GameWidget::drawSeaweed(QPainter &p, int x, int y, int h) {
    p.setPen(QPen(QColor(20, 120, 60), 3, Qt::SolidLine, Qt::RoundCap));
    for (int i = 0; i < h; i += 4) {
        int x1 = x + static_cast<int>(8 * std::sin(i * 0.15));
        int x2 = x + static_cast<int>(8 * std::sin((i + 4) * 0.15));
        p.drawLine(x1, y - i, x2, y - i - 4);
    }
}

// ── Entity Draw ──────────────────────────────────────────────────────────────

void GameWidget::drawHints(QPainter &p) {
    QPoint cam = game->getCameraOffset();
    for (Hint *h : game->getHints()) h->draw(p, cam);
}

void GameWidget::drawPlayer(QPainter &p) {
    QPoint cam = game->getCameraOffset();
    game->getPlayer()->draw(p, cam);
}

// ── Darkness / Torch ─────────────────────────────────────────────────────────

void GameWidget::drawDarkness(QPainter &p) {
    QPoint worldPos = game->getPlayer()->getPosition();
    QPoint cam      = game->getCameraOffset();
    QPointF sp(worldPos.x() - cam.x(), worldPos.y() - cam.y());

    // Oxygen level dims the torch when low
    float oxygen = game->getPlayer()->getOxygenLevel();
    float torchR = TORCH_RADIUS * (0.5f + 0.5f * (oxygen / 100.0f));

    QRadialGradient torch(sp, torchR);
    torch.setColorAt(0.0,  QColor(0, 0, 0, 0));
    torch.setColorAt(0.55, QColor(0, 0, 0, 60));
    torch.setColorAt(0.85, QColor(0, 0, 0, 190));
    torch.setColorAt(1.0,  QColor(0, 0, 0, 250));
    p.setBrush(torch);
    p.setPen(Qt::NoPen);
    p.drawRect(rect());

    // Warm inner glow — colour shifts red when O₂ is low
    int warmR = static_cast<int>(255);
    int warmG = static_cast<int>(80 + 100 * (oxygen / 100.0f));
    int warmB = static_cast<int>(80 * (oxygen / 100.0f));
    QRadialGradient warmth(sp, torchR * 0.35f);
    warmth.setColorAt(0.0, QColor(warmR, warmG, warmB, 22));
    warmth.setColorAt(1.0, QColor(warmR, warmG, warmB, 0));
    p.setBrush(warmth);
    p.drawRect(rect());
}

// ── HUD ──────────────────────────────────────────────────────────────────────

void GameWidget::drawOxygenBar(QPainter &p, float oxygen) {
    const int barW = 220, barH = 20;
    const int barX = 20,  barY = 20;

    // Background track
    p.setBrush(QColor(0, 0, 0, 150));
    p.setPen(QPen(QColor(255, 255, 255, 30), 1));
    p.drawRoundedRect(barX - 2, barY - 2, barW + 4, barH + 4, 10, 10);

    // Segmented tick marks
    p.setPen(QPen(QColor(255, 255, 255, 20), 1));
    for (int i = 1; i < 10; ++i)
        p.drawLine(barX + barW * i / 10, barY + 4,
                   barX + barW * i / 10, barY + barH - 4);

    // Fill gradient
    QColor lo(220, 45, 45), mid(230, 180, 0), hi(0, 210, 160);
    QColor barColor;
    if      (oxygen > 60.0f) barColor = hi;
    else if (oxygen > 30.0f) barColor = mid;
    else                     barColor = lo;

    int fillW = qMax(4, static_cast<int>(oxygen / 100.0f * barW));
    QLinearGradient fillGrad(barX, 0, barX + fillW, 0);
    fillGrad.setColorAt(0.0, barColor.lighter(130));
    fillGrad.setColorAt(1.0, barColor);
    p.setBrush(fillGrad);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(barX, barY, fillW, barH, 9, 9);

    // Label: O₂
    p.setPen(Qt::white);
    p.setFont(QFont("Courier New", 9, QFont::Bold));
    p.drawText(barX + barW + 10, barY + 14, "O₂");

    // Percentage inside bar
    p.setPen(QColor(255, 255, 255, 210));
    p.setFont(QFont("Courier New", 8, QFont::Bold));
    p.drawText(barX + 6, barY + 14, QString::number(static_cast<int>(oxygen)) + "%");
}

void GameWidget::drawOxygenWarning(QPainter &p, float oxygen) {
    if (oxygen >= 25.0f) return;
    // Pulsing red vignette
    int alpha = static_cast<int>(60 + 50 * std::sin(warningFlash * 0.25f));
    QRadialGradient vignette(QPointF(width() / 2.0, height() / 2.0), width() * 0.75);
    vignette.setColorAt(0.0, QColor(180, 0, 0, 0));
    vignette.setColorAt(0.6, QColor(180, 0, 0, 0));
    vignette.setColorAt(1.0, QColor(180, 0, 0, alpha));
    p.setBrush(vignette);
    p.setPen(Qt::NoPen);
    p.drawRect(rect());

    // Flashing text warning
    if ((warningFlash / 20) % 2 == 0) {
        p.setPen(QColor(255, 80, 80));
        p.setFont(QFont("Courier New", 11, QFont::Bold));
        p.drawText(QRect(0, height() - 90, width(), 24), Qt::AlignCenter, "⚠  OXYGEN CRITICAL  ⚠");
    }
}

void GameWidget::drawHUD(QPainter &p) {
    float oxygen = game->getPlayer()->getOxygenLevel();

    drawOxygenBar(p, oxygen);
    drawOxygenWarning(p, oxygen);

    // Clue counter
    int totalClues = 0, collected = 0;
    for (Hint *h : game->getHints()) {
        if (h->getType() == HintType::Clue) {
            totalClues++;
            if (h->isCollected()) collected++;
        }
    }
    p.setPen(QColor(255, 220, 80));
    p.setFont(QFont("Courier New", 10, QFont::Bold));
    p.drawText(20, 62, QString("CLUES  %1 / %2").arg(collected).arg(totalClues));

    // Depth
    int depth = static_cast<int>(game->getPlayer()->getPosition().y() / 10.0f);
    p.setPen(QColor(140, 200, 255, 180));
    p.setFont(QFont("Courier New", 9));
    p.drawText(20, 80, QString("DEPTH  %1 m").arg(depth));

    // Score
    p.setPen(QColor(100, 230, 200, 200));
    p.setFont(QFont("Courier New", 9));
    p.drawText(20, 96, QString("SCORE  %1").arg(game->getScore()));

    // Elapsed time (top-right)
    int secs = game->getElapsedSeconds();
    int mm = secs / 60, ss = secs % 60;
    QString timeStr = QString("%1:%2").arg(mm, 2, 10, QChar('0')).arg(ss, 2, 10, QChar('0'));
    p.setPen(QColor(180, 210, 255, 180));
    p.setFont(QFont("Courier New", 10, QFont::Bold));
    p.drawText(QRect(0, 20, width() - 196, 20), Qt::AlignRight, timeStr + "  ");

    // Controls reminder (faint, bottom-left)
    p.setPen(QColor(255, 255, 255, 50));
    p.setFont(QFont("Courier New", 7));
    p.drawText(20, height() - 12, "WASD / ARROWS  ·  ESC pause  ·  R restart");
}

// ── Mini-map ─────────────────────────────────────────────────────────────────

void GameWidget::drawMiniMap(QPainter &p) {
    const int mmW = 160, mmH = 100;
    const int mmX = width() - mmW - 16;
    const int mmY = 16;

    float scaleX = static_cast<float>(mmW) / mapWidth;
    float scaleY = static_cast<float>(mmH) / mapHeight;

    p.setBrush(QColor(0, 10, 25, 190));
    p.setPen(QPen(QColor(100, 160, 220, 80), 1));
    p.drawRect(mmX, mmY, mmW, mmH);

    // Viewport outline
    QPoint cam = game->getCameraOffset();
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(255, 255, 255, 40), 1));
    p.drawRect(mmX + static_cast<int>(cam.x() * scaleX),
               mmY + static_cast<int>(cam.y() * scaleY),
               static_cast<int>(800 * scaleX),
               static_cast<int>(600 * scaleY));

    // Hint dots
    for (Hint *h : game->getHints()) {
        if (!h->isActive() || h->isCollected()) continue;
        int hx = mmX + static_cast<int>(h->getPosition().x() * scaleX);
        int hy = mmY + static_cast<int>(h->getPosition().y() * scaleY);
        QColor dotColor;
        switch (h->getType()) {
        case HintType::Clue:         dotColor = QColor(255, 200, 0);   break;
        case HintType::Treasure:     dotColor = QColor(255, 215, 30);  break;
        case HintType::OxygenBubble: dotColor = QColor(80, 200, 255);  break;
        }
        p.setBrush(dotColor);
        p.setPen(Qt::NoPen);
        int ds = (h->getType() == HintType::Treasure) ? 6 : 4;
        p.drawEllipse(hx - ds/2, hy - ds/2, ds, ds);
    }

    // Player dot
    QPoint pp = game->getPlayer()->getPosition();
    int px = mmX + static_cast<int>(pp.x() * scaleX);
    int py = mmY + static_cast<int>(pp.y() * scaleY);
    p.setBrush(QColor(100, 200, 255));
    p.setPen(Qt::NoPen);
    p.drawEllipse(px - 4, py - 4, 8, 8);

    p.setPen(QColor(180, 220, 255, 160));
    p.setFont(QFont("Courier New", 7));
    p.drawText(mmX + 4, mmY + mmH - 4, "SONAR MAP");
}

// ── Message Popup ────────────────────────────────────────────────────────────

void GameWidget::drawMessagePopup(QPainter &p) {
    int boxW = 520, boxH = 130;
    int boxX = (width()  - boxW) / 2;
    int boxY = (height() - boxH) / 2;

    p.setBrush(QColor(0, 15, 35, 230));
    p.setPen(QPen(QColor(80, 160, 220, 200), 2));
    p.drawRoundedRect(boxX, boxY, boxW, boxH, 12, 12);

    p.setPen(QColor(255, 200, 60));
    p.setFont(QFont("Courier New", 11, QFont::Bold));
    p.drawText(boxX + 20, boxY + 30, "✦  CLUE DISCOVERED");

    p.setPen(QColor(200, 230, 255));
    p.setFont(QFont("Courier New", 10));
    p.drawText(QRect(boxX + 20, boxY + 46, boxW - 40, 60),
               Qt::AlignLeft | Qt::TextWordWrap,
               game->getCurrentMessage());

    p.setPen(QColor(120, 160, 200, 160));
    p.setFont(QFont("Courier New", 8));
    p.drawText(boxX + boxW - 200, boxY + boxH - 10, "[ SPACE / ENTER to continue ]");
}

// ── End / Pause Overlay ──────────────────────────────────────────────────────

void GameWidget::drawEndOverlay(QPainter &p) {
    GameState state = game->getStateManager()->getState();
    p.fillRect(rect(), QColor(0, 0, 0, 175));

    if (state == GameState::Paused) {
        p.setPen(QColor(140, 200, 255));
        p.setFont(QFont("Courier New", 32, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "— PAUSED —");
        p.setFont(QFont("Courier New", 12));
        p.setPen(QColor(160, 200, 230, 180));
        p.drawText(QRect(0, height() / 2 + 50, width(), 30), Qt::AlignCenter, "Press ESC to resume");
        return;
    }

    if (state == GameState::Win) {
        // Gold glow
        QRadialGradient glow(QPointF(width() / 2.0, height() / 2.0 - 40), 220);
        glow.setColorAt(0.0, QColor(255, 200, 30, 80));
        glow.setColorAt(1.0, QColor(255, 200, 30, 0));
        p.setBrush(glow);
        p.setPen(Qt::NoPen);
        p.drawRect(rect());

        p.setPen(QColor(255, 210, 60));
        p.setFont(QFont("Courier New", 34, QFont::Bold));
        p.drawText(QRect(0, height() / 2 - 100, width(), 60), Qt::AlignCenter, "TREASURE FOUND!");

        p.setPen(QColor(200, 230, 255));
        p.setFont(QFont("Courier New", 13));
        p.drawText(QRect(0, height() / 2 - 30, width(), 30), Qt::AlignCenter,
                   QString("You survived with %1% oxygen remaining.")
                       .arg(static_cast<int>(game->getPlayer()->getOxygenLevel())));

        int mm = game->getElapsedSeconds() / 60, ss = game->getElapsedSeconds() % 60;
        p.setPen(QColor(160, 220, 255, 200));
        p.setFont(QFont("Courier New", 12));
        p.drawText(QRect(0, height() / 2 + 5, width(), 30), Qt::AlignCenter,
                   QString("Time: %1:%2   Score: %3")
                       .arg(mm, 2, 10, QChar('0'))
                       .arg(ss, 2, 10, QChar('0'))
                       .arg(game->getScore()));
    }

    if (state == GameState::Lose) {
        p.setPen(QColor(220, 50, 50));
        p.setFont(QFont("Courier New", 34, QFont::Bold));
        p.drawText(QRect(0, height() / 2 - 100, width(), 60), Qt::AlignCenter, "OXYGEN DEPLETED");

        p.setPen(QColor(200, 180, 180));
        p.setFont(QFont("Courier New", 13));
        p.drawText(QRect(0, height() / 2 - 30, width(), 30), Qt::AlignCenter,
                   "The depths claimed another soul...");

        p.setPen(QColor(180, 160, 160, 180));
        p.setFont(QFont("Courier New", 11));
        p.drawText(QRect(0, height() / 2 + 10, width(), 30), Qt::AlignCenter,
                   QString("Score: %1   Clues found: %2 / 4").arg(game->getScore()).arg(game->getCurrentHintIndex()));
    }

    p.setPen(QColor(140, 180, 200, 180));
    p.setFont(QFont("Courier New", 11));
    p.drawText(QRect(0, height() / 2 + 80, width(), 30), Qt::AlignCenter,
               "Press R to dive again   |   ESC to surface");
}
