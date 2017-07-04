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
	//double* values = m_vtkCamera->GetPosition();
	//m_vtkCamera->SetPosition(values[0], values[1], values[2] + (this->m_iTimerCount/0.1f));
	//if (m_iIndex < m_pPoints->GetNumberOfPoints())
	//{
	//	double point[3];
	//	m_pPoints->GetPoint(m_iIndex, point);
	//	m_vtkCamera->SetPosition(point[0], point[1], point[2]);
	//	m_iIndex++;
	//	//
	//	//m_vtkCamera->SetPosition(m_pPoints->get[0], m_pPoints[1], m_pPoints[2]);
	//}
	//else 
	//{
	//	m_iIndex = 0;
	//}

	// Safe normally to continue
	vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
	iren->GetRenderWindow()->Render();
}

void vtkTimerUser::setPath(const vtkSmartPointer<vtkPoints> & pPoints)
{
	m_pPoints = pPoints;
	m_iIndex = 0;
}