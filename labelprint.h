#ifndef LABELPRINT_H
#define LABELPRINT_H

#include <QDialog>
#include "labeltemplate.h"
QT_BEGIN_NAMESPACE
namespace Ui { class LabelPrint; }
QT_END_NAMESPACE



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

private:
    Ui::LabelPrint *ui;
    LabelTemplate curTmpl;  //! 当前使用的模板
};
#endif // LABELPRINT_H
