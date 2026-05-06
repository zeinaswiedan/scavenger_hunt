#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class LoginWindow : public QWidget{
	Q_OBJECT
public:
	explicit LoginWindow(QWidget *parent = nullptr);
private slots:
	void onLoginClicked();

private:
	QLabel *titleLabel;
	QLineEdit *usernameInput;
	QPushButton *loginButton;
	QLabel *errorLabel;

};
