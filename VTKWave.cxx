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

struct UpdateData {
	double time;
	std::chrono::system_clock::time_point startTime;
	std::vector<vtkSmartPointer<vtkActor>> actors;
};

vtkSmartPointer <vtkActor> CreateSphereActor(int posX, int posY, int posZ) {
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	sphereSource->SetCenter(0.0, 0.0, 0.0);
	sphereSource->SetRadius(1.0);
	sphereSource->SetThetaResolution(30);
	sphereSource->SetPhiResolution(30);

	transform->Translate(posX, posY, posZ);

	mapper->SetInputConnection(sphereSource->GetOutputPort());
	actor->SetMapper(mapper);
	double position[3] = { posX, posY, posZ };
	actor->SetPosition(position);
	//actor->SetUserTransform(transform);
	return actor;
}



void UpdateAppTime(double* time, std::chrono::system_clock::time_point start)
{
	auto end = std::chrono::system_clock::now();
	auto seconds = end - start;
	*time += 1/60.0;

}

void Update(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
	UpdateData* data = (UpdateData*)clientData;
	vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);
	int size = data->actors.size();
	double position[3];
	for (int i = 0; i < size; i++)
	{
		data->actors[i]->GetPosition(position);
		double zPos = cos(data->time + position[1] * 0.5);
		double newPos[3] = { position[0], position[1],  zPos};
		data->actors[i]->SetPosition(newPos);
	}
	std::cout << data->time << std::endl;
	UpdateAppTime(&data->time, data->startTime);
	interactor->GetRenderWindow()->Render();
}


int main(int argc, char* argv[])
{
	// Create a renderer
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	std::vector<vtkSmartPointer<vtkActor>> actors;
	//CreateSpheres
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			vtkSmartPointer<vtkActor> actor = CreateSphereActor(i * 2, j * 2, 0);
			actors.push_back(actor);
			renderer->AddActor(actor);
		}
	}

	// Create a render window
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	// Create a render window interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Initialize the interactor and start the rendering loop
	renderWindow->Render();
	renderWindowInteractor->Initialize();

	UpdateData clientData;
	clientData.actors = actors;
	clientData.startTime = std::chrono::system_clock::now();
	clientData.time = 0;

	vtkSmartPointer<vtkCallbackCommand> renderCallback = vtkSmartPointer<vtkCallbackCommand>::New();
	renderCallback->SetCallback(Update);
	renderCallback->SetClientData(&clientData);
	renderWindowInteractor->AddObserver(vtkCommand::TimerEvent, renderCallback);
	int timerId = renderWindowInteractor->CreateRepeatingTimer(0);
	renderWindowInteractor->Start();
	return 0;
}