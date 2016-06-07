#include "mainwindow.h"

#include <QPixmap>
#include <QBitmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      timerId(),
      image(),
      imageLabel(new QLabel),
      videoLayout(new QVBoxLayout),
      controlLayout(new QHBoxLayout),
      buttonStart(new QPushButton),
      videoGroup(new QGroupBox),
      controlGroup(new QGroupBox),
      cameraList(new QComboBox),
      m_bitmap()
{
	setCentralWidget(controlGroup);

	imageLabel->setMinimumSize(320, 240);
	imageLabel->setBackgroundRole(QPalette::Base);

	videoLayout->addWidget(imageLabel);
	videoLayout->addWidget(buttonStart);
	videoGroup->setLayout(videoLayout);

	cameraList->setFixedWidth(150);
	controlLayout->addWidget(videoGroup);
	controlLayout->addWidget(cameraList);
	controlGroup->setLayout(controlLayout);

        timerId = startTimer(1000);
}

MainWindow::~MainWindow()
{
    delete m_bitmap;
    killTimer(timerId);
}

void MainWindow::bindBitmap(QBitmap* bitmap) {
    bitmap = new QBitmap;
    m_bitmap = bitmap;
}

void MainWindow::timerEvent(QTimerEvent *event) {
	const QPixmap* pixmap = reinterpret_cast<QPixmap*>(m_bitmap);
    imageLabel->setPixmap(*pixmap);
    imageLabel->repaint();
}
