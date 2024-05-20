#include <vtkPolyDataMapper.h>
#include <GenericMesh.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <MathUtils.h>
class PlaneMesh : public GenericMesh<FrameData> {
    int res_width, res_height;
public:
	PlaneMesh(int x, int y);

    //fields
    vtkSmartPointer<vtkPolyData> meshData;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkActor> actor;

    //methods
    void Initialize() override;
    void Tick(float deltaTime) override;
    void UpdateData(FrameData* data) override;

protected:
    void GenerateMesh() override;
    void SetupActor();
};