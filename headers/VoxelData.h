#include<FrameData.h>
#ifndef VOXEL_DATA_H
#define VOXEL_DATA_H
class VoxelData : public FrameData {
public:
	VoxelData(vtkSmartPointer<vtkImageData> texture, int input[2]);
};
#endif // !1

