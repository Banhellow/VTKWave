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
#include <vtkSmartPointer.h>
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

class ArrowInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
	int* inputAxis;
	static ArrowInteractorStyle* New();

	vtkTypeMacro(ArrowInteractorStyle, vtkInteractorStyleTrackballCamera);
	virtual void OnKeyPress() override
	{
		vtkRenderWindowInteractor* rwi = this->Interactor;
		std::string key = rwi->GetKeySym();
		if (key == "Up") {
			inputAxis[1] += 10;
		}
		if (key == "Down") {
			inputAxis[1] -= 10;
		}
		if (key == "Right") {
			inputAxis[0] -= 10;
		}
		if (key == "Left") {
			inputAxis[0] += 10;
		}

		vtkInteractorStyleTrackballCamera::OnKeyPress();
	}

	void InitiailizeAxis() {
		inputAxis = new int[2] {0, 0};
	}

	int GetPosX() const {
		return this->inputAxis[0];
	}

	int GetPosY() const {
		return this->inputAxis[1];
	}

};
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

vtkSmartPointer<vtkPNGReader> ReadHeightMap(std::string height_map){
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


int main(int argc, char* argv[])
{
	// Create a renderer
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
	vtkSmartPointer<vtkPNGReader> height_map = ReadHeightMap(R"(images\height_map.png)");
	camera = renderer->GetActiveCamera();
	camera->SetFocalPoint(256, 256, 0);
	camera->SetPosition(255.9, 0, -40);
	camera->SetRoll(180);

	camera->GetProjectionTransformMatrix(renderer)->Print(std::cout);
	vtkSmartPointer<ArrowInteractorStyle> arrowInteractor = vtkSmartPointer<ArrowInteractorStyle>::New();
	arrowInteractor->InitiailizeAxis();
	int* dims = height_map->GetOutput()->GetDimensions();
	//Voxel
	auto voxel = new VoxelMesh(dims[0], dims[1], 100);
	auto voxelData = new VoxelData(height_map->GetOutput(), arrowInteractor->inputAxis);
	voxel->UpdateData(voxelData);
	voxel->Initialize();
	renderer->AddVolume(voxel->volume);
	renderer->SetBackground(0.1, 0.2, 0.4);
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	auto skybox = CreateSkybox({R"(images\posx.jpg)", R"(images\negx.jpg)",
								R"(images\negy.jpg)", R"(images\posy.jpg)",
								R"(images\posz.jpg)", R"(images\negz.jpg)"});
	renderer->AddActor(skybox);

	// Create a render window interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderWindowInteractor->SetInteractorStyle(arrowInteractor);
	// Initialize the interactor and start the rendering loop
	renderWindow->Render();
	renderWindowInteractor->Initialize();

	UpdateData clientData;
	clientData.time = 0;
	clientData.arrowInteractor = arrowInteractor;
	clientData.mesh = voxel;

	vtkSmartPointer<vtkCallbackCommand> renderCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	renderCallback->SetCallback(Update);
	renderCallback->SetClientData(&clientData);
	renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, renderCallback);
	int timerId = renderWindowInteractor->CreateRepeatingTimer(0);
	renderWindowInteractor->Start();
	return 0;
}