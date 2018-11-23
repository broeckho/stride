/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#include "BoxplotGenerator.h"

#include <QtCharts/QBoxPlotSeries>
#include <QtCharts/QBoxSet>
#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

#include <algorithm>
#include <iostream>

QT_CHARTS_USE_NAMESPACE

namespace calibration {

/*
 * Inspired by https://doc.qt.io/qt-5.10/qtcharts-boxplotchart-example.html
 * */

BoxplotGenerator::BoxplotGenerator()
    : logger(stride::util::LogUtils::CreateCliLogger("boxplot_generator_logger", "boxplot_generator_log.txt"))
{
}

void BoxplotGenerator::WriteToFile(std::vector<BoxplotData> data) const { GeneratePlots(data, true); }

void BoxplotGenerator::Display(std::vector<BoxplotData> data) const { GeneratePlots(data, false); }

void BoxplotGenerator::GeneratePlots(std::vector<BoxplotData> data, bool write) const
{
        for (const auto& boxplotData : data) {
                int          argc = 0;
                QApplication a(argc, nullptr);
                auto*        boxplot = new QBoxPlotSeries();
                auto*        box     = new QBoxSet(boxplotData.name.c_str());

                box->setValue(QBoxSet::LowerExtreme, boxplotData.min_value);
                box->setValue(QBoxSet::UpperExtreme, boxplotData.max_value);
                box->setValue(QBoxSet::Median, boxplotData.median);
                box->setValue(QBoxSet::LowerQuartile, boxplotData.lower_quartile);
                box->setValue(QBoxSet::UpperQuartile, boxplotData.upper_quartile);
                boxplot->append(box);

                QChart* chart = new QChart();
                chart->addSeries(boxplot);
                chart->setTitle("BoxplotGenerator of results");
                chart->createDefaultAxes();
                chart->axisY()->setMin(boxplotData.min_value * 0.99);
                chart->axisY()->setMax(boxplotData.max_value * 1.01);
                chart->legend()->setVisible(true);
                chart->legend()->setAlignment(Qt::AlignBottom);

                auto* chartView = new QChartView(chart);
                chartView->setRenderHint(QPainter::Antialiasing);

                QMainWindow window;
                window.setCentralWidget(chartView);
                if (write) {
                        window.grab().save((boxplotData.name + ".png").c_str());
                } else {
                        window.resize(800, 600);
                        window.show();
                        QApplication::exec();
                }
        }
}

} // namespace calibration
