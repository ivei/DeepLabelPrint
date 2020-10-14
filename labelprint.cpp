#include "labelprint.h"
#include "ui_labelprint.h"
#include <QCoreApplication>
#include <QPrintPreviewDialog>
#include <QPrintDialog>
#include <QFileDialog>
#include "dbgutility.h"

#pragma execution_character_set("utf-8")

const int PageWidth = 28;
const int PageHeight = 20;
const int LabelWidth = 20;
const int LabelHeight = 25;
const int LineCount = 1;

void dbgPrintInfo(QPrinter* printer);

/**
 * @brief 构造一个有一个二维码和一个二维码文本的标签, 带一条分割线.
 * @details
 *
 */
LabelTemplate test_make_template_01()
{
    //QString filename = QString("D:\\workspace\\DeepLabelPrint\\icon.ico");
    //QPixmap pixmap;
    //pixmap.load(filename);

    LabelTemplate   tmpl;
    tmpl.name   = "template_001";
    tmpl.size   = QSize(LabelWidth,LabelHeight);

    // 二维码大小为18x18
    LabelTemplate::Element elm1 = buildQRCodeElement(QRect(2,2,16,16), 1, 0);

    tmpl.elements.push_back(elm1);

    LabelTemplate::Element elm2 = buildTextElement(QRect(0,20,20,5), "TJJY", 5);
    tmpl.elements.push_back(elm2);

    //LabelTemplate::Element elm3 = buildLineElement(QLine(15, 0, 15, 20), 1);
    //tmpl.elements.push_back(elm3);

    return tmpl;
}


LabelPrint::LabelPrint(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LabelPrint)
{
    ui->setupUi(this);
}

LabelPrint::~LabelPrint()
{
    delete ui;
}

void LabelPrint::printRequested(QPrinter* printer)
{
    TRACE();
    const int dpi = 203;

    //LabelTemplate tmpl01 = test_make_template_01();
    LabelTemplate tmpl01 = this->curTmpl;
    setElement(tmpl01.elements[0], "JY20200111");
    LabelTemplate tmpl02 = this->curTmpl;
    setElement(tmpl02.elements[0], "JY20200112");
    setElement(tmpl02.elements[1], "JY20200112");

    //printer->setOrientation(QPrinter::Portrait);
    printer->setPrinterName("TSC TTP-244 Pro");
    printer->setOrientation(QPrinter::Landscape);
    printer->setPageSize(QPageSize(QSizeF(PageWidth,PageHeight), QPageSize::Unit::Millimeter,"", QPageSize::ExactMatch));
    printer->setFullPage(true);
    printer->setResolution(dpi);

    //drawLabel(QRect(0,0,20,25), 203, printer, tmpl01);
    QPainter painter;
    painter.begin(printer);

    for( int index=0; index<LineCount; index++)
    {
        int offset_x = 1 + index * LabelWidth;
        int offset_y = 0;
        TRACE() << "print at " << offset_x << offset_y;
        drawLabel(QRect(offset_x, offset_y, LabelWidth, LabelHeight), dpi, &painter, tmpl01);
    }

    painter.end();
}


void LabelPrint::on_btsTest1_clicked()
{


    this->curTmpl = test_make_template_01();

    QString filename = QFileDialog::getSaveFileName(this, tr("选择文件"), QCoreApplication::applicationDirPath(), tr("json files (*.json)"));

    //QString filename = QString("%1/%2.json").arg(QCoreApplication::applicationDirPath()).arg(tmpl.name);



    saveTemplate(this->curTmpl, filename);
}





void LabelPrint::on_loadTmplBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("选择文件"), QCoreApplication::applicationDirPath(), tr("json files (*.json)"));

    loadTemplate(this->curTmpl, filename);
}

void LabelPrint::on_prevPrintBtn_clicked()
{
    QPrinter printer;
    QPrintPreviewDialog dlg;
    connect(&dlg, &QPrintPreviewDialog::paintRequested, this, &LabelPrint::printRequested);
    dlg.exec();


}

void LabelPrint::on_printBtn_clicked()
{
    QPrinter printer;
    bool ret;
    printer.setPrinterName("TSC TTP-244 Pro");
    TRACE() << "valid printer? " << printer.isValid();
    TRACE() << "Old printer param: ";
    dbgPrintInfo(&printer);

    printer.setOrientation(QPrinter::Landscape);
    TRACE() << "set pageSize" << printer.setPageSize(QPageSize(QSizeF(81,20), QPageSize::Unit::Millimeter, "", QPageSize::ExactMatch));


    TRACE() << "New prminter param: ";
    dbgPrintInfo(&printer);
#if 1
    QPainter painter;
    painter.begin(&printer);

    for(int i=0; i<10; i++)
    {
        painter.drawRect(i*5, i*5, 10, 10);
    }

    painter.end();
#endif
}


void dbgPrintInfo(QPrinter* printer)
{
    TRACE() << "Printer name: " << printer->printerName();
    auto layout = printer->pageLayout();
    TRACE() << "orientation " << ((layout.orientation()==QPageLayout::Orientation::Portrait) ? "Portrait" : "Landscape");
    TRACE() << "pageSize: " << layout.fullRect(QPageLayout::Unit::Millimeter) << layout.fullRectPixels(203);

    TRACE() << "margin: " << layout.margins(QPageLayout::Unit::Millimeter);
}
