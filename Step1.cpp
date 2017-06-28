/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "QmitkRegisterClasses.h"
#include "QmitkRenderWindow.h"

#include <mitkStandaloneDataStorage.h>

#include <QApplication>
#include <itksys/SystemTools.hxx>

#include <mitkIOUtil.h>

#include <mitkCameraController.h>

#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>

// ** NOT USED **
//if was invoked alone, as a function using the SetCallback 
//
//void TimerCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData))
//{
//	vtkRenderWindowInteractor *iren =	static_cast<vtkRenderWindowInteractor*>(caller);
//	//do to something here
//	iren->Render();
//}

/// Class to handle an implementation of an Observer under a Command event
class vtkTimerCallback2 : public vtkCommand
{
	//attributes
private:
	int m_iTimerCount;
public:
	vtkSmartPointer<vtkCamera> m_vtkCamera;
	
public:
	// Important!
	static vtkTimerCallback2 *New()
	{
		vtkTimerCallback2 *pCallBack = new vtkTimerCallback2;
		pCallBack->m_iTimerCount = 0;
		return pCallBack;
	}

	// Function that execute the main code of the Command
	virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData))
	{
		if (vtkCommand::TimerEvent == eventId)
		{
			++this->m_iTimerCount;
		}
		
		// Get the current position
		double* values = m_vtkCamera->GetPosition();
		m_vtkCamera->SetPosition(values[0], values[1], values[2] + this->m_iTimerCount);
		
		// Safe normally to continue
		vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::SafeDownCast(caller);
		iren->GetRenderWindow()->Render();
	}
};

//##Documentation
//## @brief Load image (nrrd format) and display it in a 2D view
int main(int argc, char *argv[])
{
	QApplication qtapplication(argc, argv);

	// Register Qmitk-dependent global instances
	QmitkRegisterClasses();

	// Create a DataStorage
	// The DataStorage manages all data objects. It is used by the
	// rendering mechanism to render all data objects
	// We use the standard implementation mitk::StandaloneDataStorage.
	mitk::StandaloneDataStorage::Pointer ds = mitk::StandaloneDataStorage::New();

	// Load datanode (eg. many image formats, surface formats, etc.)
	mitk::IOUtil::Load("C:\\code\\bronchi labelling\\output.obj", *ds);

	// Create a RenderWindow
	QmitkRenderWindow renderWindow;

	// Tell the RenderWindow which (part of) the datastorage to render
	renderWindow.GetRenderer()->SetDataStorage(ds);

	// Use it as a 3D view!
	renderWindow.GetRenderer()->SetMapperID(mitk::BaseRenderer::Standard3D);

	// Get the existing camera and set a new position and orientation
	// this preserve the current functionalities of the current camera
	vtkSmartPointer<vtkCamera> tCamera = renderWindow.GetRenderer()->GetVtkRenderer()->GetActiveCamera();
	tCamera->SetPosition(242.29, 203.07, 488.58);
	tCamera->SetFocalPoint(223.96, 202.18, 327.66);

	// Get the existing interactor from the GetInteractor() directly instead use GetVtkRenderWindowInteractor
	vtkSmartPointer<QVTKInteractor> qInteractor = renderWindow.GetInteractor();
	qInteractor->SetRenderWindow(renderWindow.GetRenderWindow());
	int timerId = qInteractor->CreateRepeatingTimer(100);

	// Instancing a class to handle the TimerEvent function, added as an Observer of renderWindow interactor
	vtkSmartPointer<vtkTimerCallback2> tCBInstance = vtkSmartPointer<vtkTimerCallback2>::New();
	tCBInstance->m_vtkCamera = tCamera;
	qInteractor->AddObserver(vtkCommand::TimerEvent, tCBInstance);

	// Regular drawing window
	renderWindow.show();
	renderWindow.resize(512, 512);


	// for testing
#include "QtTesting.h"
	if (strcmp(argv[argc - 1], "-testing") != 0)
		return qtapplication.exec();
	else
		return QtTesting();

	// cleanup: Remove References to DataStorage. This will delete the object
	ds = nullptr;
}
/**
\example Step1.cpp
*/
