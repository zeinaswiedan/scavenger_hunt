#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QPushButton *startButton;
    QPushButton *exitButton;

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void openGameWindow();
};

#endif
