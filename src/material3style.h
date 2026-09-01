/*
 * SPDX-FileCopyrightText: 2026 KDE Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef MATERIAL3STYLE_H
#define MATERIAL3STYLE_H

#include "dolphin_export.h"
#include "m3colorengine.h"

#include <QCommonStyle>

/**
 * @brief Native Qt6 QStyle engine implementing Google's Material Design 3 (Material You / MD3) specifications.
 *
 * Dynamically driven by Matugen color tokens from M3ColorEngine.
 */
class DOLPHIN_EXPORT Material3Style : public QCommonStyle
{
    Q_OBJECT

public:
    explicit Material3Style();
    ~Material3Style() override;

    void polish(QWidget *widget) override;
    void unpolish(QWidget *widget) override;
    void polish(QApplication *app) override;
    void polish(QPalette &palette) override;

    int pixelMetric(PixelMetric metric, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;
    int styleHint(StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

    QRect subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget = nullptr) const override;
    QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget = nullptr) const override;
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget = nullptr) const override;

    QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

private:
    const M3ColorScheme &scheme() const;
    void renderPillButton(const QRectF &rect, const QStyleOption *option, QPainter *painter, const QWidget *widget, qreal radius) const;
};

#endif // MATERIAL3STYLE_H
