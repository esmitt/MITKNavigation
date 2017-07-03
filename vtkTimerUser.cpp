#include "vtkTimerUser.h"

vtkTimerUser* vtkTimerUser::New()
{
	vtkTimerUser *pCallBack = new vtkTimerUser;
	pCallBack->m_iTimerCount = 0;
	return pCallBack;
}

void vtkTimerUser::Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData))
{
	if (vtkCommand::TimerEvent == eventId)
	{
		++this->m_iTimerCount;
	}

	// Get the current position
	double* values = m_vtkCamera->GetPosition();
	m_vtkCamera->SetPosition(values[0], values[1], values[2] + (this->m_iTimerCount/0.1f));

	// Safe normally to continue
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
	iren->GetRenderWindow()->Render();
}