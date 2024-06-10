#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <vtkSphereSource.h>
#include <vtkCallbackCommand.h>
#include <vtkTransform.h>
#include <math.h>
#include <time.h>
#include <thread>
#include <vtkTriangle.h>
#include <vtkPoints.h>
#include <vtkCell.h>
#include <QPushButton>
#include <vtkLookupTable.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPngReader.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkDataSetMapper.h>
#include <voxel.h>
#include <VoxelData.h>
#include <vtkTexture.h>
#include <vtkSkybox.h>
#include<vtkJPEGReader.h>
#include <qfile.h>
#include <vtkUnsignedShortArray.h>
#include <vtkSmartVolumeMapper.h>
#include<fstream>
#include<vtkPiecewiseFunction.h>
#include<vtkColorTransferFunction.h>
#include<vtkVolumeProperty.h>
#include<ArrowInteractorStyle.h>
#include "VTKWave.h"
#include "omp.h"
vtkStandardNewMacro(ArrowInteractorStyle);

struct UpdateData {
	double time;
	std::chrono::system_clock::time_point startTime;
	VoxelMesh* mesh;
	vtkSmartPointer<ArrowInteractorStyle> arrowInteractor;
};


void UpdateAppTime(double* time, std::chrono::system_clock::time_point start)
{
	auto end = std::chrono::system_clock::now();
	auto seconds = end - start;
	*time += 1 / 60.0;

}

std::vector<unsigned short>  ReadDataFromFile(std::string path) {
	std::ifstream readStream(path, std::ios::binary);

	if (!readStream) {
		std::cout << "Failed to open file: " << std::endl;
	}

	readStream.seekg(0, std::ios::end);
	std::streampos fileSize = readStream.tellg();
	readStream.seekg(0, std::ios::beg);
	std::vector<unsigned short> fileData(fileSize / sizeof(unsigned short));
	readStream.read(reinterpret_cast<char*>(fileData.data()), fileSize);
	readStream.close();
	auto max_it = std::max_element(fileData.begin(), fileData.end());
	std::cout << path << ": " << *max_it << std::endl;
	if (512 * 512 * 247 != fileData.size()) {
		std::cerr << "Volume dimensions do not match the file size" << std::endl;
	}

	return fileData;
}

vtkSmartPointer<vtkLookupTable> CreateLookupTable(float hueMin, float hueMax, float satMin, float satMax, float valMin, float valMax)
{
	vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
	lookupTable->SetRange(0, 1);
	lookupTable->SetHueRange(hueMin, hueMax);
	lookupTable->SetSaturationRange(satMin, satMax);
	lookupTable->SetValueRange(valMin, valMax);
	lookupTable->Build();
	return lookupTable;
}

void Update(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
	UpdateData* data = (UpdateData*)clientData;
	vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);
	data->mesh->Tick(0);
	interactor->GetRenderWindow()->Render();
}

vtkSmartPointer<vtkPNGReader> ReadHeightMap(std::string height_map) {
	vtkSmartPointer<vtkPNGReader> reader = vtkSmartPointer<vtkPNGReader>::New();
	reader->SetFileName(height_map.c_str());
	if (reader->CanReadFile(height_map.c_str()) == false) {
		std::cout << "Can't read this file" << std::endl;
		return NULL;
	}

	reader->Update();
	return reader;
}

vtkSmartPointer<vtkSkybox> CreateSkybox(std::vector<std::string> fileNames) {
	vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
	texture->CubeMapOn();
	int i = 0;
	for (auto const& fn : fileNames) {
		vtkSmartPointer<vtkJPEGReader> jpegReader = vtkSmartPointer<vtkJPEGReader>::New();
		jpegReader->SetFileName(fn.c_str());
		jpegReader->Update();
		texture->SetInputConnection(i, jpegReader->GetOutputPort());
		i++;
	}

	vtkSmartPointer<vtkSkybox> skybox = vtkSmartPointer<vtkSkybox>::New();
	skybox->SetTexture(texture);
	return skybox;
}


VTKWave::VTKWave()
{

}

void VTKWave::UpdateSlicePosX(int value)
{
	slicePosX = value;
	UpdateSlice();
}

void VTKWave::UpdateSlicePosY(int value)
{
	slicePosY = value;
	UpdateSlice();
}

void VTKWave::UpdateDensity(int value)
{
	opacityFunction->RemoveAllPoints();
	opacityFunction->AddPoint(value, 0.0);
	opacityFunction->AddPoint(5257, 1.0);
}

void VTKWave::UpdateSlice() {
	auto points = ctData->GetNumberOfPoints();
	auto ctValue = static_cast<unsigned short*>(ctData->GetScalarPointer());
	auto ctCachedValue = static_cast<unsigned short*>(ctCacheData->GetScalarPointer());
	auto mrValue = static_cast<unsigned short*>(mriData->GetScalarPointer());
	auto mrCachedValue = static_cast<unsigned short*>(mriCacheData->GetScalarPointer());
	#pragma omp parallel for
	for (vtkIdType i = 0; i < points; i++) {
		double pos[3];
		ctData->GetPoint(i, pos);
		if (pos[0] < slicePosX && pos[1] < slicePosY) {
			ctValue[i] = 0;
			mrValue[i] = 0;
		}
		else {
			ctValue[i] = ctCachedValue[i];
			mrValue[i] = mrCachedValue[i];
		}
	}
}

vtkSmartPointer<vtkGenericOpenGLRenderWindow> VTKWave::GetRenderWindow()
{
	// Create a renderer
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkPNGReader> height_map = ReadHeightMap(R"(images\height_map.png)");

	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	colorTransferFunction->AddRGBPoint(5257, 1.0, 1.0, 1.0);

	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction1 = vtkSmartPointer<vtkColorTransferFunction>::New();
	colorTransferFunction1->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	colorTransferFunction1->AddRGBPoint(597, 1.0, 1.0, 1.0);

	opacityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	opacityFunction->AddPoint(0, 0.0);
	opacityFunction->AddPoint(5257, 1.0);

	vtkSmartPointer<vtkPiecewiseFunction> densityFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	densityFunction->AddPoint(0.0, 0.0);
	densityFunction->AddPoint(597, 1.0);

	auto CTVolumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	CTVolumeProperty->SetScalarOpacity(opacityFunction);
	//volumeProperty->ShadeOn();
	CTVolumeProperty->SetColor(colorTransferFunction);
	CTVolumeProperty->SetInterpolationTypeToLinear();

	auto RTVolumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	RTVolumeProperty->SetScalarOpacity(densityFunction);
	RTVolumeProperty->SetColor(colorTransferFunction1);
	RTVolumeProperty->SetInterpolationTypeToLinear();

	auto data = ReadDataFromFile(R"(images\CT.rdata)");

	ctData = vtkSmartPointer<vtkImageData>::New();
	ctCacheData = vtkSmartPointer<vtkImageData>::New();
	ctData->SetDimensions(512, 512, 247);
	ctData->SetSpacing(0.447266, 0.447266, 0.625);
	ctData->SetOrigin(0.0, 0.0, 0.0);
	ctData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
	unsigned short* pointer = static_cast<unsigned short*>(ctData->GetScalarPointer());
	std::copy(data.begin(), data.end(), pointer);
	ctCacheData->DeepCopy(ctData);
	auto mapper1 = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
	mapper1->SetInputData(ctData);
	auto volume1 = vtkSmartPointer<vtkVolume>::New();
	volume1->SetMapper(mapper1);
	volume1->SetProperty(CTVolumeProperty);

	auto data2 = ReadDataFromFile(R"(images\MR_TO_CT.rdata)");

	mriData = vtkSmartPointer<vtkImageData>::New();
	mriCacheData = vtkSmartPointer<vtkImageData>::New();
	mriData->SetDimensions(512, 512, 247);
	mriData->SetSpacing(0.447266, 0.447266, 0.625);
	mriData->SetOrigin(0.0, 0.0, 0.0);
	mriData->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
	unsigned short* pointer1 = static_cast<unsigned short*>(mriData->GetScalarPointer());
	std::copy(data2.begin(), data2.end(), pointer1);
	mriCacheData->DeepCopy(mriData);
	auto mapper2 = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
	mapper2->SetInputData(mriData);
	auto volume2 = vtkSmartPointer<vtkVolume>::New();
	volume2->SetMapper(mapper2);
	volume2->SetProperty(RTVolumeProperty);
	UpdateSlice();

	camera->GetProjectionTransformMatrix(renderer)->Print(std::cout);
	vtkSmartPointer<ArrowInteractorStyle> arrowInteractor = vtkSmartPointer<ArrowInteractorStyle>::New();
	arrowInteractor->InitiailizeAxis();
	int* dims = height_map->GetOutput()->GetDimensions();
	//Voxel
	auto voxel = new VoxelMesh(dims[0], dims[1], 100);
	auto voxelData = new VoxelData(height_map->GetOutput(), arrowInteractor->inputAxis);
	voxel->UpdateData(voxelData);
	voxel->Initialize();
	renderer->AddVolume(volume1);
	renderer->AddVolume(volume2);
	renderer->SetBackground(0.1, 0.2, 0.4);
	vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	return renderWindow;
}
