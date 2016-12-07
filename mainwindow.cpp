#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connected = false;
    // The TCP socket should be non-blocking to keep the GUI from hanging

    pingObject.moveToThread(&workerThread);
    QObject::connect(this, &MainWindow::setupConnection, &pingObject, &ServerPing::setupConnection);
    QObject::connect(this, &MainWindow::closeConnection, &pingObject, &ServerPing::closeConnection);
    QObject::connect(this,&MainWindow::clearDatabase, &pingObject, &ServerPing::clearDatabase);
    QObject::connect(this,&MainWindow::sendDatabaseInfo, &databaseInfo, &ChangeUserForDatabase::startDatabaseInfo);
    QObject::connect(&databaseInfo, &ChangeUserForDatabase::sendDatabaseInfo, this, &MainWindow::acceptDatabaseInfo);
    workerThread.start();

}

MainWindow::~MainWindow()
{
    workerThread.quit();
    workerThread.wait();
    delete ui;

}


void MainWindow::on_clearDatabaseButton_clicked()
{
    emit clearDatabase();
}

void MainWindow::on_setupConnectionButton_clicked()
{
    if (!connected)
    {
        ui->setupConnectionButton->setText("Close Connection");
        emit setupConnection();

    }
    else
    {
        emit closeConnection();
        ui->setupConnectionButton->setText("Setup Connection");
    }
    connected = !connected;
}

void MainWindow::on_changeUserAndPassword_clicked()
{
    emit sendDatabaseInfo(pingObject.databaseUser, pingObject.databasePassword);
    databaseInfo.show();
}

void MainWindow::acceptDatabaseInfo(std::string user, std::string pass)
{
    pingObject.databaseUser = user;
    pingObject.databasePassword = pass;
}
