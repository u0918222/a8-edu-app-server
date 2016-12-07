#ifndef CHANGEUSERFORDATABASE_H
#define CHANGEUSERFORDATABASE_H

#include <QDialog>

namespace Ui {
class ChangeUserForDatabase;
}

class ChangeUserForDatabase : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeUserForDatabase(QWidget *parent = 0);
    ~ChangeUserForDatabase();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

public slots:
    void startDatabaseInfo(std::string, std::string);

signals:
    void sendDatabaseInfo(std::string, std::string);

private:
    Ui::ChangeUserForDatabase *ui;
};

#endif // CHANGEUSERFORDATABASE_H
