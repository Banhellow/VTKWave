#ifndef ARROWINTERACTORSTYLE_H
#define ARROWINTERACTORSTYLE_H
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
class ArrowInteractorStyle : public vtkInteractorStyleTrackballCamera
{

public:
	int* inputAxis;
	static ArrowInteractorStyle* New();
public:
	vtkTypeMacro(ArrowInteractorStyle, vtkInteractorStyleTrackballCamera);
	virtual void OnKeyPress() override;

	void InitiailizeAxis();

	int GetPosX() const;

	int GetPosY() const;

};
#endif // !ARROWINTERACTORSTYLE_H
