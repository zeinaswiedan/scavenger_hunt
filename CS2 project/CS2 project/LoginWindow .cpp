#include "loginwindow.hpp"
#include "mainwindow.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    usernameInput = new QLineEdit();
    usernameInput->setPlaceholderText("Enter username");

    loginButton = new QPushButton("Login");

    layout->addWidget(new QLabel("Scavenger Hunt Login"));
    layout->addWidget(usernameInput);
    layout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked,
            this, &LoginWindow::openMainWindow);
}

void LoginWindow::openMainWindow() {
    MainWindow *menu = new MainWindow();
    menu->show();
    this->close();
}
