#include "labeltemplate.h"
#include <QBuffer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QPixmap>
#include <QVariant>
#include "dbgutility.h"
#include "qrencode.h"

#pragma execution_character_set("utf-8")

void setElement(LabelTemplate::Element &elem, const QPixmap &pixmap)
{
    if( elem.type == LabelTemplate::ElementType::ICON){
        elem.value = pixmap;
    }
    else {
        TRACE() << "Element is not ICON type!";
    }
}

void setElement(LabelTemplate::Element &elem, const QString &text)
{
    if( elem.type == LabelTemplate::ElementType::ICON){
        TRACE() << "Element is not text";
        return;
    }
    elem.value = text;
}

/**
 * @brief 将标签标签模板保存到json文件中
 * @param tmpl  模板数据
 * @param path  文件名
 */
void saveTemplate(const LabelTemplate &tmpl, const QString &path)
{
    QJsonArray elements;
    for(auto elem: tmpl.elements)
    {
        auto obj = QJsonObject::fromVariantMap({
                {"type", elem.type},
                {"left", elem.left},
                {"top", elem.top},
                {"right", elem.right},
                {"bottom", elem.bottom},
                {"margin", elem.margin},
                {"hAlign", elem.hAlign},
                {"vAlign", elem.vAlign},
                {"lineWidth", elem.lineWidth},
                {"fontSize", elem.fontSize},
                {"fontName", elem.fontName},
                {"link", elem.link}
            });
        switch (elem.type)
        {
        case LabelTemplate::QRCODE:
        case LabelTemplate::QRCODE_TEXT:
            break;
        case LabelTemplate::TEXT:
            //obj.insert("value", elem.value.toString());
            obj.insert("value", QString::fromLocal8Bit(elem.value.toString().toUtf8()));
            break;
        case LabelTemplate::ICON:
            if(elem.value.canConvert<QPixmap>()){
                QPixmap pixmap = elem.value.value<QPixmap>();
                QByteArray ba;
                QBuffer buffer(&ba);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer, "JPG");
                obj.insert("value", ba.toBase64().data());
            }
            else {
                TRACE() << "Invalid icon format!";
            }
            break;
        default:
            break;

        }
        elements.append(obj);
    }

    auto json = QJsonObject::fromVariantMap({
        {"name", tmpl.name}
        ,{"width", tmpl.size.width()}
        ,{"height", tmpl.size.height()}
        ,{"unit", tmpl.unit}
        ,{"elements", elements}
        });


    QFile file(path);
    if( !file.open(QFile::WriteOnly|QFile::Text)){
        TRACE() << "open data file failed!";
        return;
    }
    QTextStream buf(&file);
    QJsonDocument doc(json);
    buf << doc.toJson();
    file.close();
}

/**
 * @brief 从json文件中加载模板
 * @param tmpl
 * @param path
 */
void loadTemplate(LabelTemplate &tmpl, const QString &path)
{
    QFile file(path);
    if( !file.open(QIODevice::ReadOnly|QIODevice::Text)){
        TRACE() << "Open file" << path << "failed!";
        return;
    }

    auto data = file.readAll();
    auto obj = QJsonDocument::fromJson(data).object();

    tmpl.name = obj.value("name").toString();
    tmpl.size = QSize(obj.value("width").toInt(), obj.value("height").toInt());
    tmpl.unit = obj.value("unit").toInt();
    for(auto e: obj.value("elements").toArray())
    {
        LabelTemplate::Element elem;
        auto ee = e.toObject();
        elem.type   = ee.value("type").toInt();
        //elem.rect   = QRect(ee.value("top").toInt(), ee.value("left").toInt(), ee.value("width").toInt(), ee.value("height").toInt());
        elem.left   = ee.value("left").toInt();
        elem.top    = ee.value("top").toInt();
        elem.right  = ee.value("right").toInt();
        elem.bottom = ee.value("bottom").toInt();

        elem.margin = ee.value("margin").toInt();
        elem.hAlign = ee.value("hAlign").toInt();
        elem.vAlign = ee.value("vAlign").toInt();
        elem.lineWidth = ee.value("lineWidth").toInt();
        elem.fontSize = ee.value("fontSize").toInt();
        elem.fontName = ee.value("fontName").toString();
        elem.value    = ee.value("value").toVariant();
        tmpl.elements.push_back(elem);
    }
}

/**
 * @brief 画二维码
 * @param rect
 * @param painter
 * @param code
 */
void drawQRCode(const QRect &rect, QPainter *painter, const QString &str)
{
    auto code = QRcode_encodeString(str.toUtf8().data(), 1, QR_ECLEVEL_M, QR_MODE_8, 1);
    if(!code){
        TRACE() << "Invalid QRCode!";
        return;
    }
    TRACE() << "Create QR Code with" << code->width;
    // 要计算放大倍数.
    int ratio = std::min(rect.width(), rect.height()) / code->width;
    int drawSize = code->width * ratio;
    QPoint startPoint = QPoint(rect.center().x()-drawSize/2, rect.center().y()-drawSize/2);

    // 构造二维码的矩形列表. 后面一次性绘制.
    QVector<QRect> rects;
    auto data = code->data;
    for(int y=0; y<code->width; ++y)
    {
        for(int x=0; x<code->width; ++x)
        {
            if(*data++ & 0x01){
                rects.append(QRect(QPoint(x,y)*ratio + startPoint, QSize(ratio, ratio)));
            }
        }
    }
    QRcode_free(code);

    painter->setBrush(QBrush(Qt::black));
    painter->drawRects(rects);

}



/**
 * @brief 画标签
 * @param drawRect: 要绘制的区域. 单位: mm
 * @param dpi:      要绘制的设备的dpi值.
 * @param painter
 * @param label
 * @note
 *      将label会知道设备的drawRect区域中.
 *      label中的单位固定为mm, 设备drawRect的单位是pixel. 因此, 需要将label中的mm转换为pixel.
 *      同时, label中的坐标是相对于label的局部坐标系, 需要转换为设备的全局坐标系.
 */
void drawLabel(const QRect &drawRect, int dpi, QPainter *painter, const LabelTemplate &label)
{
    TRACE() << "drawRect " << drawRect;
    // 计算放大倍数, 每毫米的像素数. 1inch=25.4mm
    double scale = dpi / 25.4;
    // 标签的绘制区域.
    QRect pixRect = QRect(drawRect.topLeft()*scale, drawRect.size()*scale);
    painter->drawRect(pixRect.adjusted(2,2,-2,-2));
    // 分别绘制每个element
    for(auto& elem: label.elements)
    {
        int margin = elem.margin * scale;
        QPoint p1 = QPoint(elem.left, elem.top) * scale + pixRect.topLeft();
        QPoint p2 = QPoint(elem.right, elem.bottom) * scale + pixRect.topLeft();
        QRect elemRect = QRect(p1, p2).adjusted(margin, margin, -margin, -margin);

        switch(elem.type){
        case LabelTemplate::ElementType::QRCODE:
            drawQRCode(elemRect, painter, elem.value.value<QString>());
            break;
        case LabelTemplate::ElementType::TEXT:
            painter->setFont(QFont(elem.fontName, elem.fontSize));
            painter->setPen(Qt::black);
            painter->drawText(elemRect, elem.hAlign|elem.vAlign, elem.value.value<QString>());
            break;
        case LabelTemplate::ElementType::ICON:
            if( elem.value.canConvert<QPixmap>()){
                painter->drawPixmap(elemRect, elem.value.value<QPixmap>().scaled(elemRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            else {
                TRACE() << "Invalid icon format!";
            }
            break;
        case LabelTemplate::ElementType::LINE:
            painter->setPen(QPen(Qt::black, elem.lineWidth));
            painter->drawLine(p1, p2);
            break;
        case LabelTemplate::ElementType::RECT:
            if( elem.lineWidth>0){
                painter->setBrush(QBrush(Qt::NoBrush));
                painter->setPen(QPen(Qt::black, elem.lineWidth));
            }
            else{
                painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
                painter->setPen(QPen(Qt::black));
            }
            painter->drawRect(elemRect);
            break;
        default:
            break;
        }
    }// -- end of for
}





LabelTemplate::Element buildQRCodeElement(const QRect &rect, int link, int margin)
{
    LabelTemplate::Element elem ;
    elem.type   = LabelTemplate::ElementType::QRCODE;
    elem.left   = rect.left();
    elem.right  = rect.right();
    elem.top    = rect.top();
    elem.bottom = rect.bottom();
    elem.margin = margin;
    elem.hAlign = LabelTemplate::Alignment::AlignCenter;
    elem.vAlign = LabelTemplate::Alignment::AlignMiddle;
    elem.lineWidth = 0;
    elem.fontSize = 0;
    elem.fontName = "";
    elem.link     = link;
    elem.value    = QVariant();

    return elem;
}

LabelTemplate::Element buildTextElement(const QRect &rect, const QString &text, int fontSize
                                        , const QString &fontName, int margin, int hAlign, int vAlign)
{
    LabelTemplate::Element elem ;
    elem.type   = LabelTemplate::ElementType::TEXT;
    elem.left   = rect.left();
    elem.right  = rect.right();
    elem.top    = rect.top();
    elem.bottom = rect.bottom();
    elem.margin = margin;
    elem.hAlign = hAlign;
    elem.vAlign = vAlign;
    elem.lineWidth = 0;
    elem.fontSize = fontSize;
    elem.fontName = fontName;
    elem.link     = -1;
    elem.value    = text;

    return elem;
}

LabelTemplate::Element buildStringElement(const QRect &rect, int link, int fontSize
                                        , const QString &fontName, int margin, int hAlign, int vAlign)
{
    LabelTemplate::Element elem ;
    elem.type   = LabelTemplate::ElementType::TEXT;
    elem.left   = rect.left();
    elem.right  = rect.right();
    elem.top    = rect.top();
    elem.bottom = rect.bottom();
    elem.margin = margin;
    elem.hAlign = hAlign;
    elem.vAlign = vAlign;
    elem.lineWidth = 0;
    elem.fontSize = fontSize;
    elem.fontName = fontName;
    elem.link     = link;
    elem.value    = QVariant();

    return elem;
}

LabelTemplate::Element buildIconElement(const QRect &rect, const QPixmap &pixmap, int margin, int hAlign, int vAlign)
{
    LabelTemplate::Element elem ;
    elem.type   = LabelTemplate::ElementType::ICON;
    elem.left   = rect.left();
    elem.right  = rect.right();
    elem.top    = rect.top();
    elem.bottom = rect.bottom();
    elem.margin = margin;
    elem.hAlign = hAlign;
    elem.vAlign = vAlign;
    elem.fontSize = 0;
    elem.fontName = "";
    elem.lineWidth = 0;
    elem.link     = -1;
    elem.value    = pixmap;

    return elem;
}

LabelTemplate::Element buildLineElement(const QLine &line, int width)
{
    LabelTemplate::Element elem ;
    elem.type   = LabelTemplate::ElementType::LINE;
    elem.left   = line.x1();
    elem.top    = line.y1();
    elem.right  = line.x2();
    elem.bottom = line.y2();
    elem.margin = 0;
    elem.hAlign = LabelTemplate::Alignment::AlignLeft;
    elem.vAlign = LabelTemplate::Alignment::AlignTop;
    elem.lineWidth = width;
    elem.fontSize = 0;
    elem.fontName = "";
    elem.link     = -1;
    elem.value    = QVariant();

    return elem;
}

LabelTemplate::Element buildRectElement(const QRect &rect, int linewidth)
{
    LabelTemplate::Element elem ;
    elem.type   = LabelTemplate::ElementType::RECT;
    elem.left   = rect.left();
    elem.right  = rect.right();
    elem.top    = rect.top();
    elem.bottom = rect.bottom();
    elem.margin = 0;
    elem.hAlign = LabelTemplate::Alignment::AlignLeft;
    elem.vAlign = LabelTemplate::Alignment::AlignTop;
    elem.lineWidth = linewidth;
    elem.fontSize = 0;
    elem.fontName = "";
    elem.link     = -1;
    elem.value    = QVariant();

    return elem;
}
