#ifndef PDFGENERATOR_H
#define PDFGENERATOR_H

#include <hpdf.h>
#include <QString>

#include "mydatabase.h"

#include <fstream>
#include <iostream>

#include <QDebug>
#include <numeric> // Include for std::accumulate

class PDFGenerator {
public:
    PDFGenerator();
    ~PDFGenerator();
    void generatePDF(const QString &filename);

private:
    HPDF_Doc pdf;
    HPDF_Page page;
    HPDF_Font fontDef , fontBold, fontItalic, fontBoldItalic;
    HPDF_REAL pageHeight, pageWidth;

    void initializePDF();
    void drawHeader();
    void drawTable();
    void sqlPrint();

    //const MyDatabase& mMyDatabase;
    MyDatabase mPDFMyDatabase;

    int nextline(int cLine);

    int col1X=55;       //column 1 start x
    int col2X=221;      //column 2 start x
    int col3X=390;      //column 3 start x
    int col4X=476;      //column 4 start x

    int cLine=370;      //Row start y

    double cellWidth = 78.336;

    QList<QList<QString>> listCoulmn1,listCoulmn2,listCoulmn3,listCoulmn4;

    int rFunRCol1=0,rFunRCol2=0,rFunRCol3=0,rFunRCol4=0;

    int mapIterator(QList<QList<QString>> &myList, int rowBrackOn, int &colX, int printColumnNo, int startKey);

    void recursiveFunc(QString dateStringLong);

};

#endif // PDFGENERATOR_H
