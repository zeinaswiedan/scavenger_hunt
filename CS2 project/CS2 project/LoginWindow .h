#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

class QLineEdit;
class QPushButton;

class LoginWindow : public QWidget {
    Q_OBJECT

private:
    QLineEdit *usernameInput;
    QPushButton *loginButton;

public:
    LoginWindow(QWidget *parent = nullptr);

private slots:
    void openMainWindow();
};

#endif
