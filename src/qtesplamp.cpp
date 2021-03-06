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
#include <QMessageBox>

#include "color_dialog.hpp"
#include "color_utils.hpp"

using namespace color_widgets;

class QEspLamp::Private
{
public :
	Ui_QEspLamp ui;
	bool pick_from_screen;
	QColor color;

	Private() : pick_from_screen(false) { }
};

QEspLamp::QEspLamp(QWidget *parent) :
	QWidget(parent), p(new Private), tcpSocket(new QTcpSocket(this))
{
	p->ui.setupUi(this);

	setAcceptDrops(true);

	connect(p->ui.wheel, &ColorWheel::colorSpaceChanged, this, &QEspLamp::colorSpaceChanged);
	connect(p->ui.wheel, &ColorWheel::selectorShapeChanged, this, &QEspLamp::wheelShapeChanged);
	connect(p->ui.wheel, &ColorWheel::rotatingSelectorChanged, this, &QEspLamp::wheelRotatingChanged);

	connect(tcpSocket,  &QTcpSocket::connected, [this] ()
	{
		int r, g, b, a;
		p->color.getRgb(&r, &g, &b, &a);
		if (int mode = p->ui.modeComboBox->currentText().toStdString() == "SOLID_RARE")
		{
			unsigned char data[] = { 2, 0, static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b) };
			tcpSocket->write(reinterpret_cast<char *>(data), sizeof(data));
			tcpSocket->waitForBytesWritten();
			printf("OK wrote [ %d, %d, %d, %d, %d]\n", data[0], data[1], data[2], data[3], data[4]);
		}
		else
		{
			unsigned char data[] = { 12, static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b) };
			tcpSocket->write(reinterpret_cast<char *>(data), sizeof(data));
			tcpSocket->waitForBytesWritten();
			printf("OK wrote [ %d, %d, %d, %d]\n", data[0], data[1], data[2], data[3]);
		}

		tcpSocket->close();
	});

	connect(tcpSocket, &QTcpSocket::disconnected, [this] ()
	{
		p->ui.pushButton->setEnabled(true);
	});

	connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
		[this] (QAbstractSocket::SocketError socketError)
	{
		p->ui.pushButton->setEnabled(true);

#define QSOCKETERRSTR(name) case QAbstractSocket::name : strErr = #name; break;
		std::string strErr;
		switch (socketError)
		{
		QSOCKETERRSTR(ConnectionRefusedError);
		QSOCKETERRSTR(RemoteHostClosedError);
		QSOCKETERRSTR(HostNotFoundError);
		QSOCKETERRSTR(SocketAccessError);
		QSOCKETERRSTR(SocketResourceError);
		QSOCKETERRSTR(SocketTimeoutError);
		QSOCKETERRSTR(DatagramTooLargeError);
		QSOCKETERRSTR(NetworkError);
		QSOCKETERRSTR(AddressInUseError);
		QSOCKETERRSTR(SocketAddressNotAvailableError);
		QSOCKETERRSTR(UnsupportedSocketOperationError);
		QSOCKETERRSTR(ProxyAuthenticationRequiredError);
		QSOCKETERRSTR(SslHandshakeFailedError);
		QSOCKETERRSTR(UnfinishedSocketOperationError);
		QSOCKETERRSTR(ProxyConnectionRefusedError);
		QSOCKETERRSTR(ProxyConnectionClosedError);
		QSOCKETERRSTR(ProxyConnectionTimeoutError);
		QSOCKETERRSTR(ProxyNotFoundError);
		QSOCKETERRSTR(ProxyProtocolError);
		QSOCKETERRSTR(OperationError);
		QSOCKETERRSTR(SslInternalError);
		QSOCKETERRSTR(SslInvalidUserDataError);
		QSOCKETERRSTR(TemporaryError);
		default: strErr = "UnknownSocketError";
		}

		QMessageBox messageBox;
		messageBox.critical(0, "Error", QString::fromStdString(strErr));
		messageBox.show();
	});
}

QEspLamp::~QEspLamp()
{
	delete p;
}

QSize QEspLamp::sizeHint() const
{
	return QSize(400, 0);
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


	if (!p->ui.edit_hex->isModified() )
		p->ui.edit_hex->setColor(col);

	p->ui.preview->setColor(col);

	blockSignals(blocked);
	Q_FOREACH(QWidget* w, findChildren<QWidget*>())
		w->blockSignals(false);

	Q_EMIT colorChanged(col);
}

void QEspLamp::set_hsv()
{
	if (signalsBlocked()) return;

	QColor col = QColor::fromHsv(
		p->ui.slide_hue->value(),
		p->ui.slide_saturation->value(),
		p->ui.slide_value->value());
	p->ui.wheel->setColor(col);
	setColorInternal(col);
}

void QEspLamp::set_rgb()
{
	if (signalsBlocked()) return;

	QColor col(
		p->ui.slide_red->value(),
		p->ui.slide_green->value(),
		p->ui.slide_blue->value());
	if (col.saturation() == 0)
		col = QColor::fromHsv(p->ui.slide_hue->value(), 0, col.value());
	p->ui.wheel->setColor(col);
	setColorInternal(col);
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
	tcpSocket->connectToHost(p->ui.hostLineEdit->text(), p->ui.portSpinBox->value());
}

void QEspLamp::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasColor() ||
		 (event->mimeData()->hasText() && QColor(event->mimeData()->text()).isValid()))
		event->acceptProposedAction();
}


void QEspLamp::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasColor())
	{
		setColorInternal(event->mimeData()->colorData().value<QColor>());
		event->accept();
	}
	else if (event->mimeData()->hasText())
	{
		QColor col(event->mimeData()->text());
		if (col.isValid() )
		{
			setColorInternal(col);
			event->accept();
		}
	}
}

void QEspLamp::mouseReleaseEvent(QMouseEvent *event)
{
	if (!p->pick_from_screen) return;
		
	setColorInternal(utils::get_screen_color(event->globalPos()));
	p->pick_from_screen = false;
	releaseMouse();
}

void QEspLamp::mouseMoveEvent(QMouseEvent *event)
{
	if (!p->pick_from_screen) return;
	
	setColorInternal(utils::get_screen_color(event->globalPos()));
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

