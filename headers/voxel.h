#include<vtkActor.h>
#include<vtkPolyDataMapper.h>
#include<vtkSmartPointer.h>
#include<vtkRenderer.h>
#include<vtkCell.h>
#include<vtkTriangle.h>
#include<vtkPoints.h>
#include<GenericMesh.h>
#include<vtkDataSetMapper.h>
#include<VoxelData.h>
#include<vtkImageData.h>
#include<vtkPointData.h>
#include<vtkPiecewiseFunction.h>
#include<vtkVolumeProperty.h>
#include<vtkFixedPointVolumeRayCastMapper.h>
#include<vtkColorTransferFunction.h>
#include<vtkVolume.h>
#include<MathUtils.h>

class VoxelMesh : public GenericMesh<VoxelData> {
	int dimX, dimY, dimZ;
	void SetupVolume();
	void SetupOpacityFunction();
	vtkSmartPointer<vtkPiecewiseFunction> opacityFunction;
public:
	VoxelMesh(int dimX, int dimY, int dimZ);
	vtkSmartPointer<vtkImageData> imageData;
	vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> mapper;
	vtkSmartPointer<vtkVolume> volume;
	void Initialize() override;
	void Tick(float deltaTime) override;
	void UpdateData(VoxelData* data) override;

protected:
	void GenerateMesh();
};