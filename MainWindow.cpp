#include "MainWindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
MainWindow::MainWindow(const QString &username, QWidget *parent)
	: QMainWindow(parent), playerName(username)
{
	setWindowTitle("Scavenger Hunt");
	setFixedSize(800, 600);
	QWidget *central = new QWidget(this);
	setCentralWidget(central);

	welcomeLabel = new QLabel("Welcome, " + playerName + "!\nReady to hunt?");
	startButton  = new QPushButton("▶  Start Game");
	exitButton   = new QPushButton("✕  Exit");

	welcomeLabel->setAlignment(Qt::AlignCenter);
	welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
	startButton->setStyleSheet("font-size: 16px; padding: 10px 40px;");
	exitButton->setStyleSheet("font-size: 14px; padding: 8px 40px; color: gray;");

	QVBoxLayout *layout = new QVBoxLayout(central);
	layout->setAlignment(Qt::AlignCenter);
	layout->setSpacing(20);
	layout->addWidget(welcomeLabel);
	layout->addWidget(startButton);
	layout->addWidget(exitButton);

	connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartGame);
	connect(exitButton,  &QPushButton::clicked, this, &MainWindow::onExitGame);

}

void MainWindow::onStartGame(){
	gameWidget = new GameWidget(playerName, 2400, 1600, this);
	setCentralWidget(gameWidget);
	gameWidget->setFocus();
}

void MainWindow::onExitGame(){
	this->close();
}
