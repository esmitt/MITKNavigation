#include "graph.h"
#include <assert.h>

/// Constructor
Graph::Graph()
{
	m_vtkPoints = vtkSmartPointer<vtkPoints>::New();
}

/// Destructor
Graph::~Graph() {}

/// Function to add a single vertex into the Graph. Also, add the vtkPoint
///
/// @param v vertex to add into the graph
void Graph::addGraphVertex(GraphVertex v)
{
	m_vGraphVertexes.push_back(v);
	m_vtkPoints->InsertNextPoint(v.get_ptr());
}

/// Function to construct the array of vtkPoints with all points in the Graph
/// IMPORTANT: the variable m_vtkPoints should be with data
void Graph::constructVTKPoints()
{
	assert(m_vGraphVertexes.size());	//DEBUG: should be different from zero
	
	m_vtkPoints->SetNumberOfPoints(m_vGraphVertexes.size());	//set the number of vertexes for efficiency
	//copy points into
	std::for_each(m_vGraphVertexes.begin(), m_vGraphVertexes.end(), [&](GraphVertex& v)
	{
		m_vtkPoints->SetPoint(v.getIndex(), v.get_ptr());
	});
}

/// Function to link/connect a vertex with another one (using only its IDs)
///
/// @param index1 integer index of vertex to link
/// @param index2 integer index of vertex to link
void Graph::addEdgeIndex(int index1, int index2)
{
	assert(m_vGraphVertexes.size());	//DEBUG: should be >= 0
	assert(index1 && index2);//DEBUG: should be positive

	m_vGraphVertexes[index1].link(index2);
}

/// Function to calculate the Euclidean distance between all connected points
///
/// @param epsilon determines the distance to connect a vertex with another one.
/// It should be the radius of a sphere, given the condition to test
void Graph::computeDistances(double epsilon )
{
	auto iIter = m_vGraphVertexes.begin();
	const auto iEnd = m_vGraphVertexes.end();	//to indicate the end
	double distance;

	while (iIter != iEnd)		//1st loop
	{
		auto jIter = iIter + 1;	//I assumed that it works as indexes
		while (jIter != iEnd)		//2nd loop
		{
			distance = vtkMath::Distance2BetweenPoints((*iIter).get_ptr(), (*jIter).get_ptr());
			//here should store only the required ones. Only store the distances less than a certain criteria
			//for example less than the radius of a circle
			// ** for now is storing all distances! (this is bad)
			if (distance < epsilon)	//only consider the less than EPSILON
			{
				iIter->link((*jIter).getIndex(), distance);
				jIter->link((*iIter).getIndex(), distance);	//bidirectional
			}
			jIter++;
		}
		iIter++;
	}//end while

	 //print just to see
	 //std::for_each(m_vGraphVertexes.begin(), m_vGraphVertexes.end(), [](GraphVertex & v) 
	 //{
	 //	v.printNeighbours();
	 //	//std::cout << v.getIndex() << std::endl;
	 //});
}

/// Function which creates a Surface (MITK) and associate all internal data on it. 
/// After, the surface is stored into a DataNode to be returned. 
/// Surface contains lines linked between them in the graph
///
/// @return a pointer to a datanode of MITK to be added into the existing drawing pipeline
mitk::DataNode::Pointer Graph::getDrawableLines()
{
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->Allocate();

	std::for_each(m_vGraphVertexes.begin(), m_vGraphVertexes.end(), [&polydata](GraphVertex & v)
	{
		std::set<std::pair<int, double>> theSet = v.getNeighbours();	//explore each neighbour
		std::for_each(theSet.begin(), theSet.end(), [&polydata, &v](std::pair<int, double> element)
		{
			vtkIdType line[2] = { v.getIndex(), element.first };
			polydata->InsertNextCell(VTK_LINE, 2, line);
		});
	});

	//set the lines info into the polydata
	polydata->SetPoints(m_vtkPoints);

	// Create the MITK surface object
	mitk::Surface::Pointer lines_surface = mitk::Surface::New();
	lines_surface->SetVtkPolyData(polydata);

	// Create a new node in DataNode with properties
	mitk::DataNode::Pointer linesResult = mitk::DataNode::New();
	linesResult->SetColor(0.9, 0.1, 0.1);
	std::string nameOfOuputImage = "lines-in-path";
	linesResult->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));

	// Set the surface into the Datanode
	linesResult->SetData(lines_surface);
	return linesResult;
}

/// Function which creates a Surface (MITK) and associate all internal data on it. 
/// After, the surface is stored into a DataNode to be returned
/// Surface contains points of the graph
///
/// @return a pointer to a datanode of MITK to be added into the existing drawing pipeline
mitk::DataNode::Pointer Graph::getDrawablePoints()
{
	//add points into a vtkPolyData structure
	vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
	pointsPolydata->SetPoints(m_vtkPoints);

	vtkSmartPointer<vtkGlyph3D> glyph3D =	vtkSmartPointer<vtkGlyph3D>::New();
	vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
	sphereSource->SetRadius(0.1f);

	glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
	glyph3D->SetInputData(pointsPolydata);
	glyph3D->Update();

	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->ShallowCopy(glyph3D->GetOutput());

	// Create the MITK surface object
	mitk::Surface::Pointer points_surface = mitk::Surface::New();
	points_surface->SetVtkPolyData(polydata);

	// Create a new node in DataNode with properties
	mitk::DataNode::Pointer pointResult = mitk::DataNode::New();
	pointResult->SetColor(0, 1, 0);
	std::string nameOfOuputImage = "points-in-path";
	pointResult->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));

	// Set the surface into the Datanode
	pointResult->SetData(points_surface);
	return pointResult;
}