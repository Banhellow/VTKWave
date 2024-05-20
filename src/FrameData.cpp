#include "FrameData.h"


FrameData::FrameData(vtkSmartPointer<vtkImageData> texture, int inputAxis[2])
{
	this->texture = texture;
	this->inputAxis = inputAxis;
}
