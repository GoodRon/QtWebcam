#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QListWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
	QImage image;
	QLabel* imageLabel;
	QVBoxLayout* videoLayout;
	QHBoxLayout* controlLayout;
	QPushButton* buttonStart;
	QGroupBox* videoGroup;
	QGroupBox* controlGroup;
	QListWidget* cameraList;
};

#endif // MAINWINDOW_H
