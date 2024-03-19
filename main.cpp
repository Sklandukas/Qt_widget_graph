#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QFile>
#include <QTextStream>
#include <QComboBox>
#include <QtCharts/QValueAxis>
#include <QLabel>

QT_USE_NAMESPACE

    int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QMainWindow w;
    QWidget *centralWidget = new QWidget(&w);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QPushButton *button = new QPushButton("Pasirinkti CSV failą");
    layout->addWidget(button);

    QLabel *dataLabel = new QLabel("");
    layout->addWidget(dataLabel);

    QChartView *chartView = new QChartView();
    layout->addWidget(chartView);

    QObject::connect(button, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getOpenFileName(&w, "Pasirinkite CSV failą", QDir::currentPath(), "CSV failai (*.csv)");
        if (fileName.isEmpty())
            return;

        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
            return;

        QTextStream in(&file);
        QVector<double> xValues;
        QVector<QLineSeries*> seriesList;
        QStringList header;

        if (!in.atEnd()) {
            QString line = in.readLine();
            header = line.split(",");
        }

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 2) {
                xValues.append(fields[0].toDouble());
                for (int i = 1; i < fields.size(); ++i) {
                    if (seriesList.size() < fields.size() - 1) {
                        QLineSeries *series = new QLineSeries();
                        seriesList.append(series);
                    }
                    seriesList[i - 1]->append(xValues.last(), fields[i].toDouble());
                }
            }
        }
        file.close();

        dataLabel->setText("");

        QChart *chart = new QChart();
        chart->setTitle("Diagrama");

        QValueAxis *axisX = new QValueAxis;
        axisX->setTickCount(5);
        chart->addAxis(axisX, Qt::AlignBottom);

        QValueAxis *axisY = new QValueAxis;
        chart->addAxis(axisY, Qt::AlignLeft);

        for (int i = 0; i < seriesList.size(); ++i) {
            chart->addSeries(seriesList[i]);
            seriesList[i]->attachAxis(axisX);
            seriesList[i]->attachAxis(axisY);
        }
        chartView->setChart(chart);
    });

    w.setCentralWidget(centralWidget);
    w.resize(420, 300);
    w.show();

    return a.exec();
}
