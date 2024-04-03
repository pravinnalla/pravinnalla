#include "pdfgenerator.h"


PDFGenerator::PDFGenerator()
{

}

PDFGenerator::~PDFGenerator() {
    HPDF_Free(pdf);
}


void PDFGenerator::initializePDF() {

    pdf = HPDF_New(NULL, NULL);
    if (!pdf) {
        qDebug() << "Error: Cannot create PDF document.";
    }

    page = HPDF_AddPage(pdf);
    if (!page) {
        qDebug() << "Error: Cannot add page to the document.";
    }
    HPDF_Page_SetSize  (page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);


    fontDef = HPDF_GetFont(pdf, "Times-Roman", NULL);
    fontBold = HPDF_GetFont(pdf, "Times-Bold", NULL);
    fontItalic = HPDF_GetFont(pdf, "Times-Italic", NULL);
    fontBoldItalic = HPDF_GetFont(pdf, "Times-BoldItalic", NULL);

    pageHeight = HPDF_Page_GetHeight(page);
    pageWidth = HPDF_Page_GetWidth(page);

}

void PDFGenerator::generatePDF(const QString &filename) {


    QByteArray ba = filename.toUtf8();
    const char *filePath = ba.constData();

    initializePDF();

    drawHeader();
    drawTable();

    sqlPrint();

    HPDF_SaveToFile(pdf, filePath);

    std::cout << "PDF generated sucessfully." << std::endl;



}
void PDFGenerator::sqlPrint(){

    mPDFMyDatabase.openDatabase();

    QSqlQuery qryDate;
    qryDate.exec("SELECT DISTINCT nextdate from tblMain ORDER BY nextdate");
    while (qryDate.next())
    {
        //Print Date String on pdf page
        QString dateString = qryDate.value(0).toString();
        QDate qryDt;
        qryDt = QDate::fromString(dateString, "yyyy-MM-dd");
        QString dateStringLong = "Day the " + qryDt.toString("dddd, 'of' dd MMMM, yyyy");
        QByteArray qBytedateString = dateStringLong.toLatin1();
        const char *date_title = qBytedateString.data();

        HPDF_Page_SetFontAndSize (page, fontBold, 15);
        int tw = HPDF_Page_TextWidth (page, date_title);  //text width of "date_title"
        HPDF_Page_BeginText (page);
        HPDF_Page_TextOut (page, (pageWidth - tw) / 2, pageHeight - 90, date_title);
        HPDF_Page_EndText (page);

        //pageWidth 841.89
        //pageHeight 595.276

        QSqlQuery qryForQList;
        qryForQList.prepare("SELECT casetype, caseno, caseyear, stage from tblMain WHERE nextdate=? ORDER BY casetype, caseyear, caseno");
        qryForQList.addBindValue(dateString);
        qryForQList.exec();

        while (qryForQList.next()){

            QString casetype = qryForQList.value(0).toString();
            QString casenoyear = qryForQList.value(1).toString()+"/"+qryForQList.value(2).toString()+", ";
            QString stage = qryForQList.value(3).toString();

            QList<QString> row;
            row << casetype << casenoyear;


            if (stage.contains("First")) {
                listCoulmn1.append(row);
            } else if (stage.contains("Hearing")) {
                listCoulmn2.append(row);
            } else if (stage.contains("Judgment")){
                listCoulmn3.append(row);
            } else {
                listCoulmn4.append(row);
            }
        }

        HPDF_Page_SetFontAndSize (page, fontDef, 12);

        recursiveFunc(dateStringLong);

        rFunRCol1=0;rFunRCol2=0;rFunRCol3=0;rFunRCol4=0;

        listCoulmn1.clear();
        listCoulmn2.clear();
        listCoulmn3.clear();
        listCoulmn4.clear();

        //Add New Page
        page=HPDF_AddPage(pdf);
        HPDF_Page_SetSize  (page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);
        drawHeader();
        drawTable();
    }

    mPDFMyDatabase.closeDatabase();

}



void PDFGenerator::recursiveFunc(QString dateStringLong){

    HPDF_Page_BeginText (page);
    rFunRCol1=mapIterator(listCoulmn1,2,col1X,1,rFunRCol1);
    rFunRCol2=mapIterator(listCoulmn2,2,col2X,2,rFunRCol2);
    rFunRCol3=mapIterator(listCoulmn3,1,col3X,3,rFunRCol3);
    rFunRCol4=mapIterator(listCoulmn4,4,col4X,4,rFunRCol4);
    HPDF_Page_EndText (page);


    while(rFunRCol1<listCoulmn1.size() || rFunRCol2<listCoulmn2.size() || rFunRCol3<listCoulmn3.size() || rFunRCol4<listCoulmn4.size())
    {
        //first add new page
        page=HPDF_AddPage(pdf);
        HPDF_Page_SetSize  (page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);
        drawHeader();
        drawTable();

        QByteArray qBytedateString = dateStringLong.toLatin1();
        const char *date_title = qBytedateString.data();
        int tw = HPDF_Page_TextWidth (page, date_title);  //text width of "date_title"

        HPDF_Page_SetFontAndSize (page, fontBold, 15);
        HPDF_Page_BeginText (page);
        HPDF_Page_TextOut (page, (pageWidth - tw) / 2, pageHeight - 90, date_title);
        HPDF_Page_EndText (page);
        HPDF_Page_SetFontAndSize (page, fontDef, 12);

        if (rFunRCol1<listCoulmn1.size()) recursiveFunc(dateStringLong);
        if (rFunRCol2<listCoulmn2.size()) recursiveFunc(dateStringLong);
        if (rFunRCol3<listCoulmn3.size()) recursiveFunc(dateStringLong);
        if (rFunRCol4<listCoulmn4.size()) recursiveFunc(dateStringLong);
    }
}



int PDFGenerator::nextline(int cLine){
    return cLine-20;
}

int PDFGenerator::mapIterator(QList<QList<QString> > &myList, int rowBrackOn, int &colX, int printColumnNo, int startKey)
{
    int returnValue=0;

    if (startKey >= 0 && startKey < myList.size()) {

        QString caseTypeToCheck = myList[startKey][0];

        double ic=colX;
        double c1Line= cLine;
        int count=0;
        int cellBreakCount = 0;

        for (int i = startKey; i < myList.size(); ++i) {
            const QList<QString>& row = myList.at(i);

            if(count==0){
                QByteArray qByteCtypeString = caseTypeToCheck.toLatin1();
                const char *cTypeString = qByteCtypeString.data();
                HPDF_Page_TextOut (page, ic, c1Line, cTypeString);
                ic=colX; c1Line=nextline(c1Line);
                caseTypeToCheck=row.at(0);

                cellBreakCount=0;
            }

            if(caseTypeToCheck!=row.at(0)){
                if(ic!=colX) { ic=colX; c1Line=nextline(c1Line);}
                QByteArray qByteCtypeString = row.at(0).toLatin1();
                const char *cTypeString = qByteCtypeString.data();
                HPDF_Page_TextOut (page, ic, c1Line, cTypeString);
                ic=colX; c1Line=nextline(c1Line);
                caseTypeToCheck=row.at(0);

                cellBreakCount=0;
            }

            count++;

            QByteArray qBytedataString = row.at(1).toLatin1();
            const char *dataString = qBytedataString.data();
            float textWidth = HPDF_Page_TextWidth(page, dataString);
            float icnew = ic + cellWidth - textWidth;
            HPDF_Page_TextOut (page, icnew, c1Line, dataString);
            ic= ic+cellWidth;

            cellBreakCount++;

            if(cellBreakCount>=rowBrackOn){
                c1Line=nextline(c1Line);
                ic=colX;
                cellBreakCount=0;
            }

            if (i+startKey == myList.size() - 1) {
                return myList.size();
            }

            if(c1Line<60){
                HPDF_Page_TextOut (page, 650, 50, "Continued on the next page ..");
                returnValue=i+1;
                return returnValue;
                break;
            }
        }
    }

    return myList.size();
}



void PDFGenerator::drawHeader(){

    const char *page_title = "Memorandum Book";
    const char *page_title_ref = "Form No XI Chapter XXX of Criminal Manual";
    const char *page_title_tookseat = "Took seat at ........................................................................................ Rose at .............................................................................................................";

    int tw;

    //"Memorandum Book";
    HPDF_Page_SetFontAndSize (page, fontBold, 14);
    tw = HPDF_Page_TextWidth (page, page_title);
    HPDF_Page_BeginText (page);
    HPDF_Page_TextOut (page, (pageWidth - tw) / 2, pageHeight - 50, page_title);

    //"Form No XI Chapter XXX of Criminal Manual";
    HPDF_Page_SetFontAndSize (page, fontItalic, 12);
    tw = HPDF_Page_TextWidth (page, page_title_ref);
    HPDF_Page_TextOut (page, (pageWidth - tw) / 2, pageHeight - 65, page_title_ref);

    //"Took seat at";
    HPDF_Page_SetFontAndSize (page, fontDef, 12);
    tw = HPDF_Page_TextWidth (page, page_title_tookseat);
    HPDF_Page_TextOut (page, (pageWidth - tw) / 2, pageHeight - 130, page_title_tookseat);
    HPDF_Page_EndText (page);
}

void PDFGenerator::drawTable() {

    HPDF_Page_SetLineWidth(page, 0.8); // Set line width

    const int rows = 3;
    const int cols = 4;
    const HPDF_REAL cellWidths[cols] = {165, 167, 90, 325}; // Adjust column widths as needed
    const HPDF_REAL cellHeights[rows] = {38, 15, 350}; // Different row heights
    const HPDF_REAL startX = 50;
    const HPDF_REAL startY = pageHeight - 150;

    // Draw table borders
    for (int i = 0; i < cols + 1; ++i) {
        HPDF_Page_MoveTo(page, startX + std::accumulate(cellWidths, cellWidths + i, 0.0), startY);
        HPDF_Page_LineTo(page, startX + std::accumulate(cellWidths, cellWidths + i, 0.0), startY - std::accumulate(cellHeights, cellHeights + rows, 0.0));
        HPDF_Page_Stroke(page);
    }

    HPDF_REAL currentY = startY;

    for (int i = 0; i < rows + 1; ++i) {
        HPDF_Page_MoveTo(page, startX, currentY);
        HPDF_Page_LineTo(page, startX + std::accumulate(cellWidths, cellWidths + cols, 0.0), currentY);
        HPDF_Page_Stroke(page);

        if (i < rows) {
            currentY -= cellHeights[i];
            HPDF_Page_MoveTo(page, startX, currentY);
            HPDF_Page_LineTo(page, startX + std::accumulate(cellWidths, cellWidths + cols, 0.0), currentY);
            HPDF_Page_Stroke(page);
        }
    }

    HPDF_Page_SetFontAndSize (page, fontBold, 12);
    HPDF_Page_BeginText (page);
    HPDF_Page_TextOut (page, pageWidth - 778, pageHeight - 165, "Cases fixed for first hearing");
    HPDF_Page_TextOut (page, pageWidth - 605, pageHeight - 165, "Cases fixed for adjourned");
    HPDF_Page_TextOut (page, pageWidth - 450, pageHeight - 165, "Cases fixed for");
    HPDF_Page_TextOut (page, pageWidth - 365, pageHeight - 165, "Any other proceeding coming on this date and not entered");
    HPDF_Page_TextOut (page, pageWidth - 560, pageHeight - 180, "hearing");
    HPDF_Page_TextOut (page, pageWidth - 435, pageHeight - 180, "judgment");
    HPDF_Page_TextOut (page, pageWidth - 260, pageHeight - 180, "in Columns 1 to 3");
    HPDF_Page_EndText (page);

    HPDF_Page_SetFontAndSize (page, fontDef, 12);
    HPDF_Page_BeginText (page);
    HPDF_Page_TextOut (page, pageWidth - 710, pageHeight - 200, "1");
    HPDF_Page_TextOut (page, pageWidth - 545, pageHeight - 200, "2");
    HPDF_Page_TextOut (page, pageWidth - 412, pageHeight - 200, "3");
    HPDF_Page_TextOut (page, pageWidth - 210, pageHeight - 200, "4");
    HPDF_Page_EndText (page);
}
