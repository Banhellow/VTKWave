#include "planeMesh.h"

PlaneMesh::PlaneMesh(int x, int y)
{
}

void PlaneMesh::Initialize()
{
}

void PlaneMesh::Tick(float deltaTime)
{
	auto points = meshData->GetPoints();
	int size = points->GetNumberOfPoints();
	auto pixel_points = data->texture->GetPointData()->GetScalars();
	int* dims = data->texture->GetDimensions();
	auto scalars = vtkSmartPointer<vtkFloatArray>::New();
	double position[3];
	for (int i = 0; i < size; i++)
	{
		double* point = points->GetPoint(i);
		int indexY = MathUtils::Modulo(data->inputAxis[1] + (int)point[1], dims[1]);
		int indexX = MathUtils::Modulo(data->inputAxis[0] + (int)point[0], dims[0]);
		double* zPos = pixel_points->GetTuple((indexX + indexY * dims[0]));
		points->SetPoint(i, point[0], point[1], (zPos[0] / 255.0) * 100);
		scalars->InsertNextTuple1(zPos[0] / 255.0);
	}
}

void PlaneMesh::UpdateData(FrameData* data)
{
	this->data = data;
}

void PlaneMesh::SetupActor() {
	mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(meshData);
	actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
}

void PlaneMesh::GenerateMesh()
{
	auto points = vtkSmartPointer<vtkPoints>::New();
	for (int j = 0; j < res_height; j++)
	{
		for (int i = 0; i < res_width; i++)
		{
			points->InsertNextPoint(i, j, 0);
		}
	}

	auto cells = vtkSmartPointer<vtkCellArray>::New();

	for (int h = 0; h < res_height - 1; h++)
	{
		for (int w = 0; w < res_width - 1; w++)
		{
			auto triangle_left = vtkSmartPointer<vtkTriangle>::New();
			triangle_left->GetPointIds()->SetId(0, h * res_width + w);
			triangle_left->GetPointIds()->SetId(1, h * res_width + w + 1);
			triangle_left->GetPointIds()->SetId(2, (h + 1) * res_width + w + 1);
			cells->InsertNextCell(triangle_left);

			auto triangle_right = vtkSmartPointer<vtkTriangle>::New();
			triangle_right->GetPointIds()->SetId(0, h * res_width + w);
			triangle_right->GetPointIds()->SetId(1, (h + 1) * res_width + w + 1);
			triangle_right->GetPointIds()->SetId(2, (h + 1) * res_width + w);
			cells->InsertNextCell(triangle_right);
		}
	}

	auto poly_data = vtkSmartPointer<vtkPolyData>::New();
	poly_data->SetPoints(points);
	poly_data->SetPolys(cells);
	meshData = poly_data;
}
