#ifndef CSVFILEHANDLER_H
#define CSVFILEHANDLER_H

#include <QDebug>
#include <QString>
#include <QFile>

#include <QRegularExpression>
#include <QDate>

#include <fstream>
#include <iostream>


class CSVFileHandler
{
public:
    CSVFileHandler(int lCount, QString fPath);

    bool csvSrcCheck();

    int getlineCount();
    QString getfilePath();

    bool filePathCheck();
    bool filePathExtenCheck();
    bool regExpCheck(QString input);


    QVector<QVariant> regExpSplitTxt(QString input);
    bool isValidDate(const QString &dateString);
    QString convertToSQLiteFormat(const QString &dateString);
private:

    int lineCount;
    QString filePath;
};

#endif // CSVFILEHANDLER_H
