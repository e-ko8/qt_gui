/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2013-2020 Mattia Basaglia
 * \copyright Copyright (C) 2014 Calle Laakkonen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "qtesplamp.hpp"
#include "ui_qtesplamp.h"

#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDesktopWidget>
#include <QMimeData>
#include <QPushButton>
#include <QScreen>
#include <QApplication>
#include <QPushButton>
#include <QScreen>

#include "color_dialog.hpp"
#include "color_utils.hpp"

using namespace color_widgets;

class QEspLamp::Private
{
public:
    Ui_QEspLamp ui;
    ButtonMode button_mode;
    bool pick_from_screen;
    QColor color;

    Private() : pick_from_screen(false)
    {}

};

QEspLamp::QEspLamp(QWidget *parent) :
    QWidget(parent), p(new Private), tcpSocket(new QTcpSocket(this))
{
    p->ui.setupUi(this);

    setAcceptDrops(true);
#if 0
    // Add "pick color" button
    QPushButton *pickButton = p->ui.buttonBox->addButton(tr("Pick"), QDialogButtonBox::ActionRole);
    pickButton->setIcon(QIcon::fromTheme(QStringLiteral("color-picker")));
#endif
    setButtonMode(OkApplyCancel);

    connect(p->ui.wheel, &ColorWheel::colorSpaceChanged, this, &QEspLamp::colorSpaceChanged);
    connect(p->ui.wheel, &ColorWheel::selectorShapeChanged, this, &QEspLamp::wheelShapeChanged);
    connect(p->ui.wheel, &ColorWheel::rotatingSelectorChanged, this, &QEspLamp::wheelRotatingChanged);
}

QEspLamp::~QEspLamp()
{
    delete p;
}

QSize QEspLamp::sizeHint() const
{
    return QSize(400,0);
}

QColor QEspLamp::color() const
{
    QColor col = p->color;
    return col;
}

void QEspLamp::setColor(const QColor &c)
{
    p->ui.preview->setComparisonColor(c);
    p->ui.edit_hex->setModified(false);
    setColorInternal(c);
}

void QEspLamp::showColor(const QColor &c)
{
    setColor(c);
    show();
}

void QEspLamp::setPreviewDisplayMode(ColorPreview::DisplayMode mode)
{
    p->ui.preview->setDisplayMode(mode);
}

ColorPreview::DisplayMode QEspLamp::previewDisplayMode() const
{
    return p->ui.preview->displayMode();
}

void QEspLamp::setButtonMode(ButtonMode mode)
{
#if 0
    p->button_mode = mode;
    QDialogButtonBox::StandardButtons btns;
    switch(mode) {
        case OkCancel: btns = QDialogButtonBox::Ok | QDialogButtonBox::Cancel; break;
        case OkApplyCancel: btns = QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply | QDialogButtonBox::Reset; break;
        case Close: btns = QDialogButtonBox::Close;
    }

    p->ui.buttonBox->setStandardButtons(btns);
#endif
}

QEspLamp::ButtonMode QEspLamp::buttonMode() const
{
    return p->button_mode;
}

void QEspLamp::setColorInternal(const QColor &col)
{
    /**
     * \note Unlike setColor, this is used to update the current color which
     * migth differ from the final selected color
     */
    p->ui.wheel->setColor(col);

    p->color = col;

    bool blocked = signalsBlocked();
    blockSignals(true);
    Q_FOREACH(QWidget* w, findChildren<QWidget*>())
        w->blockSignals(true);


    p->ui.slide_red->setValue(col.red());
    p->ui.spin_red->setValue(p->ui.slide_red->value());
    p->ui.slide_red->setFirstColor(QColor(0,col.green(),col.blue()));
    p->ui.slide_red->setLastColor(QColor(255,col.green(),col.blue()));

    p->ui.slide_green->setValue(col.green());
    p->ui.spin_green->setValue(p->ui.slide_green->value());
    p->ui.slide_green->setFirstColor(QColor(col.red(),0,col.blue()));
    p->ui.slide_green->setLastColor(QColor(col.red(),255,col.blue()));

    p->ui.slide_blue->setValue(col.blue());
    p->ui.spin_blue->setValue(p->ui.slide_blue->value());
    p->ui.slide_blue->setFirstColor(QColor(col.red(),col.green(),0));
    p->ui.slide_blue->setLastColor(QColor(col.red(),col.green(),255));

    p->ui.slide_hue->setValue(qRound(p->ui.wheel->hue()*360.0));
    p->ui.slide_hue->setColorSaturation(p->ui.wheel->saturation());
    p->ui.slide_hue->setColorValue(p->ui.wheel->value());
    p->ui.spin_hue->setValue(p->ui.slide_hue->value());

    p->ui.slide_saturation->setValue(qRound(p->ui.wheel->saturation()*255.0));
    p->ui.spin_saturation->setValue(p->ui.slide_saturation->value());
    p->ui.slide_saturation->setFirstColor(QColor::fromHsvF(p->ui.wheel->hue(),0,p->ui.wheel->value()));
    p->ui.slide_saturation->setLastColor(QColor::fromHsvF(p->ui.wheel->hue(),1,p->ui.wheel->value()));

    p->ui.slide_value->setValue(qRound(p->ui.wheel->value()*255.0));
    p->ui.spin_value->setValue(p->ui.slide_value->value());
    p->ui.slide_value->setFirstColor(QColor::fromHsvF(p->ui.wheel->hue(), p->ui.wheel->saturation(),0));
    p->ui.slide_value->setLastColor(QColor::fromHsvF(p->ui.wheel->hue(), p->ui.wheel->saturation(),1));


    if ( !p->ui.edit_hex->isModified() )
        p->ui.edit_hex->setColor(col);

    p->ui.preview->setColor(col);

    blockSignals(blocked);
    Q_FOREACH(QWidget* w, findChildren<QWidget*>())
        w->blockSignals(false);

    Q_EMIT colorChanged(col);
}

void QEspLamp::set_hsv()
{
    if ( !signalsBlocked() )
    {
        QColor col = QColor::fromHsv(
            p->ui.slide_hue->value(),
            p->ui.slide_saturation->value(),
            p->ui.slide_value->value()
        );
        p->ui.wheel->setColor(col);
        setColorInternal(col);
    }
}

void QEspLamp::set_rgb()
{
    if ( !signalsBlocked() )
    {
        QColor col(
                p->ui.slide_red->value(),
                p->ui.slide_green->value(),
                p->ui.slide_blue->value()
            );
        if (col.saturation() == 0)
            col = QColor::fromHsv(p->ui.slide_hue->value(), 0, col.value());
        p->ui.wheel->setColor(col);
        setColorInternal(col);
    }
}

void QEspLamp::on_edit_hex_colorChanged(const QColor& color)
{
    setColorInternal(color);
}

void QEspLamp::on_edit_hex_colorEditingFinished(const QColor& color)
{
    p->ui.edit_hex->setModified(false);
    setColorInternal(color);
}

void QEspLamp::on_submitClicked()
{
    p->ui.pushButton->setEnabled(false);
    tcpSocket->abort();
    tcpSocket->connectToHost(p->ui.hostLineEdit->text(), p->ui.portSpinBox->value());
    if (tcpSocket->waitForConnected())
    {
        if (tcpSocket->state() == QAbstractSocket::ConnectedState)
        {
            int r, g, b, a;
            p->color.getRgb(&r, &g, &b, &a);

            int mode = p->ui.modeComboBox->itemData(p->ui.modeComboBox->currentIndex()) == "SOLID_RARE" ? 1 : 12;
           unsigned char data[] =
	    {
                (unsigned char)mode,
		        0,
                (unsigned char)r,
                (unsigned char)g,
                (unsigned char)b,
	    };
            tcpSocket->write(reinterpret_cast<char*>(data), sizeof(data));
	    tcpSocket->waitForBytesWritten();
	    printf("OK wrote [%d, %d, %d, %d]\n", data[0], data[1], data[2], data[3]);
	    tcpSocket->close();
	}
    }
    p->ui.pushButton->setEnabled(true);
#if 0
    let data = Buffer.from([Mode.SOLID_RARE, 0, 0, 0, 0]) //# все диоды не активны
 //let data = Buffer.from([Mode.FADE_ANIM, 0, 0, 128, 0]) //# мерцание синим цветом
//let data = Buffer.from([Mode.SOLID_RARE, 0, 20, 243, 70]) //# 20 70 243 - мятный - горит
//let data = Buffer.from([Mode.SOLID_RARE, 0, 20, 114, 243]) //#1472f3 20 114 243
// let data = Buffer.from([Mode.SOLID_RARE, 0, 255, 255, 255]) //full bright
//let data = Buffer.from([Mode.SOLID_RARE, 10, 0, 25, 0])
 //let data = Buffer.from([Mode.SOLID_RARE, 6, 0, 0, 0])
//let data = Buffer.from([Mode.SOLID_RARE, 1, 255, 0])
// let data = Buffer.from([Mode.FADE_ANIM, 15,34,70])
 //let data = Buffer.concat([Buffer.from([Mode.TIME_CURSOR, 0, 0, 255]), Buffer.from('10 ', 'utf8')])
// let data = Buffer.concat([
//     Buffer.from([Mode.SECTION_SOLID, 0, 255, 0]),
//     Buffer.from('0 5 ', 'utf8')
// ])
 //let data = Buffer.concat([
 //    Buffer.from([Mode.SECTION_SOLID, 20, 243, 70]),
 //    Buffer.from('10 20 ', 'utf8')
// ])

function send(data) {
    client.connect({port: 8888, host: HOST}, function () {
        console.log("TCP connection established with the server.")
        client.write(data)
    })
}
send(data)
#endif
}

#if 0
void QEspLamp::on_buttonBox_clicked(QAbstractButton *btn)
{
    QDialogButtonBox::ButtonRole role = p->ui.buttonBox->buttonRole(btn);

    switch(role) {
    case QDialogButtonBox::AcceptRole:
    case QDialogButtonBox::ApplyRole:
        // Explicitly select the color
        p->ui.preview->setComparisonColor(color());
        Q_EMIT colorSelected(color());
        break;

    case QDialogButtonBox::ActionRole:
        // Currently, the only action button is the "pick color" button
        grabMouse(Qt::CrossCursor);
        p->pick_from_screen = true;
        break;

    case QDialogButtonBox::ResetRole:
        // Restore old color
        setColorInternal(p->ui.preview->comparisonColor());
        break;

    default: break;
    }
}
#endif

void QEspLamp::dragEnterEvent(QDragEnterEvent *event)
{
    if ( event->mimeData()->hasColor() ||
         ( event->mimeData()->hasText() && QColor(event->mimeData()->text()).isValid() ) )
        event->acceptProposedAction();
}


void QEspLamp::dropEvent(QDropEvent *event)
{
    if ( event->mimeData()->hasColor() )
    {
        setColorInternal(event->mimeData()->colorData().value<QColor>());
        event->accept();
    }
    else if ( event->mimeData()->hasText() )
    {
        QColor col(event->mimeData()->text());
        if ( col.isValid() )
        {
            setColorInternal(col);
            event->accept();
        }
    }
}

void QEspLamp::mouseReleaseEvent(QMouseEvent *event)
{
    if (p->pick_from_screen)
    {
        setColorInternal(utils::get_screen_color(event->globalPos()));
        p->pick_from_screen = false;
        releaseMouse();
    }
}

void QEspLamp::mouseMoveEvent(QMouseEvent *event)
{
    if (p->pick_from_screen)
    {
        setColorInternal(utils::get_screen_color(event->globalPos()));
    }
}

void QEspLamp::setWheelShape(ColorWheel::ShapeEnum shape)
{
    p->ui.wheel->setSelectorShape(shape);
}

ColorWheel::ShapeEnum QEspLamp::wheelShape() const
{
    return p->ui.wheel->selectorShape();
}

void QEspLamp::setColorSpace(ColorWheel::ColorSpaceEnum space)
{
    p->ui.wheel->setColorSpace(space);
}

ColorWheel::ColorSpaceEnum QEspLamp::colorSpace() const
{
    return p->ui.wheel->colorSpace();
}

void QEspLamp::setWheelRotating(bool rotating)
{
    p->ui.wheel->setRotatingSelector(rotating);
}

bool QEspLamp::wheelRotating() const
{
    return p->ui.wheel->rotatingSelector();
}

int main(int argc, char *argv[])
{
        QApplication a(argc, argv);
        QEspLamp t;
        t.show();
        return a.exec();
}
