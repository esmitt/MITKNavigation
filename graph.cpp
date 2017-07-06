#include "graph.h"
#include <assert.h>

#define INF 0x3f3f3f3f

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

// Function to print shortest path from source to j
// using parent array
void Graph::printPath(std::vector<int> parent, int j)
{
	// Base Case : If j is source
	if (parent[j] == -1)
		return;

	printPath(parent, parent[j]);

	printf("%d ", j);
}

//extracted from http://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-set-in-stl/
std::vector<int> Graph::shortestPath(const int & indexI, const int indexJ)
{
	std::vector<int> vEdges(m_vGraphVertexes.size(), -1);	//-1 is no parent at all

	std::set<Graph::tEdge> setds;
	// Create a vector for distances and initialize all
	// distances as infinite (INF)
	std::vector<int> dist(m_vGraphVertexes.size(), INF);

	// Insert source itself in Set and initialize its
	// distance as 0.
	setds.insert(std::make_pair(0, indexI));
	dist[indexI] = 0;

	//Looping till all shortest distance are finalized
	//	then setds will become empty 
	while (!setds.empty()) 
	{
		// The first vertex in Set is the minimum distance
		Graph::tEdge tmp = *(setds.begin());
		// vertex, extract it from set.
		setds.erase(setds.begin());
		// vertex label is stored in second of pair (it
		// has to be done this way to keep the vertices
		// sorted distance (distance must be first item
		// in pair)
		int u = tmp.second;
		// 'i' is used to get all adjacent vertices of a vertex
		//std::list<Graph::tEdge>::iterator i;
		std::set <std::pair<int, double>> adj;
		for (auto i = m_vGraphVertexes[u].getNeighbours().begin(); i != m_vGraphVertexes[u].getNeighbours().end(); ++i)
		{
			// Get vertex label and weight of current adjacent
			// of u.
			int v = (*i).first;
			double weight = (*i).second;
			//  If there is shorter path to v through u.
			if (dist[v] > dist[u] + weight)
			{
				/*  If distance of v is not INF then it must be in
				our set, so removing it and inserting again
				with updated less distance.
				Note : We extract only those vertices from Set
				for which distance is finalized. So for them,
				we would never reach here.  */
				if (dist[v] != INF)
					setds.erase(setds.find(std::make_pair(dist[v], v)));

				// Updating distance of v
				dist[v] = dist[u] + weight;
				vEdges[v] = u;
				setds.insert(std::make_pair(dist[v], v));
			}
		}
	}
	//// Print shortest distances stored in dist[]
	//printf("Vertex   Distance from Source\n");
	//for (int i = 0; i < m_vGraphVertexes.size(); ++i)
	//	printf("%d \t\t %d\n", i, dist[i]);

	int src = 0;
	//printf("Vertex\t  Distance\tPath");
	//for (int i = 1; i <  m_vGraphVertexes.size(); i++)
	//{
		//printf("\n%d -> %d \t\t %d\t\t%d ", src, i, dist[i], src);
		printPath(vEdges, 5379);
	//}

	return vEdges;
}