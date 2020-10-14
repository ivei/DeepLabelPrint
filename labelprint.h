#ifndef LABELPRINT_H
#define LABELPRINT_H

#include <QCoreApplication>
#include <QDialog>
#include <QSettings>
#include <QString>
#include "labeltemplate.h"
QT_BEGIN_NAMESPACE
namespace Ui { class LabelPrint; }
QT_END_NAMESPACE

//! 打印配置参数
class PConfig
{
public:
    static PConfig& getInstance(){
        static PConfig _instance;
        return _instance;
    }

    int getPageWidth() {return this->pageWidth; }
    void setPageWidth(int width) { this->pageWidth = width;  this->settings.setValue("pageWidth", width); }

private:
    int     pageWidth;  // 纸宽度
    int     pageHeight; // 纸高度
    int     pageLines;  // 每张纸打印的标签行数
    int     lineLabels; // 每行打印的标签数
    int     labelRepeats;   // 标签重复个数
    QSettings settings;

    PConfig() : pageWidth(28), pageHeight(20), pageLines(1), lineLabels(1), labelRepeats(1)
        , settings(QString("%1/config.ini").arg(QCoreApplication::applicationDirPath()), QSettings::IniFormat)
    {
        if( settings.contains("pageWidth")){
            this->pageWidth = settings.value("pageWidth").toInt();
        }
        else {
            settings.setValue("pageWidth", this->pageWidth);
        }

        if( settings.contains("pageHeight")){
            this->pageHeight = settings.value("pageHeight").toInt();
        }
        else {
            settings.setValue("pageHeight", this->pageHeight);
        }

        if( settings.contains("pageLines")){
            this->pageLines = settings.value("pageLines").toInt();
        }
        else {
            settings.setValue("pageLines", this->pageLines);
        }

        if( settings.contains("lineLabels")){
            this->lineLabels = settings.value("lineLabels").toInt();
        }
        else {
            settings.setValue("lineLabels", this->lineLabels);
        }

        if( settings.contains("labelRepeats")){
            this->labelRepeats = settings.value("labelRepeats").toInt();
        }
        else {
            settings.setValue("labelRepeats", this->labelRepeats);
        }

    };
};

class LabelPrint : public QDialog
{
    Q_OBJECT

public:
    LabelPrint(QWidget *parent = nullptr);
    ~LabelPrint();

private slots:
    void printRequested(QPrinter *printer);

    void on_btsTest1_clicked();

    void on_loadTmplBtn_clicked();

    void on_prevPrintBtn_clicked();

    void on_printBtn_clicked();



private:
    Ui::LabelPrint *ui;
    LabelTemplate curTmpl;  //! 当前使用的模板
};
#endif // LABELPRINT_H
