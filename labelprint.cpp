#include "labelprint.h"
#include "ui_labelprint.h"
#include <QCoreApplication>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include "dbgutility.h"

#pragma execution_character_set("utf-8")

const int PageWidth = 100;
const int PageHeight = 20;
const int LabelWidth = 25;
const int LabelHeight = 20;

LabelTemplate test_make_template_01()
{
    QString filename = QString("D:\\workspace\\DeepLabelPrint\\icon.ico");
    QPixmap pixmap;
    pixmap.load(filename);

    LabelTemplate   tmpl;
    tmpl.name   = "tp1";
    tmpl.size   = QSize(LabelWidth,LabelHeight);

    LabelTemplate::Element elm1 = buildQRCodeElement(QRect(0,0,16,16), 1, 1);
    //setElement(elm1, "JY20201013001B");
    tmpl.elements.push_back(elm1);

    LabelTemplate::Element elm2 = buildTextElement(QRect(0,16,15,4), "天津金域", 5);
    tmpl.elements.push_back(elm2);

    LabelTemplate::Element elm3 = buildLineElement(QLine(16, 0, 16, 20), 1);
    tmpl.elements.push_back(elm3);

    LabelTemplate::Element elm4 = buildIconElement(QRect(16, 0, 9, 9), pixmap, 0);
    tmpl.elements.push_back(elm4);

    LabelTemplate::Element elm5 = buildRectElement(QRect(18, 10, 6,6), 1);
    tmpl.elements.push_back(elm5);

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

    printer->setOrientation(QPrinter::Portrait);
    printer->setPageSize(QPageSize(QSizeF(PageWidth,PageHeight), QPageSize::Unit::Millimeter,"", QPageSize::ExactMatch));
    printer->setFullPage(true);
    printer->setResolution(dpi);

    //drawLabel(QRect(0,0,20,25), 203, printer, tmpl01);
    QPainter painter;
    painter.begin(printer);
    for( int index=0; index<4; index++)
    {
        int offset_x = 0 + index * LabelWidth;
        int offset_y = 0;
        drawLabel(QRect(offset_x, offset_y, LabelWidth, LabelHeight), dpi, &painter, tmpl01);
    }
    //drawLabel(QRect(50,0,25,20), dpi, &painter, tmpl01);
    painter.end();
}


void LabelPrint::on_btsTest1_clicked()
{
    LabelTemplate   tmpl;


    tmpl = test_make_template_01();


    QString filename = QString("%1/%2.json").arg(QCoreApplication::applicationDirPath()).arg(tmpl.name);
    saveTemplate(tmpl, filename);
}

void LabelPrint::on_btnTest2_clicked()
{
    //QString filename = QString("%1/archive.json").arg(QCoreApplication::applicationDirPath());
    QString filename = QFileDialog::getOpenFileName(this, tr("选择文件"), QCoreApplication::applicationDirPath(), tr("json files (*.json)"));

    loadTemplate(this->curTmpl, filename);
}

void LabelPrint::on_testPrintBtn_clicked()
{

#if 0
    // 构造tp01,
    LabelTemplate tmpl01 = test_make_template_01();


    setElement(tmpl01.elements[1], "JY20201012001");
    setElement(tmpl01.elements[0], "JY20201012001");
    QPixmap pixmap(250, 200);
    QPainter painter;
    painter.begin(&pixmap);
    painter.fillRect(pixmap.rect(), Qt::white);
    drawLabel(pixmap.rect(), 203, &painter, tmpl01);
    painter.end();

    ui->dbgLabel->setPixmap(pixmap);
#endif

    QPrinter printer;
    printer.setOrientation(QPrinter::Portrait);
    printer.setPageSize(QPageSize(QSizeF(PageWidth,PageHeight), QPageSize::Unit::Millimeter,"", QPageSize::ExactMatch));
    printer.setFullPage(true);

    QPrintPreviewDialog dlg;
    connect(&dlg, &QPrintPreviewDialog::paintRequested, this, &LabelPrint::printRequested);
    dlg.exec();


}
