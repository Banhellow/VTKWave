#include "MainWindow.h"
#include "VTKWave.h"
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QVBoxLayout* sliderLayout = new QVBoxLayout;
	QWidget* sliderWidget = new QWidget(this);
	QSlider* horizontalSlider = new QSlider(Qt::Horizontal, this);
	QLabel* horizontalLabel = new QLabel("Slice X");
	QLabel* verticalLabel = new QLabel("Slice Y");
	QLabel* ctLabel = new QLabel("CT Density");
	horizontalSlider->setRange(0, 256);
	horizontalSlider->setValue(0);
	connect(horizontalSlider,&QSlider::valueChanged, this, &MainWindow::updateSlicePositionX);
	QSlider* verticalSlider = new QSlider(Qt::Horizontal, this);
	verticalSlider->setRange(0, 256);
	verticalSlider->setValue(0);
	connect(verticalSlider, &QSlider::valueChanged, this, &MainWindow::updateSlicePositionY);
	QSlider* densitySlider = new QSlider(Qt::Horizontal, this);
	densitySlider->setRange(1, 5256);
	densitySlider->setValue(1);
	connect(densitySlider, &QSlider::valueChanged, this, &MainWindow::updateDensityValue);
	sliderLayout->addWidget(horizontalLabel);
	sliderLayout->addWidget(horizontalSlider);
	sliderLayout->addWidget(verticalLabel);
	sliderLayout->addWidget(verticalSlider);
	sliderLayout->addWidget(ctLabel);
	sliderLayout->addWidget(densitySlider);

	sliderWidget->setLayout(sliderLayout);

	vtkWidget = new QVTKOpenGLNativeWidget(this);
	setCentralWidget(vtkWidget);

	wave = VTKWave();
	renderWindow = wave.GetRenderWindow();

	vtkWidget->setRenderWindow(renderWindow);

	mainLayout->addWidget(vtkWidget);
	mainLayout->addWidget(sliderWidget);
	mainLayout->setStretch(0, 3);
	mainLayout->setStretch(1, 1);
	QWidget* centralWidget = new QWidget(this);
	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
}


void MainWindow::updateSlicePositionX(int value)
{
	wave.UpdateSlicePosX(value);
	vtkWidget->renderWindow()->Render();
}


void MainWindow::updateSlicePositionY(int value)
{
	wave.UpdateSlicePosY(value);
	vtkWidget->renderWindow()->Render();
}

void MainWindow::updateDensityValue(int value)
{
	wave.UpdateDensity(value);
	vtkWidget->renderWindow()->Render();
}
