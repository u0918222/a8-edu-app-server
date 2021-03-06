#include "ServerPing.h"

ServerPing::ServerPing(QObject *parent) : QObject(parent)
{
    // The TCP socket should be non-blocking to keep the GUI from hanging
    //client.setBlocking(false);
    terminated = false;
    host = "localhost";
    databaseUser = "admin";
    databasePassword = "yogapants";
}

ServerPing::~ServerPing()
{
    for (std::list<sf::TcpSocket*>::iterator it = sockets.begin(); it != sockets.end(); ++it)
    {
        sf::TcpSocket& current = **it;
        current.disconnect();
    }
    listener.close();
    sockets.clear();
    Selector.clear();
}

void ServerPing::setupConnection()
{
    listener.listen(53001);
    Selector.add(listener);
    terminated = false;

    // Start checking for packets
    QTimer::singleShot(100, this, SLOT(getPackets()));

}

void ServerPing::closeConnection()
{
    std::cout << "Disconnecting " << std::endl;
    for (std::list<sf::TcpSocket*>::iterator it = sockets.begin(); it != sockets.end(); ++it)
    {
        sf::TcpSocket& current = **it;
        current.disconnect();
    }
    listener.close();
    sockets.clear();
    Selector.clear();
    terminated = true;
}

// Accept incoming information from the client.
// Thanks to SFML Documentation for the example related to this code
// Find it here: http://www.sfml-dev.org/documentation/2.0/classsf_1_1SocketSelector.php
void ServerPing::getPackets()
{
    // Make the selector wait for data on any socket
    if(Selector.wait())
    {
        // Test the listener
        if (Selector.isReady(listener))
        {
            // The listener is ready: there is a pending connection
            sf::TcpSocket* current = new sf::TcpSocket;
            if(listener.accept(*current) == sf::Socket::Done)
            {
                std::cout<< "Connected to user" << std::endl;
                // Add the new current client to the sockets list
                sockets.push_back(current);

                //Add the new current client to the selector so that we will be
                //notified when he sends something
                Selector.add(*current);
            }
            else
            {
                //Error, delete this socket
                delete current;
            }
        }
        else
        {
            std::list<sf::TcpSocket*> socketsToRemove;
            for (std::list<sf::TcpSocket*>::iterator it = sockets.begin(); it != sockets.end(); ++it)
            {
                sf::TcpSocket& current = **it;
                if(Selector.isReady(current))
                {
                    sf::Packet packet;
                    //QApplication::flush();
                    std::string msg = "";
                    if (current.receive(packet) == sf::Socket::Done)
                    {
                        // The client has sent some data, we can receive it
                        packet >> msg;
                        std::cout << "Msg received: " << msg << std::endl;
                        if(msg != "")
                        {
                            std::regex insertKey("(INSERT)(.*)");
                            std::regex selectKey("(SELECT)(.*)");
                            std::regex deleteKey("(DELETE)(.*)");
                            std::regex updateKey("(UPDATE)(.*)");
                            std::regex connectedKey("(Ping)");
                            std::regex terminateKey("TerminateConnection");

                            if(std::regex_match(msg,insertKey))
                            {
                                std::cout<<"call CreateAccout"<<std::endl;
                                createAccount(msg,current);
                            }
                            else if(std::regex_match(msg,selectKey))
                            {
                                accessDatabase(msg,current);
                            }
                            else if(std::regex_match(msg, deleteKey))
                            {
                                deleteAnEntry(msg,current);
                            }
                            else if(std::regex_match(msg, updateKey))
                            {
                                updateAnEntry(msg, current);
                            }
                            else if(std::regex_match(msg,connectedKey))
                            {
                                std::vector<std::string> result;
                                result.push_back("connected");
                                createAndSentPackets(result,current);
                            }
                            else if(std::regex_match(msg, terminateKey))
                            {
                                current.disconnect();
                                socketsToRemove.push_back(&current);
                            }
                        }
                    }
                }
            }

        }
    }
    if(!terminated)
    {
        // Use a timer to keep checking for packets while keeping the GUI alive
        QTimer::singleShot(100, this, SLOT(getPackets()));
    }


}
void ServerPing::createAndSentPackets(std::vector<std::string> data, sf::TcpSocket& client)
{
    sf::Packet packet;
    packet << static_cast<sf::Uint32>(data.size());//Should we restrict size of the strings?
    for (unsigned int i = 0; i < data.size(); i++)
    {
        packet << data[i];
    }
    client.send(packet);
}


void ServerPing::printDatabaseResult(std::vector<std::string> result)
{
    for(unsigned int i =0; i<result.size();i++)
    {
        std::cout << result[i] << std::endl;
    }
}
//Code for this database access is pulled from tutorial at http://blog.davenicholas.me.uk/2013/04/how-to-c-mysql-mac-osx.html
//Provided by the professor to learn to use c++ and MySQL together.
void ServerPing::accessDatabase(std::string queryString, sf::TcpSocket& client)
{
    std::regex checkUser(".*WHERE username\\s*=\\s*'[a-zA-Z]+[a-zA-Z0-9]*'\\s*(AND.*)?");
    std::vector<std::string> userInfo;
    MYSQL_RES *result;
    MYSQL_ROW row;
    MYSQL *connection;
    MYSQL mysql;

    int state;

    if(!std::regex_match(queryString,checkUser))
    {
        createAndSentPackets(userInfo,client);
    }
    else
    {
        mysql_init(&mysql);

        connection = mysql_real_connect(&mysql,host.c_str(),databaseUser.c_str(),databasePassword.c_str(),"beegameinfo",3306,0,0);  //henry's password = Hk040696

        if (connection == NULL)
        {
            std::cout << mysql_error(&mysql) << std::endl;

            createAndSentPackets(userInfo, client);
        }

        state = mysql_query(connection, queryString.c_str());
        if (state !=0)
        {
            std::cout << mysql_error(connection) << std::endl;
            createAndSentPackets(userInfo, client);
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

        createAndSentPackets(userInfo,client);
    }
}

bool ServerPing::insertIntoDatabase(std::string insertString)
{
    std::regex checkUser(".*WHERE username\\s*=\\s*'[a-zA-Z]+[a-zA-Z0-9]*'\\s*(AND.*)?");
    MYSQL *connection;
    MYSQL mysql;

    int state;
    if(!std::regex_match(insertString, checkUser))
    {
        return false;
    }
    else
    {
        mysql_init(&mysql);

        connection = mysql_real_connect(&mysql,host.c_str(),databaseUser.c_str(),databasePassword.c_str(),"beegameinfo",3306,0,0);

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
}
void ServerPing::createAccount(std::string query, sf::TcpSocket& client)
{
    std::regex checkUser(".*NULL,\\s*'[a-zA-Z]+[a-zA-Z0-9]*'\\s*,.*");
    std::vector<std::string> result;
    MYSQL *connection;
    MYSQL mysql;

    int state;

    std::string errorReport = "OK";
    if(!std::regex_match(query, checkUser))
    {
        errorReport = "You put in an invalid character to your username.";
        result.push_back(errorReport);
        createAndSentPackets(result, client);
    }
    else
    {
        mysql_init(&mysql);

        connection = mysql_real_connect(&mysql,host.c_str(),databaseUser.c_str(),databasePassword.c_str(),"beegameinfo",3306,0,0);

        if (connection == NULL)
        {
            std::cout << mysql_error(&mysql) << std::endl;
            errorReport = mysql_error(&mysql);

            result.push_back(errorReport);
            createAndSentPackets(result, client);
        }
        std::string insertString = query;
        state = mysql_query(connection, insertString.c_str());
        if (state !=0)
        {
            std::cout << mysql_error(connection) << std::endl;
            errorReport = mysql_error(connection);

            result.push_back(errorReport);
            createAndSentPackets(result, client);
        }

        mysql_close(connection);

        result.push_back(errorReport);
        createAndSentPackets(result, client);
    }
}

void ServerPing::stringToUpper(std::string &inputString)
{
    for(unsigned int i = 0; i < inputString.length(); i++)
    {
        inputString[i] = ::toupper(inputString[i]);
    }
}

void ServerPing::clearDatabase()
{
    std::string clearTableString = "TRUNCATE gamedata";
    MYSQL *connection;
    MYSQL mysql;

    int state;

    mysql_init(&mysql);

    connection = mysql_real_connect(&mysql,host.c_str(),databaseUser.c_str(),databasePassword.c_str(),"beegameinfo",3306,0,0);

    if (connection == NULL)
    {
        std::cout << mysql_error(&mysql) << std::endl;

    }

    state = mysql_query(connection, clearTableString.c_str());
    if (state !=0)
    {
        std::cout << mysql_error(connection) << std::endl;

    }

    mysql_close(connection);
}

void ServerPing::deleteAnEntry(std::string deleteString, sf::TcpSocket & current)
{
    std::regex checkUser(".*WHERE username\\s*=\\s*'[a-zA-Z]+[a-zA-Z0-9]*'\\s*(AND.*)?");
    MYSQL *connection;
    MYSQL mysql;

    int state;
    std::string deleteReport = "deleted";
    std::vector<std::string> result;

    if(!std::regex_match(deleteString,checkUser))
    {
        deleteReport = "You messed up your username string with some special character.";
        result.push_back(deleteReport);
        createAndSentPackets(result, current);
    }
    else
    {
        mysql_init(&mysql);

        connection = mysql_real_connect(&mysql,host.c_str(),databaseUser.c_str(),databasePassword.c_str(),"beegameinfo",3306,0,0);

        if (connection == NULL)
        {
            std::cout << mysql_error(&mysql) << std::endl;
            deleteReport = mysql_error(&mysql);

        }

        state = mysql_query(connection, deleteString.c_str());
        if (state !=0)
        {
            std::cout << mysql_error(connection) << std::endl;
            deleteReport = mysql_error(connection);
        }

        mysql_close(connection);

        result.push_back(deleteReport);
        createAndSentPackets(result, current);
    }
}

void ServerPing::updateAnEntry(std::string updateString, sf::TcpSocket& current)
{
    std::regex checkUser(".*WHERE username\\s*=\\s*'[a-zA-Z]+[a-zA-Z0-9]*'\\s*(AND.*)?");
    std::string updateReport = "updated";
    std::vector<std::string> result;

    MYSQL *connection;
    MYSQL mysql;

    int state;
    if(!std::regex_match(updateString, checkUser))
    {
        updateReport = "You entered your username with an invalid character.";
        result.push_back(updateReport);
        createAndSentPackets(result, current);
    }
    else
    {
        mysql_init(&mysql);

        connection = mysql_real_connect(&mysql,host.c_str(),databaseUser.c_str(),databasePassword.c_str(),"beegameinfo",3306,0,0);

        if (connection == NULL)
        {
            std::cout << mysql_error(&mysql) << std::endl;
            updateReport = mysql_error(&mysql);
        }

        state = mysql_query(connection, updateString.c_str());
        if (state !=0)
        {
            std::cout << mysql_error(connection) << std::endl;
            updateReport = mysql_error(connection);
        }
        mysql_close(connection);

        result.push_back(updateReport);
        createAndSentPackets(result, current);
    }

}

