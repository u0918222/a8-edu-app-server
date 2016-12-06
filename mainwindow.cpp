#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connected = false;
    // The TCP socket should be non-blocking to keep the GUI from hanging
    //client.setBlocking(false);
    //listener.listen(53001);
    pingObject.moveToThread(&workerThread);
    QObject::connect(this, &MainWindow::setupConnection, &pingObject, &ServerPing::setupConnection);
    QObject::connect(this, &MainWindow::closeConnection, &pingObject, &ServerPing::closeConnection);
    QObject::connect(this,&MainWindow::clearDatabase, &pingObject, &ServerPing::clearDatabase);
    workerThread.start();

}

MainWindow::~MainWindow()
{
    workerThread.quit();
    workerThread.wait();
    delete ui;

}

/*
void MainWindow::setupConnection()
{
    std::cout << "Waiting for client" << std::endl;
    listener.accept(client);
    std::cout << "Connected to client" << std::endl;
    // Start checking for packets
    QTimer::singleShot(100, this, SLOT(getPackets()));

}

void MainWindow::closeConnection()
{
    std::cout << "Disconnecting " << std::endl;
    client.disconnect();
    //connected = false;
    listener.listen(53001);
}



void MainWindow::getPackets()
{
    sf::Packet packet;
    QApplication::flush();
    std::string msg = "";
    if (client.receive(packet) == sf::Socket::Done)
    {
        // The client has sent some data, we can receive it
        packet >> msg;
        std::cout << "Msg received: " << msg << std::endl;
        if(msg != "")
        {
            std::regex insertKey("(INSERT)(.*)");
            std::regex selectKey("(SELECT)(.*)");
            std::regex connectedKey("(ping)");
            if(std::regex_match(msg,insertKey))
            {
                std::cout<<"call CreateAccout"<<std::endl;
                createAccount(msg);
            }
            else if(std::regex_match(msg,selectKey))
            {
                accessDatabase(msg);
            }
            else if(std::regex_match(msg,connectedKey))
            {
                std::vector<std::string> result;
                result.push_back("connected");
                createAndSentPackets(result);
            }
        }
    }
    // Use a timer to keep checking for packets while keeping the GUI alive
    QTimer::singleShot(100, this, SLOT(getPackets()));
}

void MainWindow::createAndSentPackets(std::vector<std::string> data)
{
    sf::Packet packet;
    packet << static_cast<sf::Uint32>(data.size());//Should we restrict size of the strings?
    for (unsigned int i = 0; i < data.size(); i++)
    {
        packet << data[i];
    }
    client.send(packet);
}*/


void MainWindow::on_pushButton_clicked()
{
    if (!connected)
    {
        ui->pushButton->setText("Close Connection");
        emit setupConnection();

    }
    else
    {
        emit closeConnection();
        ui->pushButton->setText("Setup Connection");
    }
    connected = !connected;
}

/*
void MainWindow::printDatabaseResult(std::vector<std::string> result)
{
    for(unsigned int i =0; i<result.size();i++)
    {
        std::cout << result[i] << std::endl;
    }
}
//Code for this database access is pulled from tutorial at http://blog.davenicholas.me.uk/2013/04/how-to-c-mysql-mac-osx.html
//Provided by the professor to learn to use c++ and MySQL together.
void MainWindow::accessDatabase(std::string queryString)
{
    std::vector<std::string> userInfo;
    MYSQL_RES *result;
    MYSQL_ROW row;
    MYSQL *connection;
    MYSQL mysql;

    int state;

    mysql_init(&mysql);

    connection = mysql_real_connect(&mysql,"localhost","root","Hk040696","beegameinfo",3306,0,0);  //henry's password = Hk040696

    if (connection == NULL)
    {
        std::cout << mysql_error(&mysql) << std::endl;

        createAndSentPackets(userInfo);
    }

    state = mysql_query(connection, queryString.c_str());
    if (state !=0)
    {
        std::cout << mysql_error(connection) << std::endl;
        createAndSentPackets(userInfo);
    }

    result = mysql_store_result(connection);

    while ( ( row=mysql_fetch_row(result)) != NULL )
    {
        for(unsigned int i = 0; i < mysql_num_fields(result); i++)
        {
            userInfo.push_back(row[i]);
        }
    }

    mysql_free_result(result);

    mysql_close(connection);

    createAndSentPackets(userInfo);
}
bool MainWindow::insertIntoDatabase(std::string insertString)
{
    MYSQL *connection;
    MYSQL mysql;

    int state;

    mysql_init(&mysql);

    connection = mysql_real_connect(&mysql,"localhost","root","Hk040696","beegameinfo",3306,0,0);

    if (connection == NULL)
    {
        std::cout << mysql_error(&mysql) << std::endl;
        return false;
    }

    state = mysql_query(connection, insertString.c_str());
    if (state !=0)
    {
        std::cout << mysql_error(connection) << std::endl;
        return false;
    }

    mysql_close(connection);

    return true;
}
void MainWindow::createAccount(std::string query)
{
    std::vector<std::string> result;
    MYSQL *connection;
    MYSQL mysql;

    int state;
    std::string errorReport = "OK";

    mysql_init(&mysql);

    connection = mysql_real_connect(&mysql,"localhost","root","Hk040696","beegameinfo",3306,0,0);

    if (connection == NULL)
    {
        std::cout << mysql_error(&mysql) << std::endl;
        errorReport = mysql_error(&mysql);

        result.push_back(errorReport);
        createAndSentPackets(result);
    }
    std::string insertString = query;
    state = mysql_query(connection, insertString.c_str());
    if (state !=0)
    {
        std::cout << mysql_error(connection) << std::endl;
        errorReport = mysql_error(connection);

        result.push_back(errorReport);
        createAndSentPackets(result);
    }

    mysql_close(connection);

    result.push_back(errorReport);
    createAndSentPackets(result);
}

void MainWindow::stringToUpper(std::string &inputString)
{
    for(unsigned int i = 0; i < inputString.length(); i++)
    {
        inputString[i] = ::toupper(inputString[i]);
    }
}

bool MainWindow::clearDatabase()
{
    std::string clearTableString = "TRUNCATE gamedata";
    MYSQL *connection;
    MYSQL mysql;

    int state;

    mysql_init(&mysql);

    connection = mysql_real_connect(&mysql,"localhost","root","Hk040696","beegameinfo",3306,0,0);

    if (connection == NULL)
    {
        std::cout << mysql_error(&mysql) << std::endl;
        return false;
    }

    state = mysql_query(connection, clearTableString.c_str());
    if (state !=0)
    {
        std::cout << mysql_error(connection) << std::endl;
        return false;
    }

    mysql_close(connection);

    return true;
}*/

void MainWindow::on_clearDatabaseButton_clicked()
{
    emit clearDatabase();
}
