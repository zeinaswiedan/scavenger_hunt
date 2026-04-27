#include "mainwindow.hpp"
#include "gamewindow.hpp"

#include <QVBoxLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *central = new QWidget();
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(central);

    startButton = new QPushButton("Start Game");
    exitButton = new QPushButton("Exit");

    layout->addWidget(startButton);
    layout->addWidget(exitButton);

    connect(startButton, &QPushButton::clicked,
            this, &MainWindow::openGameWindow);

    connect(exitButton, &QPushButton::clicked,
            this, &MainWindow::close);
}

void MainWindow::openGameWindow() {
    GameWindow *game = new GameWindow();
    game->show();
    this->close();
}
