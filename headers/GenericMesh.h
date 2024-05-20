#include<vtkActor.h>
#include<vtkPolyDataMapper.h>
#include<vtkSmartPointer.h>
#include<vtkRenderer.h>
#include<vtkCell.h>
#include<vtkTriangle.h>
#include<vtkPoints.h>
#include<FrameData.h>
#ifndef generic_mesh_h
#define generic_mesh_h

#endif // !generic_mesh.h

template <class Data = FrameData> class GenericMesh {
public:
    vtkSmartPointer<vtkActor> actor;
    virtual void Initialize();
    virtual void Tick(float deltaTime);
    virtual void UpdateData(Data* data);
    ~GenericMesh();
protected:
    Data* data;
    virtual void GenerateMesh();
};

template<class Data>
void GenericMesh<Data>::Initialize()
{
}

template<class Data>
void GenericMesh<Data>::Tick(float deltaTime)
{
}

template<class Data>
void GenericMesh<Data>::UpdateData(Data* data)
{
}

template<class Data>
GenericMesh<Data>::~GenericMesh()
{
    delete data;
}


template<class Data>
void GenericMesh<Data>::GenerateMesh()
{
}
