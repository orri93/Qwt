#include <qspinbox.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qevent.h>
#include <qdatetime.h>
#include <qwt_plot_canvas.h>
#include "plot.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    initToolBar();
    initStatusBar();

    d_plot = new Plot(this);
    setCentralWidget(d_plot);
    d_plot->canvas()->installEventFilter(this);

    connect(d_timerInterval, SIGNAL(valueChanged(int)),
        d_plot, SLOT(setTimerInterval(int)) );
    connect(d_numPoints, SIGNAL(valueChanged(int)),
        d_plot, SLOT(setNumPoints(int)) );

    d_timerInterval->setValue(20);
    d_numPoints->setValue(1000);
}

void MainWindow::initToolBar()
{
    QToolBar *toolBar = new QToolBar(this);

#if QT_VERSION < 0x040000
    setDockEnabled(TornOff, true);
    setRightJustification(true);
#else
    toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
#endif
    QWidget *hBox = new QWidget(toolBar);

    d_timerInterval = new QSpinBox(hBox);
    d_timerInterval->setRange(0, 1000);
    d_timerInterval->setSingleStep(1);

    d_numPoints = new QSpinBox(hBox);
    d_numPoints->setRange(10, 1000000.0);
    d_numPoints->setSingleStep(100);

    QHBoxLayout *layout = new QHBoxLayout(hBox);
    layout->addWidget(new QLabel("Update Interval", hBox));
    layout->addWidget(d_timerInterval);
    layout->addSpacing(20);
    layout->addWidget(new QLabel("Points", hBox));
    layout->addWidget(d_numPoints);
    layout->addWidget(new QWidget(hBox), 10); // spacer);

#if QT_VERSION >= 0x040000
    toolBar->addWidget(hBox);
#endif
    addToolBar(toolBar);
}

void MainWindow::initStatusBar()
{
    d_frameCount = new QLabel();
    statusBar()->addWidget(d_frameCount, 10);
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if ( object == d_plot->canvas() && event->type() == QEvent::Paint )
    {
        static int counter;
        static QTime timeStamp;

        if ( !timeStamp.isValid() )
        {
            timeStamp.start();
            counter = 0;
        }
        else
        {
            counter++;

            const double elapsed = timeStamp.elapsed() / 1000.0;
            if ( elapsed >= 1 )
            {
                QString fps;
                fps.setNum(qRound(counter / elapsed));
                fps += " Fps";

                d_frameCount->setText(fps);

                counter = 0;
                timeStamp.start();
            }
        }
    }

    return QMainWindow::eventFilter(object, event);
};
