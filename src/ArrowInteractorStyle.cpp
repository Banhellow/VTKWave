#include "ArrowInteractorStyle.h"

void ArrowInteractorStyle::OnKeyPress()
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

void ArrowInteractorStyle::InitiailizeAxis()
{
	inputAxis = new int[2] {0, 0};
}

int ArrowInteractorStyle::GetPosX() const
{
	return this->inputAxis[0];
}

int ArrowInteractorStyle::GetPosY() const
{
	return this->inputAxis[1];
}
