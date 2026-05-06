#include "GameWidget.h"
#include <cmath>

GameWidget::GameWidget(const QString& playerName, int mapWidth, int mapHeight, QWidget* parent) :
	QWidget(parent),
	mapWidth(mapWidth),
	mapHeight(mapHeight)
{
	setFocusPolicy(Qt::StrongFocus);
	setFixedSize(800, 600);
	game = new Game(mapWidth, mapHeight, 800, 600);
	game->start(playerName);

	gameTimer = new QTimer(this);
	connect(gameTimer, &QTimer::timeout, this, &GameWidget::onGameTick);
	gameTimer->start(16);

	secondTimer = new QTimer(this);
	connect(secondTimer, &QTimer::timeout, this, &GameWidget::onSecondTick);
	secondTimer->start(1000);
}

GameWidget::~GameWidget() {
	delete game;
}

void GameWidget::onGameTick(){
	game->update();
	update();
}

void GameWidget::onSecondTick(){
	game->tickOxygen();
}

void GameWidget::keyPressEvent(QKeyEvent *event) {
	if (game->getStateManager()->isOver()) {
        	if (event->key() == Qt::Key_R)
            		game->reset();
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
        	if (s == GameState::Playing)
        		game->getStateManager()->setState(GameState::Paused);
        	else if (s == GameState::Paused)
         	   	game->getStateManager()->setState(GameState::Playing);
        	return;
    	}

    	game->setPlayerMoving(static_cast<Qt::Key>(event->key()), true);
}


void GameWidget::keyReleaseEvent(QKeyEvent *event) {
	game->setPlayerMoving(static_cast<Qt::Key>(event->key()), false);
}

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

    	if (game->hasActiveMessage())
        	drawMessagePopup(p);

    	if (game->getStateManager()->isOver() || game->getStateManager()->isPaused())
        	drawEndOverlay(p);
}

void GameWidget::drawBackground(QPainter &p) {
	QLinearGradient sea(0, 0, 0, height());
	sea.setColorAt(0.0, QColor(0, 50, 90));
	sea.setColorAt(0.4, QColor(0, 25, 55));
	sea.setColorAt(1.0, QColor(0, 5, 18));
	p.fillRect(rect(), sea);

	QRandomGenerator rng(99);
	p.setPen(QPen(QColor(255, 255, 255, 18), 1));
	for (int i = 0; i < 120; ++i) {
		int x = rng.bounded(width());
		int y = rng.bounded(height());
		p.drawPoint(x, y);
	}
}

void GameWidget::drawMapObjects(QPainter &p) {
	QPoint cam = game->getCameraOffset();

	drawSunkenShip(p, 400 - cam.x(), 900 - cam.y(), 1.0f);
	drawSunkenShip(p, 1600 - cam.x(), 500 - cam.y(), 1.3f);
	drawSunkenShip(p, 900 - cam.x(), 1200 - cam.y(), 0.8f);

	drawRuinPillar(p, 700 - cam.x(), 800 - cam.y(), 120);
	drawRuinPillar(p, 760 - cam.x(), 820 - cam.y(), 80);
	drawRuinPillar(p, 820 - cam.x(), 790 - cam.y(), 150);
	drawRuinPillar(p, 1200 - cam.x(), 400 - cam.y(), 100);
	drawRuinPillar(p, 1260 - cam.x(), 420 - cam.y(), 130);
	drawRuinPillar(p, 2000 - cam.x(), 1100 - cam.y(), 90);
	drawRuinPillar(p, 2060 - cam.x(), 1080 - cam.y(), 110);

	drawRock(p, 300 - cam.x(), 1000 - cam.y(), 60);
	drawRock(p, 1100 - cam.x(), 700 - cam.y(), 45);
	drawRock(p, 1800 - cam.x(), 1300 - cam.y(), 70);
	drawRock(p, 500 - cam.x(), 400 - cam.y(), 35);
	drawRock(p, 2100 - cam.x(), 600 - cam.y(), 55);

	drawCoral(p, 600 - cam.x(), 950 - cam.y(), QColor(200, 60, 90));
	drawCoral(p, 650 - cam.x(), 970 - cam.y(), QColor(220, 100, 50));
	drawCoral(p, 1000 - cam.x(), 1100 - cam.y(), QColor(180, 40, 120));
	drawCoral(p, 1500 - cam.x(), 800 - cam.y(), QColor(200, 70, 60));
	drawCoral(p, 1900 - cam.x(), 400 - cam.y(), QColor(210, 80, 40));

	drawSeaweed(p, 450 - cam.x(), 1050 - cam.y(), 80);
	drawSeaweed(p, 480 - cam.x(), 1040 - cam.y(), 60);
	drawSeaweed(p, 1300 - cam.x(), 650 - cam.y(), 90);
	drawSeaweed(p, 1750 - cam.x(), 1200 - cam.y(), 70);
	drawSeaweed(p, 850 - cam.x(), 500 - cam.y(), 100);
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
	p.drawEllipse(sc(20), sc(20), sc(18), sc(18));
	p.drawEllipse(sc(55), sc(20), sc(18), sc(18));
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
	p.drawLine(x + 5, y + 50, x, y + 80);
}

void GameWidget::drawCoral(QPainter &p, int x, int y, QColor color) {
	p.setPen(Qt::NoPen);

	for (int i = 0; i < 5; ++i) {
		int bx = x + (i - 2) * 10;
		int bh = 20 + (i % 3) * 15;
		QColor c = color.lighter(90 + i * 8);
		p.setBrush(c);
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

void GameWidget::drawHints(QPainter &p) {
	QPoint cam = game->getCameraOffset();
	for (Hint *h : game->getHints())
		h->draw(p, cam);
}

void GameWidget::drawPlayer(QPainter &p) {
	QPoint cam = game->getCameraOffset();
	game->getPlayer()->draw(p, cam);
}

void GameWidget::drawDarkness(QPainter &p) {
	QPoint worldPos = game->getPlayer()->getPosition();
	QPoint cam = game->getCameraOffset();
	QPointF screenPos = QPointF(worldPos.x() - cam.x(), worldPos.y() - cam.y());

	QRadialGradient torch(screenPos, TORCH_RADIUS);
	torch.setColorAt(0.0, QColor(0, 0, 0, 0));
	torch.setColorAt(0.55, QColor(0, 0, 0, 60));
	torch.setColorAt(0.85, QColor(0, 0, 0, 180));
	torch.setColorAt(1.0, QColor(0, 0, 0, 245));

	p.setBrush(torch);
	p.setPen(Qt::NoPen);
	p.drawRect(rect());

	QRadialGradient warmth(screenPos, TORCH_RADIUS * 0.3);
	warmth.setColorAt(0.0, QColor(255, 180, 80, 18));
	warmth.setColorAt(1.0, QColor(255, 180, 80, 0));
	p.setBrush(warmth);
	p.drawRect(rect());
}

void GameWidget::drawHUD(QPainter &p) {
	float oxygen = game->getPlayer()->getOxygenLevel();
	int hints = game->getHints().size();
	int current = game->getCurrentHintIndex();

	int barW = 200, barH = 18;
	int barX = 20, barY = 20;

	p.setBrush(QColor(0, 0, 0, 140));
	p.setPen(QPen(QColor(255, 255, 255, 40), 1));
	p.drawRoundedRect(barX, barY, barW, barH, 9, 9);

	QColor barColor;
	if (oxygen > 60.0f) barColor = QColor(0, 210, 160);
	else if (oxygen > 30.0f) barColor = QColor(230, 180, 0);
	else barColor = QColor(220, 45, 45);

	int fillW = static_cast<int>(oxygen / 100.0f * barW);
	p.setBrush(barColor);
	p.setPen(Qt::NoPen);
	p.drawRoundedRect(barX, barY, qMax(fillW, 4), barH, 9, 9);

	p.setPen(Qt::white);
	p.setFont(QFont("Courier New", 9, QFont::Bold));
	p.drawText(barX + barW + 8, barY + 13, "O₂");

	p.setPen(QColor(255, 255, 255, 200));
	p.setFont(QFont("Courier New", 8));
	p.drawText(barX + 6, barY + 13, QString::number(static_cast<int>(oxygen)) + "%");

	p.setPen(QColor(255, 220, 80));
	p.setFont(QFont("Courier New", 10, QFont::Bold));
	p.drawText(20, 58, QString("HINT  %1 / %2").arg(current).arg(hints));

	int depth = static_cast<int>(game->getPlayer()->getPosition().y() / 10.0f);
	p.setPen(QColor(140, 200, 255, 180));
	p.setFont(QFont("Courier New", 9));
	p.drawText(20, 76, QString("DEPTH  %1m").arg(depth));
}

void GameWidget::drawMiniMap(QPainter &p) {
	const int mmW = 160, mmH = 100;
	const int mmX = width() - mmW - 16;
	const int mmY = 16;

	float scaleX = static_cast<float>(mmW) / mapWidth;
	float scaleY = static_cast<float>(mmH) / mapHeight;

	p.setBrush(QColor(0, 10, 25, 180));
	p.setPen(QPen(QColor(100, 160, 220, 80), 1));
	p.drawRect(mmX, mmY, mmW, mmH);

	QPoint cam = game->getCameraOffset();
	int vpX = mmX + static_cast<int>(cam.x() * scaleX);
	int vpY = mmY + static_cast<int>(cam.y() * scaleY);
	int vpW = static_cast<int>(800 * scaleX);
	int vpH = static_cast<int>(600 * scaleY);
	p.setBrush(Qt::NoBrush);
	p.setPen(QPen(QColor(255, 255, 255, 50), 1));
	p.drawRect(vpX, vpY, vpW, vpH);

	for (Hint *h : game->getHints()) {
		if (h->isActive() && !h->isCollected()) {
			int hx = mmX + static_cast<int>(h->getPosition().x() * scaleX);
			int hy = mmY + static_cast<int>(h->getPosition().y() * scaleY);
			p.setBrush(QColor(255, 200, 0));
			p.setPen(Qt::NoPen);
			p.drawEllipse(hx - 3, hy - 3, 6, 6);
		}
	}

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

void GameWidget::drawMessagePopup(QPainter &p) {
	int boxW = 500, boxH = 110;
	int boxX = (width() - boxW) / 2;
	int boxY = (height() - boxH) / 2;

	p.setBrush(QColor(0, 15, 35, 220));
	p.setPen(QPen(QColor(80, 160, 220, 180), 2));
	p.drawRoundedRect(boxX, boxY, boxW, boxH, 12, 12);

	p.setPen(QColor(255, 200, 60));
	p.setFont(QFont("Courier New", 11, QFont::Bold));
	p.drawText(boxX + 20, boxY + 28, "✦  CLUE DISCOVERED");

	p.setPen(QColor(200, 230, 255));
	p.setFont(QFont("Courier New", 10));
	p.drawText(QRect(boxX + 20, boxY + 44, boxW - 40, 44), Qt::AlignLeft | Qt::TextWordWrap, game->getCurrentMessage());

	p.setPen(QColor(120, 160, 200, 160));
	p.setFont(QFont("Courier New", 8));
	p.drawText(boxX + boxW - 180, boxY + boxH - 10, "[ SPACE / ENTER to continue ]");
}

void GameWidget::drawEndOverlay(QPainter &p) {
	GameState state = game->getStateManager()->getState();

	p.fillRect(rect(), QColor(0, 0, 0, 170));

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
		p.setPen(QColor(255, 210, 60));
		p.setFont(QFont("Courier New", 34, QFont::Bold));
		p.drawText(QRect(0, height() / 2 - 80, width(), 60), Qt::AlignCenter, "TREASURE FOUND");

		p.setPen(QColor(200, 230, 255));
		p.setFont(QFont("Courier New", 14));
		p.drawText(QRect(0, height() / 2, width(), 36), Qt::AlignCenter,
			QString("You survived with %1% oxygen remaining.")
				.arg(static_cast<int>(game->getPlayer()->getOxygenLevel())));
	}

	if (state == GameState::Lose) {
		p.setPen(QColor(220, 50, 50));
		p.setFont(QFont("Courier New", 34, QFont::Bold));
		p.drawText(QRect(0, height() / 2 - 80, width(), 60), Qt::AlignCenter, "OXYGEN DEPLETED");

		p.setPen(QColor(200, 180, 180));
		p.setFont(QFont("Courier New", 13));
		p.drawText(QRect(0, height() / 2, width(), 36), Qt::AlignCenter, "The depths claimed another soul...");
	}

	p.setPen(QColor(140, 180, 200, 180));
	p.setFont(QFont("Courier New", 11));
	p.drawText(QRect(0, height() / 2 + 80, width(), 30), Qt::AlignCenter, "Press R to dive again   |   ESC to surface");
}
