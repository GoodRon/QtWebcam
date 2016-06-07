#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      image(),
      imageLabel(new QLabel),
      videoLayout(new QVBoxLayout),
      controlLayout(new QHBoxLayout),
      buttonStart(new QPushButton),
      videoGroup(new QGroupBox),
      controlGroup(new QGroupBox),
      cameraList(new QComboBox)
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
}

MainWindow::~MainWindow()
{

}
