#ifndef _VTK_TIME_USER_
#define _VTK_TIME_USER_

//vtk
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkCommand.h>

/// Class to handle an implementation of an Observer under a Command event
class vtkTimerUser : public vtkCommand
{
	//attributes
private:
	int m_iTimerCount;
	vtkSmartPointer<vtkPoints> m_pPoints;
	int m_iIndex;
public:
	vtkSmartPointer<vtkCamera> m_vtkCamera;

public:
	// Important!
	static vtkTimerUser *New();

	// Function to set the number of Points
	void setPath(const vtkSmartPointer<vtkPoints> & pPoints);

	// Function that execute the main code of the Command
	virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData));
};

#endif //_VTK_TIME_USER_