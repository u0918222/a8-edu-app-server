#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SFML/Network.hpp>
#include <mysql.h>
#include <iostream>
#include <QTimer>
#include <regex>
#include <thread>
#include <QThread>
#include <ServerPing.h>
#include <ChangeUserForDatabase.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void setupConnection();
    void closeConnection();
    void clearDatabase();
    void sendDatabaseInfo(std::string, std::string);

public slots:
    void acceptDatabaseInfo(std::string,std::string);

private slots:

    void on_clearDatabaseButton_clicked();

    void on_setupConnectionButton_clicked();

    void on_changeUserAndPassword_clicked();

private:
    ServerPing pingObject;
    Ui::MainWindow *ui;
    // Open a listener on a port
    sf::TcpListener listener;
    ChangeUserForDatabase databaseInfo;

    bool connected;

    void accessDatabase(std::string);
    bool insertIntoDatabase(std::string);
    void stringToUpper(std::string &);    //convert a string to uppercase a char at a time
    void printDatabaseResult(std::vector<std::string>);     //takes in the vector of strings returned from query into the database and prints them
    void createAccount(std::string);  //create an account returns a string incase there is an error then the error message is displayed
    void createAndSentPackets(std::vector<std::string> data);
};

#endif // MAINWINDOW_H
