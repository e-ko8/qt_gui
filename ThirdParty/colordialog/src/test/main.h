#ifndef MAIN_H
#define MAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ColorDialogTest; }
QT_END_NAMESPACE

class ColorDialogTest : public QMainWindow
{
        Q_OBJECT

public:
        ColorDialogTest(QWidget *parent = nullptr);

        ~ColorDialogTest();

private:
        Ui::ColorDialogTest *ui;
};

#endif // MAIN_H

