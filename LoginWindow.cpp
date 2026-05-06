#include "LoginWindow.h"
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QString>
LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent){
	setWindowTitle("Scavenger Hunt - Login");
	setFixedSize(360, 260);
	titleLabel = new QLabel("🎯 Scavenger Hunt");
	usernameInput = new QLineEdit();
	loginButton = new QPushButton("Start Game");
	errorLabel    = new QLabel("");

	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; margin-bottom: 8px;");
	usernameInput->setPlaceholderText("Username");
	loginButton->setStyleSheet("padding: 8px; font-size: 14px;");
	errorLabel->setStyleSheet("color: red;");
	errorLabel->setAlignment(Qt::AlignCenter);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(40, 30, 40, 30);
	layout->setSpacing(12);
	layout->addWidget(titleLabel);
	layout->addWidget(usernameInput);
	layout->addWidget(loginButton);
	layout->addWidget(errorLabel);

	connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked(){
	QString user = usernameInput->text().trimmed();
	if(user.isEmpty()){
		errorLabel->setText("Please enter your username.");
		return;
	}
	MainWindow *main = new MainWindow(user);
	main->show();
	this->close();


}
