#include "ChangeUserForDatabase.h"
#include "ui_ChangeUserForDatabase.h"

ChangeUserForDatabase::ChangeUserForDatabase(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeUserForDatabase)
{
    ui->setupUi(this);

}

ChangeUserForDatabase::~ChangeUserForDatabase()
{
    delete ui;
}

void ChangeUserForDatabase::on_buttonBox_accepted()
{
    emit sendDatabaseInfo(ui->usernameInput->text().toStdString(), ui->passwordInput->text().toStdString());
}

void ChangeUserForDatabase::on_buttonBox_rejected()
{
    //do nothing
}

void ChangeUserForDatabase::startDatabaseInfo(std::string user, std::string pass)
{
    ui->usernameInput->setText(QString(user.c_str()));
    ui->passwordInput->setText(QString(pass.c_str()));
}
