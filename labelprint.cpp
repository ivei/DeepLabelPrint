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
    LabelTemplate::Element elm1 = buildQRCodeElement(QRect(0,0,13,13), 1, 1);

    tmpl.elements.push_back(elm1);

    LabelTemplate::Element elm2 = buildTextElement(QRect(0,15,15,4), "TJJY", 5);
    tmpl.elements.push_back(elm2);

    LabelTemplate::Element elm3 = buildLineElement(QLine(15, 0, 15, 20), 1);
    tmpl.elements.push_back(elm3);

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
