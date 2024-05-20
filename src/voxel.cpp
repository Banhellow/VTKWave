#include "voxel.h"

VoxelMesh::VoxelMesh(int dimX, int dimY, int dimZ)
{
	this->dimX = dimX;
	this->dimY = dimY;
	this->dimZ = dimZ;
}

void VoxelMesh::Initialize()
{
	imageData = vtkSmartPointer<vtkImageData>::New();

	SetupOpacityFunction();
	GenerateMesh();
	SetupVolume();
}

void VoxelMesh::Tick(float deltaTime)
{
	auto points = imageData->GetNumberOfPoints();
	auto  pixel_points = data->texture->GetPointData()->GetScalars();
	int* dims = data->texture->GetDimensions();
	for (vtkIdType i = 0; i < points; i++)
	{
		double pos[3];
		imageData->GetPoint(i, pos);
		double* zPos = pixel_points->GetTuple((pos[0] + pos[1] * dims[0]));
		int texPosX = MathUtils::Modulo(data->inputAxis[0] + pos[0], 512);
		auto voxelValue = static_cast<unsigned char*>(imageData->GetScalarPointer(pos[0], pos[1], pos[2]));
		double voxelHeight = pos[2] / dimZ;
		double textureValue = zPos[0] / 255.0;
		if (voxelHeight > textureValue){
			voxelValue[0] = 0;
		}
		else {
			voxelValue[0] = 255;
		}
	}

}

void VoxelMesh::UpdateData(VoxelData* data)
{
	this->data = data;
}

void VoxelMesh::GenerateMesh()
{
	const int dims[3] = { dimX, dimY, dimZ };
	imageData->SetDimensions(dims);
	imageData->SetOrigin(0.0, 0.0, 0.0);
	imageData->SetSpacing(1.0, 1.0, 1.0);
	imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	Tick(0);
}

void VoxelMesh::SetupVolume() {
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);  // Blue for low values
	colorTransferFunction->AddRGBPoint(255.0, 1.0, 1.0, 1.0);

	auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetScalarOpacity(opacityFunction);
	volumeProperty->ShadeOn();
	volumeProperty->SetColor(colorTransferFunction);
	volumeProperty->SetInterpolationTypeToLinear();



	mapper = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
	mapper->SetInputData(imageData);
	volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(mapper);
	volume->SetProperty(volumeProperty);
}

void VoxelMesh::SetupOpacityFunction()
{
	opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	opacityFunction->AddPoint(0.0, 0.0);
	opacityFunction->AddPoint(50.0, 0.2);  // Semi-transparent
	opacityFunction->AddPoint(100.0, 0.4); // Less transparent
	opacityFunction->AddPoint(150.0, 0.6); // Less transparent
	opacityFunction->AddPoint(200.0, 0.8);
	opacityFunction->AddPoint(255.0, 1.0);
}
