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
struct UpdateData {
	double time;
	std::chrono::system_clock::time_point startTime;
	vtkSmartPointer<vtkPolyData> plane_data;
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
	*time += 1 / 60.0;

}

void Update(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
	UpdateData* data = (UpdateData*)clientData;
	vtkRenderWindowInteractor* interactor = static_cast<vtkRenderWindowInteractor*>(caller);
	vtkSmartPointer<vtkPoints> points = data->plane_data->GetPoints();
	int size = points->GetNumberOfPoints();
	double position[3];
	for (int i = 0; i < size; i++)
	{
		double* point = points->GetPoint(i);
		double zPos = cos(data->time + point[1]);
		points->SetPoint(i, point[0], point[1], zPos);
	}
	points->Modified();
	std::cout << data->time << std::endl;
	UpdateAppTime(&data->time, data->startTime);
	interactor->GetRenderWindow()->Render();
}

vtkSmartPointer<vtkPolyData> GenerateMesh(int width, int height, int res_width, int res_height) {
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

	double deltaX = width / (double)res_width;
	double deltaY = height / (double)res_height;
	for (int j = 0; j < res_height; j++)
	{
		for (int i = 0; i < res_width; i++)
		{
			points->InsertNextPoint(i * deltaX, j * deltaY, 0);
		}
	}

	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

	for (int h = 0; h < res_height - 1; h++)
	{
		for (int w = 0; w < res_width - 1; w++)
		{
			vtkSmartPointer<vtkTriangle> triangle_left = vtkSmartPointer<vtkTriangle>::New();
			triangle_left->GetPointIds()->SetId(0, h * res_width + w);
			triangle_left->GetPointIds()->SetId(1, h * res_width + w + 1);
			triangle_left->GetPointIds()->SetId(2, (h + 1) * res_width + w + 1);
			cells->InsertNextCell(triangle_left);

			vtkSmartPointer<vtkTriangle> triangle_right = vtkSmartPointer<vtkTriangle>::New();
			triangle_right->GetPointIds()->SetId(0, h * res_width + w);
			triangle_right->GetPointIds()->SetId(1, (h + 1) * res_width + w + 1);
			triangle_right->GetPointIds()->SetId(2, (h + 1) * res_width + w);
			cells->InsertNextCell(triangle_right);
		}
	}

	vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
	poly_data->SetPoints(points);
	poly_data->SetPolys(cells);
	return poly_data;
}

vtkSmartPointer<vtkActor> GetCustomMeshActor(vtkSmartPointer<vtkPolyData> poly_data) {
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(poly_data);
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	return actor;
}


int main(int argc, char* argv[])
{
	// Create a renderer
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	// Create a render window
	vtkSmartPointer<vtkPolyData> poly_data = GenerateMesh(5, 5, 100, 100);
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(poly_data);
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	renderer->AddActor(actor);
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	// Create a render window interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	// Initialize the interactor and start the rendering loop
	renderWindow->Render();
	renderWindowInteractor->Initialize();

	UpdateData clientData;
	clientData.plane_data = poly_data;
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