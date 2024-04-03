#include "mydatabase.h"

MyDatabase::MyDatabase() {    
}

MyDatabase::~MyDatabase() {
    if (db.isOpen()) {
        db.close();
    }
}

bool MyDatabase::openDatabase() {
    if (!QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::addDatabase("QSQLITE", "qt_sql_default_connection");
    } else {
        db = QSqlDatabase::database("qt_sql_default_connection");
    }
    db.setDatabaseName("pn.tmp");
    if (!db.open()) {
        qDebug() << "Error: Failed to open database" << db.lastError();
        return false;
    }
    return true;
}

bool MyDatabase::createDatabase() {

    QFile databaseFile("pn.tmp");
    if (!databaseFile.exists()) {
        if (!openDatabase()) {
            qDebug() << "Error: Failed to create database";
            return false;
        }
        db.close();
    }
    return true;
}

bool MyDatabase::deleteDatabase() {
    db.close();
    return QFile::remove("pn.tmp");
}

bool MyDatabase::createTable() {
    QSqlQuery query;
    QString queryString = "CREATE TABLE IF NOT EXISTS tblMain ("
                          "id INTEGER PRIMARY KEY,"
                          "casetype TEXT,"
                          "caseno TEXT,"
                          "caseyear TEXT,"
                          "nextdate TEXT,"
                          "stage TEXT)";
    if (!query.exec(queryString)) {
        qDebug() << "Error: Failed to create table" << query.lastError();
        return false;
    }
    return true;
}


bool MyDatabase::insertData(const QString& caseType, const int& caseNo, const int& caseYear, const QString& nextDate, const QString& stage) {


    QSqlQuery pragmS1;
    pragmS1.exec("PRAGMA synchronous = OFF");
    QSqlQuery pragmJ1;
    pragmJ1.exec("PRAGMA journal_mode = MEMORY");



    QSqlQuery query;
    query.prepare("INSERT INTO tblMain (casetype, caseno, caseyear, nextdate, stage) VALUES (:casetype, :caseno, :caseyear, :nextdate, :stage)");
    query.bindValue(":casetype", caseType);
    query.bindValue(":caseno", caseNo);
    query.bindValue(":caseyear", caseYear);
    query.bindValue(":nextdate", nextDate);
    query.bindValue(":stage", stage);
     if (!query.exec()) {
         qDebug() << "Error: Failed to insert data" << query.lastError();
         return false;
     }
     return true;
}

void MyDatabase::closeDatabase()
{
    if (db.isOpen()) {
        db.close();
    }
}

