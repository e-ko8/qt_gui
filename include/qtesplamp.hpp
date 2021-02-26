#ifndef QESPLAMP_HPP
#define QESPLAMP_HPP

#include "colorwidgets_global.hpp"
#include "color_preview.hpp"
#include "color_wheel.hpp"

#include <QWidget>

class QAbstractButton;

class QCP_EXPORT QEspLamp : public QWidget
{
    Q_OBJECT
    Q_ENUMS(ButtonMode)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged DESIGNABLE true)
    Q_PROPERTY(color_widgets::ColorWheel::ShapeEnum wheelShape READ wheelShape WRITE setWheelShape NOTIFY wheelShapeChanged)
    Q_PROPERTY(color_widgets::ColorWheel::ColorSpaceEnum colorSpace READ colorSpace WRITE setColorSpace NOTIFY colorSpaceChanged)
    Q_PROPERTY(bool wheelRotating READ wheelRotating WRITE setWheelRotating NOTIFY wheelRotatingChanged)
    /**
     * \brief whether the color alpha channel can be edited.
     *
     * If alpha is disabled, the selected color's alpha will always be 255.
     */
    Q_PROPERTY(bool alphaEnabled READ alphaEnabled WRITE setAlphaEnabled NOTIFY alphaEnabledChanged)

public:
    enum ButtonMode {
        OkCancel,
        OkApplyCancel,
        Close
    };

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

    bool alphaEnabled() const;

    /**
     * Select which dialog buttons to show
     *
     * There are three predefined modes:
     * OkCancel - this is useful when the dialog is modal and we just want to return a color
     * OkCancelApply - this is for non-modal dialogs
     * Close - for non-modal dialogs with direct color updates via colorChanged signal
     */
    void setButtonMode(ButtonMode mode);
    ButtonMode buttonMode() const;

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

    /**
     * Set whether the color alpha channel can be edited.
     * If alpha is disabled, the selected color's alpha will always be 255.
     */
    void setAlphaEnabled(bool a);

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

    void alphaEnabledChanged(bool alphaEnabled);

private Q_SLOTS:
    /// Update all the Ui elements to match the selected color
    void setColorInternal(const QColor &color);
    /// Update from HSV sliders
    void set_hsv();
    /// Update from RGB sliders
    void set_rgb();
    /// Update from Alpha slider
    void set_alpha();

    void on_edit_hex_colorChanged(const QColor& color);
    void on_edit_hex_colorEditingFinished(const QColor& color);
#if 0
    void on_buttonBox_clicked(QAbstractButton*);
#endif

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent * event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    class Private;
    Private * const p;
};

#endif // QESPLAMP_HPP

