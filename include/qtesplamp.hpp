#ifndef QESPLAMP_HPP
#define QESPLAMP_HPP

#include "colorwidgets_global.hpp"
#include "color_preview.hpp"
#include "color_wheel.hpp"

#include <QWidget>
#include <QTcpSocket>

class QAbstractButton;

class QCP_EXPORT QEspLamp : public QWidget
{
    Q_OBJECT
    Q_ENUMS(ButtonMode)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged DESIGNABLE true)
    Q_PROPERTY(color_widgets::ColorWheel::ShapeEnum wheelShape READ wheelShape WRITE setWheelShape NOTIFY wheelShapeChanged)
    Q_PROPERTY(color_widgets::ColorWheel::ColorSpaceEnum colorSpace READ colorSpace WRITE setColorSpace NOTIFY colorSpaceChanged)
    Q_PROPERTY(bool wheelRotating READ wheelRotating WRITE setWheelRotating NOTIFY wheelRotatingChanged)

public:

    explicit QEspLamp(QWidget *parent = nullptr);

    ~QEspLamp();

    /**
     * Get currently selected color
     */
    QColor color() const;

    /**
     * Set the display mode for the color preview
     */
    void setPreviewDisplayMode(color_widgets::ColorPreview::DisplayMode mode);

    /**
     * Get the color preview diplay mode
     */
    color_widgets::ColorPreview::DisplayMode previewDisplayMode() const;

    QSize sizeHint() const;

    color_widgets::ColorWheel::ShapeEnum wheelShape() const;
    color_widgets::ColorWheel::ColorSpaceEnum colorSpace() const;
    bool wheelRotating() const;

public Q_SLOTS:

    /**
     * Change color
     */
    void setColor(const QColor &c);

	/**
     * Set the current color and show the dialog
     */
    void showColor(const QColor &oldcolor);

    void setWheelShape(color_widgets::ColorWheel::ShapeEnum shape);
    void setColorSpace(color_widgets::ColorWheel::ColorSpaceEnum space);
    void setWheelRotating(bool rotating);

Q_SIGNALS:
    /**
     * The current color was changed
     */
    void colorChanged(QColor);

    /**
     * The user selected the new color by pressing Ok/Apply
     */
    void colorSelected(QColor);

    void wheelShapeChanged(color_widgets::ColorWheel::ShapeEnum shape);
    void colorSpaceChanged(color_widgets::ColorWheel::ColorSpaceEnum space);
    void wheelRotatingChanged(bool rotating);

private Q_SLOTS:
    /// Update all the Ui elements to match the selected color
    void setColorInternal(const QColor &color);
    /// Update from HSV sliders
    void set_hsv();
    /// Update from RGB sliders
    void set_rgb();

    void on_edit_hex_colorChanged(const QColor& color);
    void on_edit_hex_colorEditingFinished(const QColor& color);
    void on_submitClicked();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent * event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    class Private;
    Private * const p;
    QTcpSocket *tcpSocket = nullptr;
};

#endif // QESPLAMP_HPP

