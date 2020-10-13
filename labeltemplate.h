#ifndef LABELTEMPLATE_H
#define LABELTEMPLATE_H
#include <QSize>
#include <QString>
#include <QRect>
#include <QVariant>
#include <QVector>
#include <QPainter>
#include <QPrinter>
/**
 * @brief 定义标签模板
 * @note  这个类必须定义为POC! 不然需要定义拷贝构造函数等.
 */
class LabelTemplate
{
public:
    //! 单元的类别
    enum ElementType{
        EMPTY = 0,
        QRCODE,         //! 二维码
        QRCODE_TEXT,    //! 二维码文本, 不需要
        ICON,           //! 图标
        TEXT,           //! 文本. 支持固定文本, 也支持导入文本
        LINE,           //! 线段
        RECT,
    };
    enum Alignment{
        AlignLeft =Qt::AlignLeft,
        AlignRight=Qt::AlignRight,
        AlignCenter=Qt::AlignCenter,
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignMiddle = Qt::AlignHCenter
    };


    struct Element
    {
        int     type;       //! 单元的类别.
        //QRect   rect;       //! 单元的位置.
        int     left;
        int     top;
        int     right;
        int     bottom;
        int     margin;     //! 单元绘图区域的四边留白
        int     hAlign;     //! 水平方向的对其模式
        int     vAlign;     //! 垂直方向的对齐模式
        int     lineWidth;  //! 线段的线宽
        int     fontSize;   //! 文本的字号
        QString fontName;   //! 字体名称
        int     link;       //! 当导入时的列号
        QVariant value;
    };

    QString name;   //! 标签名称, 字符串, 没有实际用处
    QSize   size;   //! 标签大小, 单位mm
    int     unit;   //! 单位, 缺省是mm(0)
    QVector<Element> elements;


};

//! 转换函数: 毫米转换为pixel
inline int mm2Pixel(int mm, int dpi){
    return (mm*dpi/25.4);
}

//! 构造一个er'wei二维码的element
LabelTemplate::Element buildQRCodeElement(const QRect& rect, int link, int margin=0 );
//! 构造一个静态文本块
LabelTemplate::Element buildTextElement(const QRect& rect, const QString& text, int fontSize
        , const QString& fontName = "Microsoft Yahei"
        , int margin = 0, int hAlign = LabelTemplate::Alignment::AlignCenter
        , int vAlign = LabelTemplate::Alignment::AlignMiddle);
//! 构造一个引入的文本块
LabelTemplate::Element buildStringElement(const QRect& rect, int link, int fontSize
        , const QString& fontName = "Microsoft Yahei"
        , int margin = 0, int hAlign = LabelTemplate::Alignment::AlignCenter
        , int vAlign = LabelTemplate::Alignment::AlignMiddle);

//! 构造一个图标块
LabelTemplate::Element buildIconElement(const QRect& rect, const QPixmap& pixmap
        , int margin = 0
        , int hAlign = LabelTemplate::Alignment::AlignCenter
        , int vAlign = LabelTemplate::Alignment::AlignMiddle
        );

//! 构造一个线段块
LabelTemplate::Element buildLineElement(const QLine& line, int width);

//! 构造一个矩形块
LabelTemplate::Element buildRectElement(const QRect& rect, int linewidth=-1);



//! 设置模板的ICON
void setElement(LabelTemplate::Element& elem, const QPixmap& pixmap);
//! 设置模板的文本
void setElement(LabelTemplate::Element& elem, const QString& text);
//! 将模板保存到json文件中
void saveTemplate(const LabelTemplate& tmpl, const QString& path);
//! 从json文件中加载模板
void loadTemplate(LabelTemplate& tmpl, const QString& path);



//! 绘制二维码
void drawQRCode(const QRect &rect, QPainter* painter, const QString& str);

//! 画整个标签
void drawLabel(const QRect& drawRect, int dpi, QPainter* painter, const LabelTemplate& label);

#endif // LABELTEMPLATE_H
