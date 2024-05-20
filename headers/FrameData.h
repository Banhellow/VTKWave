#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#ifndef framedata_h
#define framedata_h
class FrameData {

public:
	vtkSmartPointer<vtkImageData> texture;
	int* inputAxis;
	FrameData(vtkSmartPointer<vtkImageData> texture, int inputAxis[2]);
};
#endif // !framedata_h

