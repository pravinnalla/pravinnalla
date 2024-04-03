#include "csvfilehandler.h"

CSVFileHandler::CSVFileHandler(int lCount, QString fPath)
{
    lineCount=lCount;
    filePath=fPath;
}

bool CSVFileHandler::csvSrcCheck()
{
    if(filePathCheck() and filePathExtenCheck())
    {
        std::cout << std::endl << "Reading CSV file please wait." << std::endl;

        int lc=0;

        QFile file(getfilePath());
        QTextStream ts (&file);

        if(file.open(QIODevice::ReadOnly)){
            while(!ts.atEnd()){
                ts.readLine();
                lc++;
            }
            lineCount=lc;   //set line count vaue while file path is already set from constructor
        }

        file.seek(0);   // Reset file position to start
        ts.seek(0);   // Reset textstream position to start

        int randumNum = (int) rand() % (lineCount - 2) + 2; //generate randam number

        QString line;
        int currentLine = 0;

        while (!ts.atEnd()) {
            line = ts.readLine();
            ++currentLine;

            if (currentLine == randumNum) {
                if(regExpCheck(line))
                    return true;            //if filepath is ok, fileextention is ok and randam line text is ok return TRUE
            }
        }
        file.close();

    }

    return false;
}

bool CSVFileHandler::regExpCheck(QString input)
{
    QRegularExpression regex("\"([^\"]*)\",\"([^\"]*/[^\"]*/[^\"]*)\",\"(\\d{2}-\\d{2}-\\d{4})\",\"([^\"]*)\"");
    QRegularExpressionMatch match = regex.match(input);

    if (match.hasMatch()) {

        return true;
    }

    return false;
}

QVector<QVariant> CSVFileHandler::regExpSplitTxt(QString input)
{
    QRegularExpression regex("\"([^\"]*)\",\"([^\"]*/[^\"]*/[^\"]*)\",\"(\\d{2}-\\d{2}-\\d{4})\",\"([^\"]*)\"");
    QRegularExpressionMatch match = regex.match(input);

    QVector<QVariant> splitData;

    if (match.hasMatch()) {
        QString value1 = match.captured(1); // Captured value for the first pattern     srno
        QString value2 = match.captured(2); // Captured value for the second pattern    type, no, year
        QString value3 = match.captured(3); // Captured value for the third pattern     date
        QString value4 = match.captured(4); // Captured value for the third pattern     stage

        //check value3 date value
        if(isValidDate(value3))
        {
            QRegularExpression regex1("[,/]");
            QStringList parts = value2.split(regex1);       //splited value2 into - type, no, year


            splitData.append(QVariant(value1));
            for (const QString &part : parts) {
                splitData.append(QVariant(part));
            }
            QString tmpdate=convertToSQLiteFormat(value3);  //temp      //check for empty not convertable date
            splitData.append(QVariant(tmpdate));
            splitData.append(QVariant(value4));

            return splitData;
        }

    }
    return splitData;
}

QString CSVFileHandler::convertToSQLiteFormat(const QString &dateString)
{
    QString inputFormat = "dd-MM-yyyy"; // Input format
    QString outputFormat = "yyyy-MM-dd"; // Output format for SQLite

    QDate date = QDate::fromString(dateString, inputFormat);

    if (date.isValid()) {
        QString formattedDate = date.toString(outputFormat);
        return formattedDate;
    } else {
        return QString(); // Return an empty string for an invalid date
    }
}

bool CSVFileHandler::isValidDate(const QString &dateString)
{
    // Define the format expected for the date string
    QString format = "dd-MM-yyyy";

    // Attempt to convert the string to a QDate using the specified format
    QDate date = QDate::fromString(dateString, format);

    // Check if the conversion was successful and the date is valid
    if (date.isValid()) {
        return true;
    } else {
        return false;
    }
}


bool CSVFileHandler::filePathCheck()
{
    QFile file(getfilePath());       //QFile f(filename.toUtf8());
    if (file.exists()) {
        return true;
    }
    std::cout << std::endl << "Source file path doesn't exist !" << std::endl;
    return false;
}

bool CSVFileHandler::filePathExtenCheck()
{
    if(filePath.toLower().endsWith(".csv")){
        return true;
    }
    std::cout << std::endl << "Source file is not valid CSV file !" << std::endl;
    return false;
}


int CSVFileHandler::getlineCount()
{
    return lineCount;
}

QString CSVFileHandler::getfilePath()
{
    return filePath;
}
