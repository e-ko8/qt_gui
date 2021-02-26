#include <QApplication>
#include <QPushButton>
#include <QScreen>

#include "main.h"
#include "ui_main.h"
#include "color_dialog.hpp"

ColorDialogTest::ColorDialogTest(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::ColorDialogTest)
{
	ui->setupUi(this);

	auto button = new QPushButton("Show", this);
	connect(button, &QPushButton::clicked, this, [&]()
	{
		color_widgets::ColorDialog* c = new color_widgets::ColorDialog();
		c->exec();
	});
}

ColorDialogTest::~ColorDialogTest()
{
	delete ui;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ColorDialogTest t;
	t.show();
	return a.exec();
}

