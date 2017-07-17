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
	if (m_pPoints != nullptr)
	{
		if (m_iIndex == 0)	//initial point
		{
			double* point = new double[3];
			//set the initial camera's position
			point = m_pPoints->GetPoint(0);	//the 1st
			m_vtkCamera->SetPosition(point);
			//m_vtkCamera->SetEyePosition(point);

			//set the initial eye view camera's direction
			point = m_pPoints->GetPoint(1);
			//m_vtkCamera->SetEyePosition(point);
			m_vtkCamera->SetFocalPoint(point);
		}
		else
		{
			double* point = new double[3];
			point = m_pPoints->GetPoint(m_iIndex);
			m_vtkCamera->SetPosition(point[0], point[1], point[2]);
			//m_vtkCamera->SetEyePosition(point);
			if (m_iIndex <  m_pPoints->GetNumberOfPoints() - 1)	//set the camera eye's direction to see
			{
				point = m_pPoints->GetPoint(m_iIndex + 1);	//the next one
				//m_vtkCamera->SetEyePosition(point);					//set the initial eye view camera's direction
				m_vtkCamera->SetFocalPoint(point);
			}
		}
		
		//reset at beginning of the path
		if (m_iIndex == m_pPoints->GetNumberOfPoints() - 1)
			m_iIndex = 0;

		m_iIndex++;
	}
	
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