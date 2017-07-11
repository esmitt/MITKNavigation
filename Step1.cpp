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

//customized
#include "navigation.h"
#include "vtkTimerUser.h"

#include <vtkConeSource.h>
#include <vtkVertexGlyphFilter.h>

// ** NOT USED **
//if was invoked alone, as a function using the SetCallback 
//
//void TimerCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* clientData, void* vtkNotUsed(callData))
//{
//	vtkRenderWindowInteractor *iren =	static_cast<vtkRenderWindowInteractor*>(caller);
//	//do to something here
//	iren->Render();
//}

#include <vtkSmartPointer.h>
#include <vtkRendererCollection.h>
#include <vtkWorldPointPicker.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkObjectFactory.h>
#include <vtkPointPicker.h>

class vtkLeftCLiking : public vtkCommand
{
	//attributes
private:
public:
	CGraph* graph;

public:

	// Important!
	static vtkLeftCLiking *New() 
	{
		vtkLeftCLiking *pCallBack = new vtkLeftCLiking;
		return pCallBack;
	}

	// Function that execute the main code of the Command
	virtual void Execute(vtkObject *caller, unsigned long eventId, void * vtkNotUsed(callData)) 
	{		
		vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
		double* p = interactor->GetPicker()->GetPickPosition();
		cout << p[0] << " " << p[1] << " " << p[2] << endl;	//x, y, z
		//find the point inside the graph and print the ID
		int index = graph->getID(p[0], p[1], p[2]);
		cout << index << endl;
	}
};

//##Documentation
//## @brief Load image (nrrd format) and display it in a 2D view
int main(int argc, char *argv[])
{
	QApplication qtapplication(argc, argv);
	CNavigation navigation;
	
	// Register Qmitk-dependent global instances
	QmitkRegisterClasses();

	// Create a DataStorage
	// The DataStorage manages all data objects. It is used by the
	// rendering mechanism to render all data objects
	// We use the standard implementation mitk::StandaloneDataStorage.
	mitk::StandaloneDataStorage::Pointer ds = mitk::StandaloneDataStorage::New();

	// Load datanode (eg. many image formats, surface formats, etc.)
	std::cout << "Status: Reading the OBJ file ..." << endl;
	mitk::IOUtil::Load("C:\\code\\bronchi labelling\\output.obj", *ds);
	std::cout << "Status: Reading the MAT file ..." << endl;
	if(!navigation.openMATFile("C:\\e\\Examples\\Tutorial\\Step1\\EXACTCase22_skel_graph.mat"))
		return EXIT_FAILURE;

	// Create a RenderWindow
	QmitkRenderWindow renderWindow;

	// Get the rendered
	mitk::BaseRenderer::Pointer rendered = mitk::BaseRenderer::GetInstance(renderWindow.GetVtkRenderWindow());
	
	// Create the cone using VTK
	vtkSmartPointer<vtkConeSource> cone = vtkConeSource::New();
	cone->SetHeight(5);
	cone->SetRadius(3);
	cone->SetResolution(100);
	cone->SetCenter(242.29, 203.07, 488.58);
	cone->Update();

	// Create the MITK surface object
	mitk::Surface::Pointer mitk_cone_surface = mitk::Surface::New();

	// Asign the VTK cone to MITK surface
	mitk_cone_surface->SetVtkPolyData(cone->GetOutput());

	// Create a new node in DataNode with properties
	mitk::DataNode::Pointer coneResult = mitk::DataNode::New();
	std::string nameOfResultImage = "cone-in-path";
	coneResult->SetProperty("name", mitk::StringProperty::New(nameOfResultImage));
	coneResult->SetColor(1, 0, 0);	//red color

	// Set the surface into the Datanode
	coneResult->SetData(mitk_cone_surface);

	// Add a data node
	ds->Add(coneResult);
	
	//// Get the existing camera and set a new position and orientation
	//// this preserve the current functionalities of the current camera
	vtkSmartPointer<vtkCamera> tCamera = renderWindow.GetRenderer()->GetVtkRenderer()->GetActiveCamera();
	tCamera->SetPosition(242.29, 203.07, 488.58);
	tCamera->SetFocalPoint(223.96, 202.18, 327.66);

	//// Get the existing interactor from the GetInteractor() directly instead use GetVtkRenderWindowInteractor
	vtkSmartPointer<QVTKInteractor> qInteractor = renderWindow.GetInteractor();
	qInteractor->SetRenderWindow(renderWindow.GetRenderWindow());
	int timerId = qInteractor->CreateRepeatingTimer(1000);

	// Add a data node
	std::cout << "Status: Adding surfaces from Graph ..." << endl;
	navigation.computePath(4646);	//starting node just for convenience 
	navigation.computeMST(4646);
	ds->Add(navigation.getDrawablePoints());
	//ds->Add(navigation.getDrawableLines());
	std::cout << "Status: Computing Prim algorithm ..." << endl;
	//navigation.getGraph()->primtMST(4646);
	ds->Add(navigation.getMSTDrawingPath(4646, 3445));
	//ds->Add(navigation.getDrawingPath(4646, 27));	//ending point 27 (for testing)
	//ds->Add(navigation.getDrawingPath(4646, 4390));
	ds->Add(navigation.getDrawingPath(4646, 3445));
	ds->Print(cout);

	// Instancing a class to handle the TimerEvent function, added as an Observer of renderWindow interactor
	vtkSmartPointer<vtkTimerUser> tCBInstance = vtkSmartPointer<vtkTimerUser>::New();
	tCBInstance->m_vtkCamera = tCamera;
	qInteractor->AddObserver(vtkCommand::TimerEvent, tCBInstance);

	vtkSmartPointer<vtkLeftCLiking> lctCBInstance = vtkSmartPointer<vtkLeftCLiking>::New();
	lctCBInstance->graph = navigation.getGraph();	//just to select a point
	qInteractor->AddObserver(vtkCommand::LeftButtonPressEvent, lctCBInstance);
	
	///////////////////////////////////////////
	/////////				final part
	//////////////////////////////////////////
	// Change the color of Background
	rendered->GetVtkRenderer()->SetBackground(0.15, 0.15, 0.15);

	// Use it as a 3D view!
	renderWindow.GetRenderer()->SetMapperID(mitk::BaseRenderer::Standard3D);

	// Add the node to DataStorage
	rendered->SetDataStorage(ds);

	//update all instances
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();

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
