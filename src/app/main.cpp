#include <QCoreApplication>

#include <fstream>
#include <iostream>
#include <QCommandLineParser>
#include <thread> // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::milliseconds

#include "csvfilehandler.h"
#include "mydatabase.h"
#include "pdfgenerator.h"

#include <QDebug>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyDatabase mMyDatabase;
    mMyDatabase.createDatabase();
    mMyDatabase.openDatabase();
    mMyDatabase.createTable();

    //PDFGenerator mpdfGenerator(mMyDatabase);
    PDFGenerator mpdfGenerator;


    QCoreApplication::setApplicationName("pravinnalla");
    QCoreApplication::setApplicationVersion("V-1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("This command parse CSV file and genarate PDF file");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add command line options for source and destination files
    parser.addPositionalArgument("source", "Source CSV file to parse.");
    parser.addPositionalArgument("destination", "Destination PDF file.");

    parser.process(a);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 2) {
        std::cerr << "Please provide both source CSV file and destination file path for pdf.";
        return 1;
    }

    QString filePath = args.at(0).toUtf8();
    QString pdfFilePath = args.at(1);

    QFileInfo fileInfo(pdfFilePath);
    if (fileInfo.suffix().toLower() != "pdf") {
        pdfFilePath = fileInfo.completeBaseName() + ".pdf";
    }

    // Get the directory path from the destination file
    QDir destinationDir(QFileInfo(pdfFilePath).absolutePath());

    // Check if the directory exists
    if (!destinationDir.exists()) {
        std::cerr << "Destination directory doesn't exist!";
        return 1;
    }


//----------------CSVFileHandler-----------

    CSVFileHandler mCsvFileHandler(0,filePath);

    if(mCsvFileHandler.csvSrcCheck())
    {
        // if file is ok then Now use thread to read file
        QFile file(mCsvFileHandler.getfilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            std::cerr << "Error: Unable to open file at path: " << filePath.toStdString() << std::endl;
            return 1;
        }

        QTextStream in(&file);

        // Reset file position to start
        file.seek(0);

        int linesProcessed = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            linesProcessed++;

            QVector<QVariant> splitedData = mCsvFileHandler.regExpSplitTxt(line);

            if (splitedData.isEmpty()) {
            } else {                
                if(!mMyDatabase.insertData(splitedData[1].toString(),splitedData[2].toInt(),splitedData[3].toInt(),splitedData[4].toString(),splitedData[5].toString()))
                {
                    std::cout << std::endl << "Unable to insert data" << std::endl;
                }
            }


            //-------- progressbar output start
            // Simulate work
            std::this_thread::sleep_for(std::chrono::microseconds(250));

            // Calculate progress based on the number of lines processed
            int progress = static_cast<int>((linesProcessed * 100.0) / mCsvFileHandler.getlineCount());

            // Display progress bar
            int barWidth = 70;
            std::cout << "[";
            int pos = static_cast<int>(barWidth * progress / 100.0);
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos)
                    std::cout << "=";
                else if (i == pos)
                    std::cout << ">";
                else
                    std::cout << " ";
            }
            std::cout << "] " << progress << "%\r";
            std::cout.flush(); // Flush the output buffer to display progress immediately
        }

        std::cout << std::endl << "File reading completed successfully." << std::endl;
        //progressbar output close

        file.close();


        mpdfGenerator.generatePDF(pdfFilePath);


        mMyDatabase.deleteDatabase();

        //display github repo path at end
        std::cout << "GitHub repo at https://github.com/pravinnalla/praveennalla.git" << std::endl;

        return 0;

    }else {

        return 1;
    }

    return a.exec();
}
