#include "GameWidget.h"
#include <cmath>

// Coral colour palette indexed by SceneryObject::colorIndex
static const QColor CORAL_COLORS[] = {
    QColor(200, 60,  90),
    QColor(220, 100, 50),
    QColor(180, 40, 120),
    QColor(200, 70,  60),
    QColor(210, 80,  40),
};

GameWidget::GameWidget(const QString &playerName, int levelNumber, QWidget *parent)
    : QWidget(parent), warningFlash(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(800, 600);
    game = new Game(800, 600);
    game->start(playerName, levelNumber);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameWidget::onGameTick);
    gameTimer->start(16);

    secondTimer = new QTimer(this);
    connect(secondTimer, &QTimer::timeout, this, &GameWidget::onSecondTick);
    secondTimer->start(1000);
}

GameWidget::~GameWidget() { delete game; }

void GameWidget::onGameTick() {
    game->update();
    // Y. Advance the warning flash counter only while oxygen is critical
    if (game->getPlayer()->getOxygenLevel() < 25.0f) warningFlash++;
    else warningFlash = 0;
    update();
}

void GameWidget::onSecondTick() { game->tickOxygen(); }

// ── Input ─────────────────────────────────────────────────────────────────────

void GameWidget::keyPressEvent(QKeyEvent *event) {
    if (game->getStateManager()->isOver()) {
        if (event->key() == Qt::Key_R) game->reset();
        return;
    }

    GameState state = game->getStateManager()->getState();

    if (state == GameState::LEVEL_COMPLETE) {
        if (event->key() == Qt::Key_Return) {
            if (game->getCurrentLevel() < 3)
                game->loadLevel(game->getCurrentLevel() + 1);
        }
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

// ── Paint ─────────────────────────────────────────────────────────────────────

void GameWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    drawBackground(p);
    drawMapObjects(p);
    drawHints(p);
    // Y. Sharks drawn before the player so the player always appears on top
    drawSharks(p);
    drawPlayer(p);
    drawDarkness(p);
    drawHUD(p);
    drawMiniMap(p);

    if (game->hasActiveMessage()) drawMessagePopup(p);
    if (game->getStateManager()->isOver() ||
        game->getStateManager()->isPaused() ||
        game->getStateManager()->getState() == GameState::LEVEL_COMPLETE)
        drawEndOverlay(p);
}

// ── Background ────────────────────────────────────────────────────────────────

void GameWidget::drawBackground(QPainter &p) {
    QLinearGradient sea(0, 0, 0, height());
    sea.setColorAt(0.0, QColor(0, 50, 90));
    sea.setColorAt(0.4, QColor(0, 25, 55));
    sea.setColorAt(1.0, QColor(0, 5, 18));
    p.fillRect(rect(), sea);

    QRandomGenerator rng(99);
    p.setPen(QPen(QColor(255, 255, 255, 18), 1));
    for (int i = 0; i < 120; ++i)
        p.drawPoint(rng.bounded(width()), rng.bounded(height()));
}

// ── Map Objects ───────────────────────────────────────────────────────────────

// Y. Now reads scenery list from the active LevelConfig instead of hard-coded
//    positions so every level gets its own unique layout automatically
void GameWidget::drawMapObjects(QPainter &p) {
    QPoint cam = game->getCameraOffset();

    // Pull the scenery list from whatever level is loaded
    const QList<SceneryObject> &scenery = game->getActiveConfig().scenery;

    for (const SceneryObject &obj : scenery) {
        int x = obj.position.x() - cam.x();
        int y = obj.position.y() - cam.y();
        switch (obj.type) {
        case SceneryType::SUNKEN_SHIP:
            drawSunkenShip(p, x, y, 1.0f);
            break;
        case SceneryType::RUIN_PILLAR:
            drawRuinPillar(p, x, y, obj.size);
            break;
        case SceneryType::CORAL: {
            int ci = qBound(0, obj.colorIndex,
                            static_cast<int>(sizeof(CORAL_COLORS) / sizeof(CORAL_COLORS[0])) - 1);
            drawCoral(p, x, y, CORAL_COLORS[ci]);
            break;
        }
        case SceneryType::ROCK:
            drawRock(p, x, y, obj.size);
            break;
        case SceneryType::SEAWEED:
            drawSeaweed(p, x, y, obj.size);
            break;
        }
    }
}

void GameWidget::drawSunkenShip(QPainter &p, int x, int y, float s) {
    auto sc = [s](int v) { return static_cast<int>(v * s); };
    p.save();
    p.translate(x, y);
    p.rotate(8);
    p.setBrush(QColor(45, 32, 18));
    p.setPen(QPen(QColor(70, 50, 30), 2));
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

// ── Entity Draw ───────────────────────────────────────────────────────────────

void GameWidget::drawHints(QPainter &p) {
    QPoint cam = game->getCameraOffset();
    for (Hint *h : game->getHints()) h->draw(p, cam);
}

void GameWidget::drawSharks(QPainter &p) {
    QPoint cam = game->getCameraOffset();
    for (Shark *shark : game->getSharks()) shark->draw(p, cam);
}

void GameWidget::drawPlayer(QPainter &p) {
    QPoint cam = game->getCameraOffset();
    game->getPlayer()->draw(p, cam);
}

// ── Darkness / Torch ──────────────────────────────────────────────────────────

void GameWidget::drawDarkness(QPainter &p) {
    QPoint worldPos = game->getPlayer()->getPosition();
    QPoint cam      = game->getCameraOffset();
    QPointF sp(worldPos.x() - cam.x(), worldPos.y() - cam.y());

    // Y. Torch radius shrinks slightly as oxygen drops to add visual tension
    float oxygen = game->getPlayer()->getOxygenLevel();
    float torchR = game->getTorchRadius() * (0.6f + 0.4f * (oxygen / 100.0f));

    QRadialGradient torch(sp, torchR);
    torch.setColorAt(0.0,  QColor(0, 0, 0, 0));
    torch.setColorAt(0.55, QColor(0, 0, 0, 60));
    torch.setColorAt(0.85, QColor(0, 0, 0, 180));
    torch.setColorAt(1.0,  QColor(0, 0, 0, 245));
    p.setBrush(torch);
    p.setPen(Qt::NoPen);
    p.drawRect(rect());

    // Y. Warm inner glow shifts from yellow-white to red as oxygen drops
    int warmG = static_cast<int>(80 + 100 * (oxygen / 100.0f));
    int warmB = static_cast<int>(80 * (oxygen / 100.0f));
    QRadialGradient warmth(sp, torchR * 0.3f);
    warmth.setColorAt(0.0, QColor(255, warmG, warmB, 18));
    warmth.setColorAt(1.0, QColor(255, warmG, warmB, 0));
    p.setBrush(warmth);
    p.drawRect(rect());
}

// ── HUD ───────────────────────────────────────────────────────────────────────

// Y. Segmented, gradient-filled O2 bar replacing the plain coloured rectangle
void GameWidget::drawOxygenBar(QPainter &p, float oxygen) {
    const int barW = 200, barH = 18;
    const int barX = 20,  barY = 20;

    p.setBrush(QColor(0, 0, 0, 140));
    p.setPen(QPen(QColor(255, 255, 255, 40), 1));
    p.drawRoundedRect(barX - 2, barY - 2, barW + 4, barH + 4, 10, 10);

    p.setPen(QPen(QColor(255, 255, 255, 20), 1));
    for (int i = 1; i < 10; ++i)
        p.drawLine(barX + barW * i / 10, barY + 4,
                   barX + barW * i / 10, barY + barH - 4);

    QColor barColor;
    if      (oxygen > 60.0f) barColor = QColor(0, 210, 160);
    else if (oxygen > 30.0f) barColor = QColor(230, 180, 0);
    else                     barColor = QColor(220, 45, 45);

    int fillW = qMax(4, static_cast<int>(oxygen / 100.0f * barW));
    QLinearGradient fillGrad(barX, 0, barX + fillW, 0);
    fillGrad.setColorAt(0.0, barColor.lighter(130));
    fillGrad.setColorAt(1.0, barColor);
    p.setBrush(fillGrad);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(barX, barY, fillW, barH, 9, 9);

    p.setPen(Qt::white);
    p.setFont(QFont("Courier New", 9, QFont::Bold));
    p.drawText(barX + barW + 8, barY + 13, "O₂");

    p.setPen(QColor(255, 255, 255, 210));
    p.setFont(QFont("Courier New", 8, QFont::Bold));
    p.drawText(barX + 6, barY + 13, QString::number(static_cast<int>(oxygen)) + "%");
}

// Y. Pulsing red vignette + flashing text when O2 drops below 25%
void GameWidget::drawOxygenWarning(QPainter &p, float oxygen) {
    if (oxygen >= 25.0f) return;
    int alpha = static_cast<int>(60 + 50 * std::sin(warningFlash * 0.25f));
    QRadialGradient vignette(QPointF(width() / 2.0, height() / 2.0), width() * 0.75);
    vignette.setColorAt(0.0, QColor(180, 0, 0, 0));
    vignette.setColorAt(0.6, QColor(180, 0, 0, 0));
    vignette.setColorAt(1.0, QColor(180, 0, 0, alpha));
    p.setBrush(vignette);
    p.setPen(Qt::NoPen);
    p.drawRect(rect());

    if ((warningFlash / 20) % 2 == 0) {
        p.setPen(QColor(255, 80, 80));
        p.setFont(QFont("Courier New", 11, QFont::Bold));
        p.drawText(QRect(0, height() - 90, width(), 24),
                   Qt::AlignCenter, "⚠  OXYGEN CRITICAL  ⚠");
    }
}

void GameWidget::drawHUD(QPainter &p) {
    float oxygen = game->getPlayer()->getOxygenLevel();
    drawOxygenBar(p, oxygen);
    drawOxygenWarning(p, oxygen);

    // Clue counter
    int hints    = game->getHints().size();
    int current  = game->getCurrentHintIndex();
    p.setPen(QColor(255, 220, 80));
    p.setFont(QFont("Courier New", 10, QFont::Bold));
    p.drawText(20, 58, QString("HINT  %1 / %2").arg(current).arg(hints));

    // Depth
    int depth = static_cast<int>(game->getPlayer()->getPosition().y() / 10.0f);
    p.setPen(QColor(140, 200, 255, 180));
    p.setFont(QFont("Courier New", 9));
    p.drawText(20, 76, QString("DEPTH  %1 m").arg(depth));

    // Y. Score display
    p.setPen(QColor(100, 230, 200, 200));
    p.setFont(QFont("Courier New", 9));
    p.drawText(20, 92, QString("SCORE  %1").arg(game->getScore()));

    // Y. Live timer (top-right, outside the minimap)
    int secs = game->getElapsedSeconds();
    QString timeStr = QString("%1:%2")
                          .arg(secs / 60, 2, 10, QChar('0'))
                          .arg(secs % 60, 2, 10, QChar('0'));
    p.setPen(QColor(180, 210, 255, 180));
    p.setFont(QFont("Courier New", 10, QFont::Bold));
    p.drawText(QRect(0, 20, width() - 196, 20), Qt::AlignRight, timeStr + "  ");

    // Y. Shark proximity warning in the HUD text area
    for (Shark *shark : game->getSharks()) {
        QPoint pp = game->getPlayer()->getPosition();
        QPoint sp = shark->getPosition();
        int dx = pp.x() - sp.x(), dy = pp.y() - sp.y();
        float dist = std::sqrt(static_cast<float>(dx*dx + dy*dy));
        if (dist < 300.0f) {
            int a = static_cast<int>(200 * (1.0f - dist / 300.0f));
            p.setPen(QColor(255, 60, 60, a));
            p.setFont(QFont("Courier New", 9, QFont::Bold));
            p.drawText(20, 108, "🦈  SHARK NEARBY");
            break;
        }
    }

    // Level name (top-centre, subtle)
    p.setPen(QColor(160, 200, 230, 120));
    p.setFont(QFont("Courier New", 8));
    p.drawText(QRect(0, 6, width(), 14), Qt::AlignCenter, game->getLevelName());

    // Controls reminder
    p.setPen(QColor(255, 255, 255, 45));
    p.setFont(QFont("Courier New", 7));
    p.drawText(20, height() - 12,
               "WASD / ARROWS  ·  ESC pause  ·  R restart");
}

// ── Mini-map ──────────────────────────────────────────────────────────────────

void GameWidget::drawMiniMap(QPainter &p) {
    const int mmW = 160, mmH = 100;
    const int mmX = width()  - mmW - 16;
    const int mmY = 16;

    float scaleX = static_cast<float>(mmW) / game->getMapWidth();
    float scaleY = static_cast<float>(mmH) / game->getMapHeight();

    p.setBrush(QColor(0, 10, 25, 180));
    p.setPen(QPen(QColor(100, 160, 220, 80), 1));
    p.drawRect(mmX, mmY, mmW, mmH);

    QPoint cam = game->getCameraOffset();
    int vpW = static_cast<int>(800 * scaleX);
    int vpH = static_cast<int>(600 * scaleY);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(255, 255, 255, 50), 1));
    p.drawRect(mmX + static_cast<int>(cam.x() * scaleX),
               mmY + static_cast<int>(cam.y() * scaleY),
               vpW, vpH);

    // Hint dots — gold for clues, bright gold + larger for treasure
    for (Hint *h : game->getHints()) {
        if (!h->isActive() || h->isCollected()) continue;
        int hx = mmX + static_cast<int>(h->getPosition().x() * scaleX);
        int hy = mmY + static_cast<int>(h->getPosition().y() * scaleY);
        int ds = h->isTreasure() ? 6 : 4;
        p.setBrush(h->isTreasure() ? QColor(255, 215, 30) : QColor(255, 200, 0));
        p.setPen(Qt::NoPen);
        p.drawEllipse(hx - ds/2, hy - ds/2, ds, ds);
    }

    // Y. Red shark icon on the minimap for every active shark
    for (Shark *shark : game->getSharks()) {
        QPoint sp = shark->getPosition();
        int sx2 = mmX + static_cast<int>(sp.x() * scaleX);
        int sy2 = mmY + static_cast<int>(sp.y() * scaleY);
        sx2 = qBound(mmX + 2, sx2, mmX + mmW - 2);
        sy2 = qBound(mmY + 2, sy2, mmY + mmH - 2);

        p.save();
        p.translate(sx2, sy2);
        // Small red diamond shape for the shark dot
        p.setBrush(QColor(220, 40, 40));
        p.setPen(QPen(QColor(255, 80, 80), 1));
        QPolygon sharkDot;
        sharkDot << QPoint(0, -5) << QPoint(4, 3) << QPoint(0, 1) << QPoint(-4, 3);
        p.drawPolygon(sharkDot);
        p.restore();

        // Proximity pulse ring when the shark is closing in
        QPoint pp = game->getPlayer()->getPosition();
        int dx = pp.x() - sp.x(), dy = pp.y() - sp.y();
        float dist = std::sqrt(static_cast<float>(dx*dx + dy*dy));
        if (dist < 400.0f) {
            int ringA = static_cast<int>(120 * (1.0f - dist / 400.0f));
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(220, 40, 40, ringA), 1));
            p.drawEllipse(QPoint(sx2, sy2), 7, 7);
        }
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

// ── Message Popup ─────────────────────────────────────────────────────────────

void GameWidget::drawMessagePopup(QPainter &p) {
    int boxW = 500, boxH = 110;
    int boxX = (width()  - boxW) / 2;
    int boxY = (height() - boxH) / 2;

    p.setBrush(QColor(0, 15, 35, 220));
    p.setPen(QPen(QColor(80, 160, 220, 180), 2));
    p.drawRoundedRect(boxX, boxY, boxW, boxH, 12, 12);

    p.setPen(QColor(255, 200, 60));
    p.setFont(QFont("Courier New", 11, QFont::Bold));
    p.drawText(boxX + 20, boxY + 28, "✦  CLUE DISCOVERED");

    p.setPen(QColor(200, 230, 255));
    p.setFont(QFont("Courier New", 10));
    p.drawText(QRect(boxX + 20, boxY + 44, boxW - 40, 44),
               Qt::AlignLeft | Qt::TextWordWrap,
               game->getCurrentMessage());

    p.setPen(QColor(120, 160, 200, 160));
    p.setFont(QFont("Courier New", 8));
    p.drawText(boxX + boxW - 180, boxY + boxH - 10,
               "[ SPACE / ENTER to continue ]");
}

// ── End / Pause Overlay ───────────────────────────────────────────────────────

void GameWidget::drawEndOverlay(QPainter &p) {
    GameState state = game->getStateManager()->getState();
    p.fillRect(rect(), QColor(0, 0, 0, 170));

    if (state == GameState::Paused) {
        p.setPen(QColor(140, 200, 255));
        p.setFont(QFont("Courier New", 32, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "— PAUSED —");
        p.setFont(QFont("Courier New", 12));
        p.setPen(QColor(160, 200, 230, 180));
        p.drawText(QRect(0, height() / 2 + 50, width(), 30),
                   Qt::AlignCenter, "Press ESC to resume");
        return;
    }

    if (state == GameState::LEVEL_COMPLETE) {
        p.setPen(QColor(80, 200, 160));
        p.setFont(QFont("Courier New", 30, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "LEVEL COMPLETE");
        p.setFont(QFont("Courier New", 12));
        // Y. Show score on the level-complete screen too
        p.setPen(QColor(200, 230, 255));
        p.drawText(QRect(0, height() / 2 + 20, width(), 30),
                   Qt::AlignCenter,
                   QString("Score: %1").arg(game->getScore()));
        p.setPen(QColor(160, 200, 230, 180));
        p.drawText(QRect(0, height() / 2 + 50, width(), 30),
                   Qt::AlignCenter, "Press ENTER to dive deeper");
        return;
    }

    if (state == GameState::Win) {
        // Y. Gold radial glow behind the win text
        QRadialGradient glow(QPointF(width() / 2.0, height() / 2.0 - 40), 220);
        glow.setColorAt(0.0, QColor(255, 200, 30, 80));
        glow.setColorAt(1.0, QColor(255, 200, 30, 0));
        p.setBrush(glow);
        p.setPen(Qt::NoPen);
        p.drawRect(rect());

        p.setPen(QColor(255, 210, 60));
        p.setFont(QFont("Courier New", 34, QFont::Bold));
        p.drawText(QRect(0, height() / 2 - 90, width(), 60),
                   Qt::AlignCenter, "TREASURE FOUND");

        p.setPen(QColor(200, 230, 255));
        p.setFont(QFont("Courier New", 13));
        p.drawText(QRect(0, height() / 2 - 20, width(), 30), Qt::AlignCenter,
                   QString("Survived with %1% oxygen remaining.")
                       .arg(static_cast<int>(game->getPlayer()->getOxygenLevel())));

        // Y. Show final score and time on the win screen
        int mm = game->getElapsedSeconds() / 60, ss = game->getElapsedSeconds() % 60;
        p.setPen(QColor(160, 220, 255, 200));
        p.setFont(QFont("Courier New", 12));
        p.drawText(QRect(0, height() / 2 + 10, width(), 30), Qt::AlignCenter,
                   QString("Time: %1:%2   Score: %3")
                       .arg(mm, 2, 10, QChar('0'))
                       .arg(ss, 2, 10, QChar('0'))
                       .arg(game->getScore()));
    }

    if (state == GameState::Lose) {
        p.setPen(QColor(220, 50, 50));
        p.setFont(QFont("Courier New", 34, QFont::Bold));
        p.drawText(QRect(0, height() / 2 - 80, width(), 60),
                   Qt::AlignCenter, "OXYGEN DEPLETED");
        p.setPen(QColor(200, 180, 180));
        p.setFont(QFont("Courier New", 13));
        p.drawText(QRect(0, height() / 2, width(), 36),
                   Qt::AlignCenter, "The depths claimed another soul...");
    }

    p.setPen(QColor(140, 180, 200, 180));
    p.setFont(QFont("Courier New", 11));
    p.drawText(QRect(0, height() / 2 + 80, width(), 30),
               Qt::AlignCenter, "Press R to dive again");
}
