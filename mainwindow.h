#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void bindBitmap(QBitmap* bitmap);

protected:
    void timerEvent(QTimerEvent *event);

private:
    int timerId;

	QImage image;
	QLabel* imageLabel;
	QVBoxLayout* videoLayout;
	QHBoxLayout* controlLayout;
	QPushButton* buttonStart;
	QGroupBox* videoGroup;
	QGroupBox* controlGroup;
	QComboBox* cameraList;
	QBitmap* m_bitmap;
};

#endif // MAINWINDOW_H
