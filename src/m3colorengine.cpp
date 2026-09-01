/*
 * SPDX-FileCopyrightText: 2026 KDE Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "m3colorengine.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>
#include <QWidget>
#include <QtMath>

M3ColorScheme M3ColorScheme::defaultDark()
{
    M3ColorScheme s;
    s.isDark = true;

    s.surface = QColor(0x11, 0x13, 0x18);
    s.onSurface = QColor(0xe2, 0xe2, 0xe9);
    s.surfaceVariant = QColor(0x44, 0x47, 0x4f);
    s.onSurfaceVariant = QColor(0xc4, 0xc6, 0xd0);
    s.surfaceContainerLowest = QColor(0x0c, 0x0e, 0x13);
    s.surfaceContainerLow = QColor(0x19, 0x1b, 0x20);
    s.surfaceContainer = QColor(0x1d, 0x20, 0x24);
    s.surfaceContainerHigh = QColor(0x28, 0x2a, 0x2f);
    s.surfaceContainerHighest = QColor(0x33, 0x35, 0x3a);
    s.surfaceDim = QColor(0x11, 0x13, 0x18);
    s.surfaceBright = QColor(0x37, 0x39, 0x3e);
    s.surfaceTint = QColor(0xad, 0xc6, 0xff);
    s.inverseSurface = QColor(0xe2, 0xe2, 0xe9);
    s.inverseOnSurface = QColor(0x2f, 0x30, 0x36);

    s.primary = QColor(0xad, 0xc6, 0xff);
    s.onPrimary = QColor(0x00, 0x2e, 0x6a);
    s.primaryContainer = QColor(0x25, 0x73, 0xe6);
    s.onPrimaryContainer = QColor(0xff, 0xff, 0xff);
    s.inversePrimary = QColor(0x00, 0x5a, 0xc2);

    s.secondary = QColor(0xbf, 0xc6, 0xdc);
    s.onSecondary = QColor(0x29, 0x30, 0x41);
    s.secondaryContainer = QColor(0x3f, 0x47, 0x59);
    s.onSecondaryContainer = QColor(0xdb, 0xe2, 0xf9);

    s.tertiary = QColor(0xde, 0xbc, 0xdf);
    s.onTertiary = QColor(0x40, 0x28, 0x43);
    s.tertiaryContainer = QColor(0x58, 0x3e, 0x5b);
    s.onTertiaryContainer = QColor(0xfc, 0xd7, 0xfb);

    s.error = QColor(0xff, 0xb4, 0xab);
    s.onError = QColor(0x69, 0x00, 0x05);
    s.errorContainer = QColor(0x93, 0x00, 0x0a);
    s.onErrorContainer = QColor(0xff, 0xda, 0xd6);

    s.outline = QColor(0x8e, 0x90, 0x99);
    s.outlineVariant = QColor(0x44, 0x47, 0x4f);
    s.shadow = QColor(0x00, 0x00, 0x00);
    s.scrim = QColor(0x00, 0x00, 0x00);

    return s;
}

M3ColorScheme M3ColorScheme::defaultLight()
{
    M3ColorScheme s;
    s.isDark = false;

    s.surface = QColor(0xf9, 0xf9, 0xff);
    s.onSurface = QColor(0x19, 0x1c, 0x20);
    s.surfaceVariant = QColor(0xe1, 0xe2, 0xec);
    s.onSurfaceVariant = QColor(0x44, 0x47, 0x4f);
    s.surfaceContainerLowest = QColor(0xff, 0xff, 0xff);
    s.surfaceContainerLow = QColor(0xf3, 0xf3, 0xfa);
    s.surfaceContainer = QColor(0xed, 0xed, 0xf4);
    s.surfaceContainerHigh = QColor(0xe7, 0xe8, 0xee);
    s.surfaceContainerHighest = QColor(0xe2, 0xe2, 0xe9);
    s.surfaceDim = QColor(0xd9, 0xda, 0xe0);
    s.surfaceBright = QColor(0xf9, 0xf9, 0xff);
    s.surfaceTint = QColor(0x44, 0x5e, 0x91);
    s.inverseSurface = QColor(0x2f, 0x30, 0x36);
    s.inverseOnSurface = QColor(0xf1, 0xf0, 0xf7);

    s.primary = QColor(0x44, 0x5e, 0x91);
    s.onPrimary = QColor(0xff, 0xff, 0xff);
    s.primaryContainer = QColor(0xd8, 0xe2, 0xff);
    s.onPrimaryContainer = QColor(0x00, 0x1a, 0x42);
    s.inversePrimary = QColor(0xad, 0xc6, 0xff);

    s.secondary = QColor(0x57, 0x5e, 0x71);
    s.onSecondary = QColor(0xff, 0xff, 0xff);
    s.secondaryContainer = QColor(0xdc, 0xe2, 0xf9);
    s.onSecondaryContainer = QColor(0x14, 0x1b, 0x2c);

    s.tertiary = QColor(0x71, 0x55, 0x73);
    s.onTertiary = QColor(0xff, 0xff, 0xff);
    s.tertiaryContainer = QColor(0xfc, 0xd7, 0xfb);
    s.onTertiaryContainer = QColor(0x29, 0x13, 0x2d);

    s.error = QColor(0xba, 0x1a, 0x1a);
    s.onError = QColor(0xff, 0xff, 0xff);
    s.errorContainer = QColor(0xff, 0xda, 0xd6);
    s.onErrorContainer = QColor(0x41, 0x00, 0x02);

    s.outline = QColor(0x75, 0x77, 0x7f);
    s.outlineVariant = QColor(0xc4, 0xc6, 0xd0);
    s.shadow = QColor(0x00, 0x00, 0x00);
    s.scrim = QColor(0x00, 0x00, 0x00);

    return s;
}

QColor M3ColorScheme::stateLayer(const QColor &base, const QColor &content, qreal alpha) const
{
    alpha = qBound(0.0, alpha, 1.0);
    const qreal inv = 1.0 - alpha;
    return QColor::fromRgbF(base.redF() * inv + content.redF() * alpha,
                            base.greenF() * inv + content.greenF() * alpha,
                            base.blueF() * inv + content.blueF() * alpha,
                            base.alphaF());
}

M3ColorEngine::M3ColorEngine(QObject *parent)
    : QObject(parent)
    , m_scheme(M3ColorScheme::defaultDark())
{
    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
    m_colorsFilePath = cacheDir + QStringLiteral("/matugen/colors.json");

    setupWatcher();
    reloadColors();
}

M3ColorEngine::~M3ColorEngine() = default;

M3ColorEngine *M3ColorEngine::instance()
{
    static M3ColorEngine *s_instance = new M3ColorEngine(qApp);
    return s_instance;
}

const M3ColorScheme &M3ColorEngine::scheme() const
{
    return m_scheme;
}

bool M3ColorEngine::isDark() const
{
    return m_scheme.isDark;
}

QString M3ColorEngine::colorsFilePath() const
{
    return m_colorsFilePath;
}

void M3ColorEngine::setCustomColorsFilePath(const QString &path)
{
    if (m_colorsFilePath != path) {
        m_colorsFilePath = path;
        setupWatcher();
        reloadColors();
    }
}

qint64 M3ColorEngine::lastIngestionDurationUs() const
{
    return m_lastIngestionDurationUs;
}

void M3ColorEngine::setupWatcher()
{
    const QStringList files = m_fileWatcher.files();
    if (!files.isEmpty()) {
        m_fileWatcher.removePaths(files);
    }
    const QStringList dirs = m_fileWatcher.directories();
    if (!dirs.isEmpty()) {
        m_fileWatcher.removePaths(dirs);
    }

    QFileInfo fi(m_colorsFilePath);
    if (fi.exists()) {
        m_fileWatcher.addPath(m_colorsFilePath);
    }
    const QString dirPath = fi.dir().absolutePath();
    if (QDir(dirPath).exists()) {
        m_fileWatcher.addPath(dirPath);
    }

    connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &M3ColorEngine::onFileChanged, Qt::UniqueConnection);
    connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged, this, &M3ColorEngine::onDirectoryChanged, Qt::UniqueConnection);
}

void M3ColorEngine::onFileChanged(const QString &path)
{
    Q_UNUSED(path)
    // Re-add path in case editor replaced the inode
    if (QFile::exists(m_colorsFilePath) && !m_fileWatcher.files().contains(m_colorsFilePath)) {
        m_fileWatcher.addPath(m_colorsFilePath);
    }
    reloadColors();
}

void M3ColorEngine::onDirectoryChanged(const QString &path)
{
    Q_UNUSED(path)
    if (QFile::exists(m_colorsFilePath) && !m_fileWatcher.files().contains(m_colorsFilePath)) {
        m_fileWatcher.addPath(m_colorsFilePath);
    }
    reloadColors();
}

static QColor parseColorValue(const QJsonObject &colorsObj, const QString &key, const QString &mode, const QColor &fallback)
{
    if (!colorsObj.contains(key)) {
        return fallback;
    }

    const QJsonValue val = colorsObj.value(key);
    if (val.isString()) {
        const QColor c(val.toString());
        return c.isValid() ? c : fallback;
    }

    if (val.isObject()) {
        const QJsonObject obj = val.toObject();
        // Check mode specific e.g. "dark" or "light"
        if (obj.contains(mode)) {
            const QJsonValue modeVal = obj.value(mode);
            if (modeVal.isString()) {
                const QColor c(modeVal.toString());
                if (c.isValid())
                    return c;
            } else if (modeVal.isObject() && modeVal.toObject().contains(QStringLiteral("color"))) {
                const QColor c(modeVal.toObject().value(QStringLiteral("color")).toString());
                if (c.isValid())
                    return c;
            }
        }
        // Check "default"
        if (obj.contains(QStringLiteral("default"))) {
            const QJsonValue defVal = obj.value(QStringLiteral("default"));
            if (defVal.isString()) {
                const QColor c(defVal.toString());
                if (c.isValid())
                    return c;
            } else if (defVal.isObject() && defVal.toObject().contains(QStringLiteral("color"))) {
                const QColor c(defVal.toObject().value(QStringLiteral("color")).toString());
                if (c.isValid())
                    return c;
            }
        }
        // Check direct "color" property
        if (obj.contains(QStringLiteral("color"))) {
            const QColor c(obj.value(QStringLiteral("color")).toString());
            if (c.isValid())
                return c;
        }
        // Check "hex"
        if (obj.contains(QStringLiteral("hex"))) {
            const QColor c(obj.value(QStringLiteral("hex")).toString());
            if (c.isValid())
                return c;
        }
    }

    return fallback;
}

bool M3ColorEngine::parseJsonFile(const QString &filePath, M3ColorScheme &targetScheme)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "M3ColorEngine: JSON parse error:" << parseError.errorString();
        return false;
    }

    const QJsonObject root = doc.object();
    bool isDark = true;
    if (root.contains(QStringLiteral("is_dark_mode"))) {
        isDark = root.value(QStringLiteral("is_dark_mode")).toBool(true);
    } else if (root.contains(QStringLiteral("mode"))) {
        isDark = (root.value(QStringLiteral("mode")).toString() != QStringLiteral("light"));
    }

    targetScheme = isDark ? M3ColorScheme::defaultDark() : M3ColorScheme::defaultLight();
    targetScheme.isDark = isDark;

    const QString modeStr = isDark ? QStringLiteral("dark") : QStringLiteral("light");
    QJsonObject colorsObj;
    if (root.contains(QStringLiteral("colors")) && root.value(QStringLiteral("colors")).isObject()) {
        colorsObj = root.value(QStringLiteral("colors")).toObject();
    } else {
        colorsObj = root;
    }

    targetScheme.surface = parseColorValue(colorsObj, QStringLiteral("surface"), modeStr, targetScheme.surface);
    targetScheme.onSurface = parseColorValue(colorsObj, QStringLiteral("on_surface"), modeStr, targetScheme.onSurface);
    targetScheme.surfaceVariant = parseColorValue(colorsObj, QStringLiteral("surface_variant"), modeStr, targetScheme.surfaceVariant);
    targetScheme.onSurfaceVariant = parseColorValue(colorsObj, QStringLiteral("on_surface_variant"), modeStr, targetScheme.onSurfaceVariant);

    targetScheme.surfaceContainerLowest = parseColorValue(colorsObj, QStringLiteral("surface_container_lowest"), modeStr, targetScheme.surfaceContainerLowest);
    targetScheme.surfaceContainerLow = parseColorValue(colorsObj, QStringLiteral("surface_container_low"), modeStr, targetScheme.surfaceContainerLow);
    targetScheme.surfaceContainer = parseColorValue(colorsObj, QStringLiteral("surface_container"), modeStr, targetScheme.surfaceContainer);
    targetScheme.surfaceContainerHigh = parseColorValue(colorsObj, QStringLiteral("surface_container_high"), modeStr, targetScheme.surfaceContainerHigh);
    targetScheme.surfaceContainerHighest =
        parseColorValue(colorsObj, QStringLiteral("surface_container_highest"), modeStr, targetScheme.surfaceContainerHighest);

    targetScheme.surfaceDim = parseColorValue(colorsObj, QStringLiteral("surface_dim"), modeStr, targetScheme.surfaceDim);
    targetScheme.surfaceBright = parseColorValue(colorsObj, QStringLiteral("surface_bright"), modeStr, targetScheme.surfaceBright);
    targetScheme.surfaceTint = parseColorValue(colorsObj, QStringLiteral("surface_tint"), modeStr, targetScheme.surfaceTint);
    targetScheme.inverseSurface = parseColorValue(colorsObj, QStringLiteral("inverse_surface"), modeStr, targetScheme.inverseSurface);
    targetScheme.inverseOnSurface = parseColorValue(colorsObj, QStringLiteral("inverse_on_surface"), modeStr, targetScheme.inverseOnSurface);

    targetScheme.primary = parseColorValue(colorsObj, QStringLiteral("primary"), modeStr, targetScheme.primary);
    targetScheme.onPrimary = parseColorValue(colorsObj, QStringLiteral("on_primary"), modeStr, targetScheme.onPrimary);
    targetScheme.primaryContainer = parseColorValue(colorsObj, QStringLiteral("primary_container"), modeStr, targetScheme.primaryContainer);
    targetScheme.onPrimaryContainer = parseColorValue(colorsObj, QStringLiteral("on_primary_container"), modeStr, targetScheme.onPrimaryContainer);
    targetScheme.inversePrimary = parseColorValue(colorsObj, QStringLiteral("inverse_primary"), modeStr, targetScheme.inversePrimary);

    targetScheme.secondary = parseColorValue(colorsObj, QStringLiteral("secondary"), modeStr, targetScheme.secondary);
    targetScheme.onSecondary = parseColorValue(colorsObj, QStringLiteral("on_secondary"), modeStr, targetScheme.onSecondary);
    targetScheme.secondaryContainer = parseColorValue(colorsObj, QStringLiteral("secondary_container"), modeStr, targetScheme.secondaryContainer);
    targetScheme.onSecondaryContainer = parseColorValue(colorsObj, QStringLiteral("on_secondary_container"), modeStr, targetScheme.onSecondaryContainer);

    targetScheme.tertiary = parseColorValue(colorsObj, QStringLiteral("tertiary"), modeStr, targetScheme.tertiary);
    targetScheme.onTertiary = parseColorValue(colorsObj, QStringLiteral("on_tertiary"), modeStr, targetScheme.onTertiary);
    targetScheme.tertiaryContainer = parseColorValue(colorsObj, QStringLiteral("tertiary_container"), modeStr, targetScheme.tertiaryContainer);
    targetScheme.onTertiaryContainer = parseColorValue(colorsObj, QStringLiteral("on_tertiary_container"), modeStr, targetScheme.onTertiaryContainer);

    targetScheme.error = parseColorValue(colorsObj, QStringLiteral("error"), modeStr, targetScheme.error);
    targetScheme.onError = parseColorValue(colorsObj, QStringLiteral("on_error"), modeStr, targetScheme.onError);
    targetScheme.errorContainer = parseColorValue(colorsObj, QStringLiteral("error_container"), modeStr, targetScheme.errorContainer);
    targetScheme.onErrorContainer = parseColorValue(colorsObj, QStringLiteral("on_error_container"), modeStr, targetScheme.onErrorContainer);

    targetScheme.outline = parseColorValue(colorsObj, QStringLiteral("outline"), modeStr, targetScheme.outline);
    targetScheme.outlineVariant = parseColorValue(colorsObj, QStringLiteral("outline_variant"), modeStr, targetScheme.outlineVariant);
    targetScheme.shadow = parseColorValue(colorsObj, QStringLiteral("shadow"), modeStr, targetScheme.shadow);
    targetScheme.scrim = parseColorValue(colorsObj, QStringLiteral("scrim"), modeStr, targetScheme.scrim);

    return true;
}

QPalette M3ColorEngine::generateQPalette() const
{
    return generateQPalette(m_scheme);
}

QPalette M3ColorEngine::generateQPalette(const M3ColorScheme &scheme) const
{
    QPalette palette;

    // Normal / Active Group
    palette.setColor(QPalette::Active, QPalette::Window, scheme.surface);
    palette.setColor(QPalette::Active, QPalette::WindowText, scheme.onSurface);
    palette.setColor(QPalette::Active, QPalette::Base, scheme.surfaceContainerLowest);
    palette.setColor(QPalette::Active, QPalette::AlternateBase, scheme.surfaceContainerLow);
    palette.setColor(QPalette::Active, QPalette::ToolTipBase, scheme.surfaceContainerHigh);
    palette.setColor(QPalette::Active, QPalette::ToolTipText, scheme.onSurface);
    palette.setColor(QPalette::Active, QPalette::Text, scheme.onSurface);
    palette.setColor(QPalette::Active, QPalette::Button, scheme.surfaceContainer);
    palette.setColor(QPalette::Active, QPalette::ButtonText, scheme.onSurface);
    palette.setColor(QPalette::Active, QPalette::BrightText, scheme.primary);
    palette.setColor(QPalette::Active, QPalette::Highlight, scheme.primaryContainer);
    palette.setColor(QPalette::Active, QPalette::HighlightedText, scheme.onPrimaryContainer);
    palette.setColor(QPalette::Active, QPalette::Link, scheme.primary);
    palette.setColor(QPalette::Active, QPalette::LinkVisited, scheme.tertiary);
    palette.setColor(QPalette::Active, QPalette::Midlight, scheme.surfaceContainerHighest);
    palette.setColor(QPalette::Active, QPalette::Mid, scheme.outlineVariant);
    palette.setColor(QPalette::Active, QPalette::Dark, scheme.outline);
    palette.setColor(QPalette::Active, QPalette::Shadow, scheme.shadow);
    palette.setColor(QPalette::Active, QPalette::Accent, scheme.primary);

    // Inactive Group (same as Active for consistent MD3 tonal clarity)
    palette.setColor(QPalette::Inactive, QPalette::Window, scheme.surface);
    palette.setColor(QPalette::Inactive, QPalette::WindowText, scheme.onSurface);
    palette.setColor(QPalette::Inactive, QPalette::Base, scheme.surfaceContainerLowest);
    palette.setColor(QPalette::Inactive, QPalette::AlternateBase, scheme.surfaceContainerLow);
    palette.setColor(QPalette::Inactive, QPalette::ToolTipBase, scheme.surfaceContainerHigh);
    palette.setColor(QPalette::Inactive, QPalette::ToolTipText, scheme.onSurface);
    palette.setColor(QPalette::Inactive, QPalette::Text, scheme.onSurface);
    palette.setColor(QPalette::Inactive, QPalette::Button, scheme.surfaceContainer);
    palette.setColor(QPalette::Inactive, QPalette::ButtonText, scheme.onSurface);
    palette.setColor(QPalette::Inactive, QPalette::BrightText, scheme.primary);
    palette.setColor(QPalette::Inactive, QPalette::Highlight, scheme.surfaceContainerHighest);
    palette.setColor(QPalette::Inactive, QPalette::HighlightedText, scheme.onSurface);
    palette.setColor(QPalette::Inactive, QPalette::Link, scheme.primary);
    palette.setColor(QPalette::Inactive, QPalette::LinkVisited, scheme.tertiary);
    palette.setColor(QPalette::Inactive, QPalette::Midlight, scheme.surfaceContainerHighest);
    palette.setColor(QPalette::Inactive, QPalette::Mid, scheme.outlineVariant);
    palette.setColor(QPalette::Inactive, QPalette::Dark, scheme.outline);
    palette.setColor(QPalette::Inactive, QPalette::Shadow, scheme.shadow);
    palette.setColor(QPalette::Inactive, QPalette::Accent, scheme.primary);

    // Disabled Group (M3 38% opacity for text/icons, 12% for containers)
    const QColor disabledOnSurface = scheme.stateLayer(scheme.surface, scheme.onSurface, 0.38);
    const QColor disabledOutline = scheme.stateLayer(scheme.surface, scheme.outline, 0.12);

    palette.setColor(QPalette::Disabled, QPalette::Window, scheme.surface);
    palette.setColor(QPalette::Disabled, QPalette::WindowText, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::Base, scheme.surfaceContainerLowest);
    palette.setColor(QPalette::Disabled, QPalette::AlternateBase, scheme.surfaceContainerLow);
    palette.setColor(QPalette::Disabled, QPalette::ToolTipBase, scheme.surfaceContainerHigh);
    palette.setColor(QPalette::Disabled, QPalette::ToolTipText, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::Button, scheme.surfaceContainer);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::BrightText, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::Highlight, disabledOutline);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::Link, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::LinkVisited, disabledOnSurface);
    palette.setColor(QPalette::Disabled, QPalette::Midlight, scheme.surfaceContainer);
    palette.setColor(QPalette::Disabled, QPalette::Mid, disabledOutline);
    palette.setColor(QPalette::Disabled, QPalette::Dark, disabledOutline);
    palette.setColor(QPalette::Disabled, QPalette::Shadow, scheme.shadow);
    palette.setColor(QPalette::Disabled, QPalette::Accent, disabledOnSurface);

    return palette;
}

void M3ColorEngine::reloadColors()
{
    QElapsedTimer timer;
    timer.start();

    M3ColorScheme newScheme;
    const bool parsed = parseJsonFile(m_colorsFilePath, newScheme);
    if (parsed) {
        m_scheme = newScheme;
    } else {
        // Retain fallback dark or light scheme
        if (m_scheme.surface.isValid()) {
            // Keep existing
        } else {
            m_scheme = M3ColorScheme::defaultDark();
        }
    }

    m_lastIngestionDurationUs = timer.nsecsElapsed() / 1000;

    applyPaletteToApplication();
    Q_EMIT colorsChanged(m_scheme);
}

void M3ColorEngine::applyPaletteToApplication()
{
    if (!qApp) {
        return;
    }

    const QPalette pal = generateQPalette(m_scheme);
    QApplication::setPalette(pal);

    const auto topLevels = QApplication::topLevelWidgets();
    for (QWidget *w : topLevels) {
        if (w) {
            w->setPalette(pal);
            QEvent event(QEvent::ApplicationPaletteChange);
            QApplication::sendEvent(w, &event);
            w->update();
        }
    }
}
