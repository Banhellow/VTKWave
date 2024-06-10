#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <qmainwindow.h>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSmartPointer.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qslider.h>
#include <VTKWave.h>
#include <qlabel.h>

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();
private:
	QVTKOpenGLNativeWidget* vtkWidget;
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
	VTKWave wave;
private slots:
	void updateSlicePositionX(int value);
	void updateSlicePositionY(int value);
	void updateDensityValue(int value);
};

#endif // !
