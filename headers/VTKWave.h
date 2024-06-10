#ifndef VTKWAVE_H
#define VTKWAVE_H
#include <vtkSmartPointer.h>
#include<vtkGenericOpenGLRenderWindow.h>
#include<vtkRenderWindowInteractor.h>
#include<vtkPiecewiseFunction.h>
#include<vtkImageData.h>
class VTKWave {
private:
	vtkSmartPointer<vtkImageData> mriData;
	vtkSmartPointer<vtkImageData> ctData;
	vtkSmartPointer<vtkImageData> ctCacheData;
	vtkSmartPointer<vtkImageData> mriCacheData;
	vtkSmartPointer<vtkPiecewiseFunction> opacityFunction;
	int slicePosX= 0, slicePosY = 0;
	void UpdateSlice();
public:
	explicit VTKWave();
	void UpdateSlicePosX(int value);
	void UpdateSlicePosY(int value);
	void UpdateDensity(int value);
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> GetRenderWindow();
};
#endif // !WTKWAVE_H
