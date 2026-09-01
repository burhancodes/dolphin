/*
 * SPDX-FileCopyrightText: 2026 KDE Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "m3colorengine.h"
#include "material3style.h"

#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>

class M3ColorEngineTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testDefaultPalettes();
    void testMatugenJsonIngestion();
    void testIngestionPerformanceBenchmark();
    void testStateLayerCalculations();
    void testHotReloadSignal();
    void testMaterial3StyleMetrics();
};

void M3ColorEngineTest::initTestCase()
{
    // Ensure instance is initialized
    QVERIFY(M3ColorEngine::instance() != nullptr);
}

void M3ColorEngineTest::testDefaultPalettes()
{
    const M3ColorScheme dark = M3ColorScheme::defaultDark();
    QVERIFY(dark.isDark);
    QVERIFY(dark.surface.isValid());
    QVERIFY(dark.primary.isValid());
    QVERIFY(dark.primaryContainer.isValid());
    QVERIFY(dark.secondaryContainer.isValid());
    QVERIFY(dark.surfaceContainerLowest.isValid());
    QVERIFY(dark.surfaceContainerLow.isValid());
    QVERIFY(dark.surfaceContainer.isValid());
    QVERIFY(dark.surfaceContainerHigh.isValid());
    QVERIFY(dark.surfaceContainerHighest.isValid());

    const M3ColorScheme light = M3ColorScheme::defaultLight();
    QVERIFY(!light.isDark);
    QVERIFY(light.surface.isValid());
    QVERIFY(light.primary.isValid());

    // Generate QPalette
    const QPalette palDark = M3ColorEngine::instance()->generateQPalette(dark);
    QCOMPARE(palDark.color(QPalette::Active, QPalette::Window), dark.surface);
    QCOMPARE(palDark.color(QPalette::Active, QPalette::Base), dark.surfaceContainerLowest);
    QCOMPARE(palDark.color(QPalette::Active, QPalette::Highlight), dark.primaryContainer);
    QCOMPARE(palDark.color(QPalette::Active, QPalette::HighlightedText), dark.onPrimaryContainer);
}

void M3ColorEngineTest::testMatugenJsonIngestion()
{
    QTemporaryFile tempJson;
    QVERIFY(tempJson.open());

    const QByteArray jsonContent = R"JSON({
  "colors": {
    "surface": { "dark": { "color": "#12141a" }, "light": { "color": "#fbfcfe" } },
    "on_surface": { "dark": { "color": "#e2e2e9" }, "light": { "color": "#191c20" } },
    "surface_container_lowest": { "dark": { "color": "#0d0f14" }, "light": { "color": "#ffffff" } },
    "surface_container_low": { "dark": { "color": "#1a1c22" }, "light": { "color": "#f4f3fb" } },
    "surface_container": { "dark": { "color": "#1e2026" }, "light": { "color": "#eeee16" } },
    "surface_container_high": { "dark": { "color": "#292a31" }, "light": { "color": "#e8e7f0" } },
    "surface_container_highest": { "dark": { "color": "#34353c" }, "light": { "color": "#e2e2eb" } },
    "primary": { "dark": { "color": "#adc6ff" }, "light": { "color": "#445e91" } },
    "on_primary": { "dark": { "color": "#002e6a" }, "light": { "color": "#ffffff" } },
    "primary_container": { "dark": { "color": "#004395" }, "light": { "color": "#d8e2ff" } },
    "on_primary_container": { "dark": { "color": "#d8e2ff" }, "light": { "color": "#001a42" } },
    "secondary_container": { "dark": { "color": "#3f4759" }, "light": { "color": "#dbe2f9" } },
    "on_secondary_container": { "dark": { "color": "#dbe2f9" }, "light": { "color": "#141b2c" } },
    "outline": { "dark": { "color": "#8e9099" }, "light": { "color": "#75777f" } },
    "outline_variant": { "dark": { "color": "#44474f" }, "light": { "color": "#c4c6d0" } }
  },
  "is_dark_mode": true,
  "mode": "dark"
})JSON";

    tempJson.write(jsonContent);
    tempJson.flush();
    tempJson.close();

    M3ColorEngine *engine = M3ColorEngine::instance();
    engine->setCustomColorsFilePath(tempJson.fileName());

    const M3ColorScheme &scheme = engine->scheme();
    QCOMPARE(scheme.surface, QColor(QStringLiteral("#12141a")));
    QCOMPARE(scheme.surfaceContainerLowest, QColor(QStringLiteral("#0d0f14")));
    QCOMPARE(scheme.surfaceContainerLow, QColor(QStringLiteral("#1a1c22")));
    QCOMPARE(scheme.primary, QColor(QStringLiteral("#adc6ff")));
    QCOMPARE(scheme.primaryContainer, QColor(QStringLiteral("#004395")));
    QCOMPARE(scheme.secondaryContainer, QColor(QStringLiteral("#3f4759")));
}

void M3ColorEngineTest::testIngestionPerformanceBenchmark()
{
    QTemporaryFile tempJson;
    QVERIFY(tempJson.open());

    const QByteArray jsonContent = R"JSON({
  "colors": {
    "surface": { "default": { "color": "#111318" } },
    "on_surface": { "default": { "color": "#e2e2e9" } },
    "surface_container_lowest": { "default": { "color": "#0c0e13" } },
    "surface_container_low": { "default": { "color": "#191b20" } },
    "surface_container": { "default": { "color": "#1d2024" } },
    "surface_container_high": { "default": { "color": "#282a2f" } },
    "surface_container_highest": { "default": { "color": "#33353a" } },
    "primary": { "default": { "color": "#adc6ff" } },
    "primary_container": { "default": { "color": "#2573e6" } },
    "secondary_container": { "default": { "color": "#3f4759" } },
    "outline": { "default": { "color": "#8e9099" } },
    "outline_variant": { "default": { "color": "#44474f" } }
  },
  "is_dark_mode": true
})JSON";
    tempJson.write(jsonContent);
    tempJson.flush();
    tempJson.close();

    M3ColorEngine *engine = M3ColorEngine::instance();
    engine->setCustomColorsFilePath(tempJson.fileName());

    QElapsedTimer timer;
    timer.start();

    const int iterations = 100;
    for (int i = 0; i < iterations; ++i) {
        engine->reloadColors();
    }

    const qint64 totalElapsedUs = timer.nsecsElapsed() / 1000;
    const double avgUs = static_cast<double>(totalElapsedUs) / iterations;
    const double avgMs = avgUs / 1000.0;

    qInfo() << "M3ColorEngine Ingestion Benchmark: Avg duration =" << avgMs << "ms (" << avgUs << "us ) over" << iterations << "runs";

    // Strictly verify ingestion completes under 16ms frame budget (Checkpoint 1)
    QVERIFY2(avgMs < 16.0, "Ingestion time must be well within 16ms frame time!");
}

void M3ColorEngineTest::testStateLayerCalculations()
{
    const M3ColorScheme scheme = M3ColorScheme::defaultDark();
    const QColor base(0x00, 0x00, 0x00);
    const QColor content(0xff, 0xff, 0xff);

    // 8% hover
    const QColor hover = scheme.stateLayer(base, content, 0.08);
    QVERIFY(hover.red() >= 19 && hover.red() <= 21); // ~20 out of 255 (8%)

    // 12% pressed
    const QColor pressed = scheme.stateLayer(base, content, 0.12);
    QVERIFY(pressed.red() >= 29 && pressed.red() <= 32); // ~30.6 out of 255 (12%)
}

void M3ColorEngineTest::testHotReloadSignal()
{
    QTemporaryFile tempJson;
    QVERIFY(tempJson.open());
    tempJson.write(R"JSON({"is_dark_mode": true, "colors": {"surface": {"default": {"color": "#112233"}}}})JSON");
    tempJson.flush();
    tempJson.close();

    M3ColorEngine *engine = M3ColorEngine::instance();
    engine->setCustomColorsFilePath(tempJson.fileName());

    QSignalSpy spy(engine, &M3ColorEngine::colorsChanged);
    engine->reloadColors();

    QCOMPARE(spy.count(), 1);
    const M3ColorScheme emittedScheme = spy.takeFirst().at(0).value<M3ColorScheme>();
    QCOMPARE(emittedScheme.surface, QColor(QStringLiteral("#112233")));
}

void M3ColorEngineTest::testMaterial3StyleMetrics()
{
    Material3Style style;

    QCOMPARE(style.pixelMetric(QStyle::PM_ButtonMargin), 12);
    QCOMPARE(style.pixelMetric(QStyle::PM_ScrollBarExtent), 10);
    QCOMPARE(style.pixelMetric(QStyle::PM_MenuHMargin), 8);
    QCOMPARE(style.pixelMetric(QStyle::PM_MenuVMargin), 8);
    QCOMPARE(style.pixelMetric(QStyle::PM_TabBarTabHSpace), 16);
    QCOMPARE(style.pixelMetric(QStyle::PM_TabBarTabVSpace), 8);
    QCOMPARE(style.pixelMetric(QStyle::PM_DefaultFrameWidth), 1);
    QCOMPARE(style.pixelMetric(QStyle::PM_LayoutHorizontalSpacing), 8);
    QCOMPARE(style.pixelMetric(QStyle::PM_LayoutVerticalSpacing), 8);
}

QTEST_MAIN(M3ColorEngineTest)

#include "m3colorenginetest.moc"
