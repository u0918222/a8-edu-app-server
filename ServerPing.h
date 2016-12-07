#ifndef SERVERPING_H
#define SERVERPING_H

#include <QObject>
#include <iostream>
#include <QTimer>
#include <regex>
#include <SFML/Network.hpp>
#include <mysql.h>
#include <QMainWindow>

class ServerPing : public QObject
{
    Q_OBJECT
public:
    explicit ServerPing(QObject *parent = 0);
    ~ServerPing();

signals:

public slots:
    void setupConnection();
    void closeConnection();
    void clearDatabase();
private slots:
    void getPackets();

private:
    std::list<sf::TcpSocket*> sockets;

    sf::SocketSelector Selector;
    // Open a listener on a port
    sf::TcpListener listener;
    // Talk over a TCP socket
    //sf::TcpSocket client;
    bool terminated;

    void accessDatabase(std::string, sf::TcpSocket&);
    bool insertIntoDatabase(std::string);

    void stringToUpper(std::string &);    //convert a string to uppercase a char at a time
    void printDatabaseResult(std::vector<std::string>);     //takes in the vector of strings returned from query into the database and prints them
    void createAccount(std::string, sf::TcpSocket&);  //create an account returns a string incase there is an error then the error message is displayed
    void createAndSentPackets(std::vector<std::string> data, sf::TcpSocket&);

    std::string databaseUser;
    std::string databasePassword;
    std::string host;

};

#endif // SERVERPING_H
