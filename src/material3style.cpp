/*
 * SPDX-FileCopyrightText: 2026 KDE Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "material3style.h"

#include <QApplication>
#include <QComboBox>
#include <QFocusFrame>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScrollBar>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionComplex>
#include <QStyleOptionMenuItem>
#include <QStyleOptionTab>
#include <QStyleOptionToolButton>
#include <QStyleOptionViewItem>
#include <QTabBar>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>
#include <QtMath>

Material3Style::Material3Style()
    : QCommonStyle()
{
}

Material3Style::~Material3Style() = default;

const M3ColorScheme &Material3Style::scheme() const
{
    return M3ColorEngine::instance()->scheme();
}

void Material3Style::polish(QWidget *widget)
{
    if (!widget)
        return;

    const auto &s = scheme();

    if (qobject_cast<QLineEdit *>(widget) || qobject_cast<QComboBox *>(widget)) {
        widget->setAttribute(Qt::WA_Hover, true);
    } else if (qobject_cast<QAbstractButton *>(widget)) {
        widget->setAttribute(Qt::WA_Hover, true);
    } else if (qobject_cast<QMenu *>(widget)) {
        widget->setAttribute(Qt::WA_TranslucentBackground, true);
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
    } else if (qobject_cast<QToolBar *>(widget)) {
        QPalette p = widget->palette();
        p.setColor(QPalette::Window, s.surface);
        p.setColor(QPalette::Base, s.surface);
        p.setColor(QPalette::Button, s.surface);
        widget->setPalette(p);
    }

    QCommonStyle::polish(widget);
}

void Material3Style::unpolish(QWidget *widget)
{
    QCommonStyle::unpolish(widget);
}

void Material3Style::polish(QApplication *app)
{
    QCommonStyle::polish(app);
}

void Material3Style::polish(QPalette &palette)
{
    palette = M3ColorEngine::instance()->generateQPalette();
    QCommonStyle::polish(palette);
}

int Material3Style::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_ButtonMargin:
        return 12;
    case PM_ScrollBarExtent:
        return 10; // Minimal floating scrollbar
    case PM_ScrollBarSliderMin:
        return 28;
    case PM_MenuHMargin:
    case PM_MenuVMargin:
        return 8;
    case PM_TabBarTabHSpace:
        return 16;
    case PM_TabBarTabVSpace:
        return 8;
    case PM_DefaultFrameWidth:
        return 1;
    case PM_LayoutHorizontalSpacing:
    case PM_LayoutVerticalSpacing:
        return 8;
    case PM_ExclusiveIndicatorWidth:
    case PM_ExclusiveIndicatorHeight:
    case PM_IndicatorWidth:
    case PM_IndicatorHeight:
        return 20;
    case PM_ToolBarIconSize:
        return 24;
    case PM_ButtonDefaultIndicator:
        return 0;
    case PM_FocusFrameVMargin:
    case PM_FocusFrameHMargin:
        return 2;
    case PM_SplitterWidth:
        return 4;
    case PM_HeaderMargin:
        return 6;
    default:
        return QCommonStyle::pixelMetric(metric, option, widget);
    }
}

int Material3Style::styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    switch (hint) {
    case SH_EtchDisabledText:
        return 0;
    case SH_ItemView_PaintAlternatingRowColorsForEmptyArea:
        return 0;
    case SH_ScrollBar_Transient:
        return 0;
    case SH_Menu_SupportsSections:
        return 1;
    case SH_Menu_SubMenuPopupDelay:
        return 100;
    case SH_ComboBox_Popup:
        return 1;
    case SH_ToolButtonStyle:
        return Qt::ToolButtonFollowStyle;
    case SH_RequestSoftwareInputPanel:
        return RSIP_OnMouseClick;
    case SH_UnderlineShortcut:
        return 0;
    default:
        return QCommonStyle::styleHint(hint, option, widget, returnData);
    }
}

void Material3Style::renderPillButton(const QRectF &rect, const QStyleOption *option, QPainter *painter, const QWidget *widget, qreal radius) const
{
    Q_UNUSED(widget)
    const auto &s = scheme();
    const bool isHovered = option->state & State_MouseOver;
    const bool isPressed = option->state & State_Sunken || option->state & State_On;
    const bool isFocused = option->state & State_HasFocus;
    const bool isEnabled = option->state & State_Enabled;

    const auto btnOpt = qstyleoption_cast<const QStyleOptionButton *>(option);
    const bool isDefault = btnOpt && (btnOpt->features & QStyleOptionButton::DefaultButton);

    QColor baseColor = isDefault ? s.primary : s.surfaceContainer;
    QColor contentColor = isDefault ? s.onPrimary : s.onSurface;

    if (!isEnabled) {
        baseColor = s.stateLayer(s.surface, s.surfaceContainer, 0.12);
        contentColor = s.stateLayer(s.surface, s.onSurface, 0.38);
    } else if (isPressed) {
        baseColor = s.stateLayer(baseColor, contentColor, 0.12);
    } else if (isHovered) {
        baseColor = s.stateLayer(baseColor, contentColor, 0.08);
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    const QRectF r = rect.adjusted(0.5, 0.5, -0.5, -0.5);
    path.addRoundedRect(r, radius, radius);

    painter->fillPath(path, baseColor);

    if (isEnabled) {
        if (isFocused) {
            QPen focusPen(s.primary, 2.0);
            painter->strokePath(path, focusPen);
        } else if (!isDefault) {
            QPen borderPen(s.outlineVariant, 1.0);
            painter->strokePath(path, borderPen);
        }
    }

    painter->restore();
}

void Material3Style::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto &s = scheme();

    switch (element) {
    case PE_PanelLineEdit: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool hasFocus = option->state & State_HasFocus;
        const bool isHovered = option->state & State_MouseOver;
        const bool isEnabled = option->state & State_Enabled;

        QRectF r = option->rect;
        r.adjust(0.5, 0.5, -0.5, -0.5);

        // 28dp radius pill for single-line search/address bars, or 12dp for multiline
        const qreal radius = qMin(r.height() / 2.0, 28.0);
        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        QColor bg = s.surfaceContainer;
        if (!isEnabled) {
            bg = s.stateLayer(s.surface, s.surfaceContainer, 0.12);
        } else if (isHovered && !hasFocus) {
            bg = s.stateLayer(bg, s.onSurface, 0.04);
        }

        painter->fillPath(path, bg);

        if (hasFocus && isEnabled) {
            QPen focusPen(s.primary, 2.0);
            painter->strokePath(path, focusPen);
        } else {
            QPen borderPen(s.outlineVariant, 1.0);
            painter->strokePath(path, borderPen);
        }

        painter->restore();
        break;
    }

    case PE_PanelButtonCommand: {
        const qreal radius = qMin(option->rect.height() / 2.0, 20.0);
        renderPillButton(option->rect, option, painter, widget, radius);
        break;
    }

    case PE_PanelButtonTool: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isHovered = option->state & State_MouseOver;
        const bool isPressed = option->state & State_Sunken || option->state & State_On;
        const bool isEnabled = option->state & State_Enabled;
        const bool isChecked = option->state & State_On;

        QRectF r = option->rect;
        r.adjust(0.5, 0.5, -0.5, -0.5);
        const qreal radius = qMin(r.height() / 2.0, 16.0);

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        if (isChecked) {
            QColor fill = s.secondaryContainer;
            if (isPressed) {
                fill = s.stateLayer(fill, s.onSecondaryContainer, 0.12);
            } else if (isHovered) {
                fill = s.stateLayer(fill, s.onSecondaryContainer, 0.08);
            }
            painter->fillPath(path, fill);
        } else if (isEnabled && (isHovered || isPressed)) {
            QColor fill = s.stateLayer(s.surface, s.onSurface, isPressed ? 0.12 : 0.08);
            painter->fillPath(path, fill);
        }

        painter->restore();
        break;
    }

    case PE_PanelMenu: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        QRectF r = option->rect;
        r.adjust(0.5, 0.5, -0.5, -0.5);
        const qreal radius = 16.0; // 16dp MD3 corner radius for context menus and sheets

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        painter->fillPath(path, s.surfaceContainerHigh);

        QPen borderPen(s.outlineVariant, 1.0);
        painter->strokePath(path, borderPen);

        painter->restore();
        break;
    }

    case PE_FrameMenu:
        // Already drawn cleanly in PE_PanelMenu
        break;

    case PE_Frame:
    case PE_FrameWindow: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        QRectF r = option->rect;
        r.adjust(0.5, 0.5, -0.5, -0.5);
        QPainterPath path;
        path.addRoundedRect(r, 12.0, 12.0);
        QPen borderPen(s.outlineVariant, 1.0);
        painter->strokePath(path, borderPen);
        painter->restore();
        break;
    }

    case PE_FrameFocusRect:
        // Handled via focus stroke in controls
        break;

    case PE_PanelItemViewItem: {
        const auto vopt = qstyleoption_cast<const QStyleOptionViewItem *>(option);
        if (!vopt)
            break;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isSelected = option->state & State_Selected;
        const bool isHovered = option->state & State_MouseOver;
        const bool hasFocus = option->state & State_HasFocus;

        QRectF r = option->rect;
        r.adjust(2.0, 2.0, -2.0, -2.0);
        const qreal radius = 12.0; // 12dp MD3 card corner radius

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        if (isSelected) {
            // primary-container fill with subtle 1px primary outline
            painter->fillPath(path, s.primaryContainer);
            QPen pen(s.primary, 1.0);
            painter->strokePath(path, pen);
        } else if (isHovered) {
            // surface-container-highest / 8% state layer
            QColor hoverColor = s.surfaceContainerHighest;
            hoverColor.setAlphaF(0.60);
            painter->fillPath(path, hoverColor);
        }

        if (hasFocus && !isSelected) {
            QPen pen(s.primary, 1.5);
            painter->strokePath(path, pen);
        }

        painter->restore();
        break;
    }

    case PE_IndicatorCheckBox: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isChecked = option->state & State_On;
        const bool isHovered = option->state & State_MouseOver;
        const bool isEnabled = option->state & State_Enabled;

        QRectF r = option->rect;
        r.adjust(1.0, 1.0, -1.0, -1.0);
        const qreal radius = 4.0;

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        if (isChecked) {
            QColor bg = s.primary;
            if (!isEnabled) {
                bg = s.stateLayer(s.surface, s.primary, 0.38);
            } else if (isHovered) {
                bg = s.stateLayer(bg, s.onPrimary, 0.08);
            }
            painter->fillPath(path, bg);

            // Draw checkmark
            QPen checkPen(s.onPrimary, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter->setPen(checkPen);

            QPainterPath checkPath;
            checkPath.moveTo(r.left() + r.width() * 0.25, r.top() + r.height() * 0.50);
            checkPath.lineTo(r.left() + r.width() * 0.45, r.top() + r.height() * 0.70);
            checkPath.lineTo(r.left() + r.width() * 0.75, r.top() + r.height() * 0.30);
            painter->strokePath(checkPath, checkPen);
        } else {
            QColor border = isEnabled ? (isHovered ? s.onSurface : s.outline) : s.outlineVariant;
            QPen borderPen(border, 2.0);
            painter->strokePath(path, borderPen);
        }

        painter->restore();
        break;
    }

    case PE_IndicatorRadioButton: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isChecked = option->state & State_On;
        const bool isHovered = option->state & State_MouseOver;
        const bool isEnabled = option->state & State_Enabled;

        QRectF r = option->rect;
        r.adjust(1.0, 1.0, -1.0, -1.0);

        if (isChecked) {
            QPen circlePen(isEnabled ? s.primary : s.outlineVariant, 2.0);
            painter->setPen(circlePen);
            painter->drawEllipse(r);

            // Center dot
            QRectF dotRect = r.adjusted(r.width() * 0.25, r.height() * 0.25, -r.width() * 0.25, -r.height() * 0.25);
            QPainterPath dotPath;
            dotPath.addEllipse(dotRect);
            painter->fillPath(dotPath, isEnabled ? s.primary : s.outlineVariant);
        } else {
            QColor border = isEnabled ? (isHovered ? s.onSurface : s.outline) : s.outlineVariant;
            QPen borderPen(border, 2.0);
            painter->setPen(borderPen);
            painter->drawEllipse(r);
        }

        painter->restore();
        break;
    }

    case PE_IndicatorBranch: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isChildren = option->state & State_Children;
        const bool isOpen = option->state & State_Open;

        if (isChildren) {
            const QRectF r = option->rect;
            const QPointF center = r.center();
            const qreal sz = 5.0;

            QPainterPath path;
            if (isOpen) {
                // Down chevron
                path.moveTo(center.x() - sz, center.y() - sz * 0.5);
                path.lineTo(center.x(), center.y() + sz * 0.5);
                path.lineTo(center.x() + sz, center.y() - sz * 0.5);
            } else {
                // Right chevron
                path.moveTo(center.x() - sz * 0.5, center.y() - sz);
                path.lineTo(center.x() + sz * 0.5, center.y());
                path.lineTo(center.x() - sz * 0.5, center.y() + sz);
            }

            QPen pen(s.onSurfaceVariant, 1.75, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter->strokePath(path, pen);
        }

        painter->restore();
        break;
    }

    case PE_PanelStatusBar: {
        painter->save();
        painter->fillRect(option->rect, s.surfaceContainerLow);
        QPen divider(s.outlineVariant, 1.0);
        painter->setPen(divider);
        painter->drawLine(option->rect.topLeft(), option->rect.topRight());
        painter->restore();
        break;
    }

    default:
        QCommonStyle::drawPrimitive(element, option, painter, widget);
        break;
    }
}

void Material3Style::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    const auto &s = scheme();

    switch (element) {
    case CE_MenuItem: {
        const auto menuOpt = qstyleoption_cast<const QStyleOptionMenuItem *>(option);
        if (!menuOpt)
            break;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isHovered = menuOpt->state & State_Selected;
        const bool isEnabled = menuOpt->state & State_Enabled;

        QRectF r = menuOpt->rect;
        r.adjust(4.0, 2.0, -4.0, -2.0);

        if (menuOpt->menuItemType == QStyleOptionMenuItem::Separator) {
            painter->setPen(QPen(s.outlineVariant, 1.0));
            const int midY = menuOpt->rect.center().y();
            painter->drawLine(menuOpt->rect.left() + 8, midY, menuOpt->rect.right() - 8, midY);
            painter->restore();
            return;
        }

        if (isHovered && isEnabled) {
            QPainterPath pillPath;
            pillPath.addRoundedRect(r, 12.0, 12.0);
            painter->fillPath(pillPath, s.stateLayer(s.surfaceContainerHigh, s.onSurface, 0.08));
        }

        // Draw Icon
        int textLeft = r.left() + 12;
        if (!menuOpt->icon.isNull()) {
            const int iconSz = pixelMetric(PM_SmallIconSize, option, widget);
            const QRect iconRect(r.left() + 8, r.top() + (r.height() - iconSz) / 2, iconSz, iconSz);
            const QIcon::Mode mode = isEnabled ? (isHovered ? QIcon::Active : QIcon::Normal) : QIcon::Disabled;
            menuOpt->icon.paint(painter, iconRect, Qt::AlignCenter, mode);
            textLeft = iconRect.right() + 12;
        }

        // Draw Text & Shortcut
        const QString fullText = menuOpt->text;
        const QStringList parts = fullText.split(QLatin1Char('\t'));
        const QString text = parts.value(0);
        const QString shortcut = parts.value(1);

        QFont font = painter->font();
        painter->setFont(font);

        QColor textColor = isEnabled ? s.onSurface : s.stateLayer(s.surface, s.onSurface, 0.38);
        painter->setPen(textColor);

        const QRect textRect(textLeft, r.top(), r.width() - (textLeft - r.left()) - 40, r.height());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

        if (!shortcut.isEmpty()) {
            painter->setPen(s.onSurfaceVariant);
            const QRect shortcutRect(r.right() - 80, r.top(), 72, r.height());
            painter->drawText(shortcutRect, Qt::AlignRight | Qt::AlignVCenter, shortcut);
        }

        painter->restore();
        break;
    }

    case CE_TabBarTab: {
        const auto tabOpt = qstyleoption_cast<const QStyleOptionTab *>(option);
        if (!tabOpt)
            break;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isSelected = tabOpt->state & State_Selected;
        const bool isHovered = tabOpt->state & State_MouseOver;

        QRectF r = tabOpt->rect;
        r.adjust(2.0, 4.0, -2.0, -4.0);
        const qreal radius = qMin(r.height() / 2.0, 16.0);

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        if (isSelected) {
            painter->fillPath(path, s.secondaryContainer);
        } else if (isHovered) {
            painter->fillPath(path, s.stateLayer(s.surfaceContainer, s.onSurface, 0.08));
        } else {
            painter->fillPath(path, s.surfaceContainer);
        }

        // Tab text
        QFont font = painter->font();
        if (isSelected) {
            font.setWeight(QFont::DemiBold);
        }
        painter->setFont(font);

        const QColor textColor = isSelected ? s.onSecondaryContainer : s.onSurfaceVariant;
        painter->setPen(textColor);

        QRect textRect = tabOpt->rect;
        if (!tabOpt->icon.isNull()) {
            const int iconSz = pixelMetric(PM_SmallIconSize, option, widget);
            const QRect iconRect(tabOpt->rect.left() + 8, tabOpt->rect.top() + (tabOpt->rect.height() - iconSz) / 2, iconSz, iconSz);
            tabOpt->icon.paint(painter, iconRect);
            textRect.setLeft(iconRect.right() + 6);
        }

        painter->drawText(textRect, Qt::AlignCenter, tabOpt->text);
        painter->restore();
        break;
    }

    case CE_PushButtonLabel:
    case CE_ToolButtonLabel: {
        painter->save();
        QFont font = painter->font();
        font.setWeight(QFont::DemiBold);
        painter->setFont(font);

        QCommonStyle::drawControl(element, option, painter, widget);
        painter->restore();
        break;
    }

    case CE_Header:
    case CE_HeaderSection: {
        painter->save();
        painter->fillRect(option->rect, s.surfaceContainerLow);
        painter->setPen(QPen(s.outlineVariant, 1.0));
        painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
        painter->restore();
        break;
    }

    case CE_HeaderLabel: {
        const auto headerOpt = qstyleoption_cast<const QStyleOptionHeader *>(option);
        if (!headerOpt)
            break;

        painter->save();
        painter->setPen(s.onSurfaceVariant);
        QFont font = painter->font();
        font.setWeight(QFont::DemiBold);
        painter->setFont(font);

        painter->drawText(headerOpt->rect, Qt::AlignLeft | Qt::AlignVCenter, headerOpt->text);
        painter->restore();
        break;
    }

    case CE_RubberBand: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        QRectF r = option->rect;
        r.adjust(0.5, 0.5, -0.5, -0.5);

        QPainterPath path;
        path.addRoundedRect(r, 4.0, 4.0);

        QColor fill = s.primary;
        fill.setAlphaF(0.24);
        painter->fillPath(path, fill);

        QPen pen(s.primary, 1.5);
        painter->strokePath(path, pen);

        painter->restore();
        break;
    }

    case CE_ScrollBarSlider: {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        const bool isHovered = option->state & State_MouseOver;
        const bool isPressed = option->state & State_Sunken;

        QRectF r = option->rect;
        // Inset slightly for floating minimal bar effect
        r.adjust(1.5, 1.5, -1.5, -1.5);
        const qreal radius = qMin(r.width(), r.height()) / 2.0;

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        QColor thumbColor = s.outline;
        thumbColor.setAlphaF(isPressed ? 0.90 : (isHovered ? 0.75 : 0.40));

        painter->fillPath(path, thumbColor);
        painter->restore();
        break;
    }

    case CE_ScrollBarAddLine:
    case CE_ScrollBarSubLine:
    case CE_ScrollBarAddPage:
    case CE_ScrollBarSubPage:
        // Modern minimal floating scrollbar track (invisible track)
        break;

    default:
        QCommonStyle::drawControl(element, option, painter, widget);
        break;
    }
}

void Material3Style::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    const auto &s = scheme();

    switch (control) {
    case CC_ScrollBar: {
        const auto sbOpt = qstyleoption_cast<const QStyleOptionSlider *>(option);
        if (!sbOpt)
            break;

        // Draw floating slider thumb
        if (sbOpt->subControls & SC_ScrollBarSlider) {
            QStyleOptionSlider subOpt = *sbOpt;
            subOpt.rect = subControlRect(CC_ScrollBar, sbOpt, SC_ScrollBarSlider, widget);
            drawControl(CE_ScrollBarSlider, &subOpt, painter, widget);
        }
        break;
    }

    case CC_ComboBox: {
        const auto cbOpt = qstyleoption_cast<const QStyleOptionComboBox *>(option);
        if (!cbOpt)
            break;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        QRectF r = cbOpt->rect;
        r.adjust(0.5, 0.5, -0.5, -0.5);
        const qreal radius = qMin(r.height() / 2.0, 20.0);

        QPainterPath path;
        path.addRoundedRect(r, radius, radius);

        QColor bg = s.surfaceContainer;
        if (cbOpt->state & State_MouseOver) {
            bg = s.stateLayer(bg, s.onSurface, 0.04);
        }
        painter->fillPath(path, bg);

        if (cbOpt->state & State_HasFocus) {
            painter->strokePath(path, QPen(s.primary, 2.0));
        } else {
            painter->strokePath(path, QPen(s.outlineVariant, 1.0));
        }

        // Draw Arrow
        const QRect arrowRect = subControlRect(CC_ComboBox, cbOpt, SC_ComboBoxArrow, widget);
        const QPointF center = arrowRect.center();
        const qreal sz = 4.0;

        QPainterPath arrowPath;
        arrowPath.moveTo(center.x() - sz, center.y() - sz * 0.5);
        arrowPath.lineTo(center.x(), center.y() + sz * 0.5);
        arrowPath.lineTo(center.x() + sz, center.y() - sz * 0.5);

        painter->strokePath(arrowPath, QPen(s.onSurfaceVariant, 1.75, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        painter->restore();
        break;
    }

    default:
        QCommonStyle::drawComplexControl(control, option, painter, widget);
        break;
    }
}

QRect Material3Style::subElementRect(SubElement subElement, const QStyleOption *option, const QWidget *widget) const
{
    switch (subElement) {
    case SE_LineEditContents: {
        QRect r = QCommonStyle::subElementRect(subElement, option, widget);
        r.adjust(12, 0, -12, 0);
        return r;
    }
    case SE_PushButtonContents: {
        QRect r = QCommonStyle::subElementRect(subElement, option, widget);
        r.adjust(8, 0, -8, 0);
        return r;
    }
    case SE_HeaderLabel: {
        QRect r = QCommonStyle::subElementRect(subElement, option, widget);
        r.adjust(8, 0, -8, 0);
        return r;
    }
    default:
        return QCommonStyle::subElementRect(subElement, option, widget);
    }
}

QRect Material3Style::subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const
{
    return QCommonStyle::subControlRect(control, option, subControl, widget);
}

QSize Material3Style::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    QSize sz = QCommonStyle::sizeFromContents(type, option, size, widget);

    switch (type) {
    case CT_PushButton:
        sz.setHeight(qMax(sz.height(), 40));
        sz.setWidth(sz.width() + 24);
        break;
    case CT_LineEdit:
        sz.setHeight(qMax(sz.height(), 48)); // 48dp MD3 search & address bar target height
        sz.setWidth(sz.width() + 24);
        break;
    case CT_ComboBox:
        sz.setHeight(qMax(sz.height(), 40));
        sz.setWidth(sz.width() + 24);
        break;
    case CT_TabBarTab:
        sz.setHeight(qMax(sz.height(), 36));
        sz.setWidth(sz.width() + 24);
        break;
    case CT_MenuItem:
        sz.setHeight(qMax(sz.height(), 36));
        sz.setWidth(sz.width() + 16);
        break;
    default:
        break;
    }

    return sz;
}

QIcon Material3Style::standardIcon(StandardPixmap standardIcon, const QStyleOption *option, const QWidget *widget) const
{
    return QCommonStyle::standardIcon(standardIcon, option, widget);
}
