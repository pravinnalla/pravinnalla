#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <QtSql>
#include <QFile>

class MyDatabase {
public:
    MyDatabase();
    ~MyDatabase();

    bool openDatabase();
    bool createDatabase();
    bool deleteDatabase();
    bool createTable();
    bool insertData(const QString& caseType, const int &caseNo, const int &caseYear, const QString& nextDate, const QString& stage);
    void closeDatabase();

private:
    QSqlDatabase db;
};

#endif // MYDATABASE_H
