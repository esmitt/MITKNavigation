#include "navigation.h"
#include <iterator>
#include <algorithm>

CNavigation::CNavigation() 
{

}

CNavigation::~CNavigation() 
{

}

bool CNavigation::openMATFile(const std::string & strFilename) 
{
	bool result = m_matObject.OpenFile(strFilename, m_graph);
	if (!result) return result;
	return true;
}

void CNavigation::computePath(const int& index)
{
	cout << "Status: Computing the shortest distance between all nodes ..." << std::endl;
	m_vPathParent = m_graph.shortestPath(index);
}

bool CNavigation::pathInGraph(int source, int destination, std::vector<int>& path)
{
	if (m_vPathParent[destination] == -1)	//path does not exists
		return false;

	if (m_vPathParent[destination] == source)	//base case
		return true;		//path found

	bool r =  pathInGraph(source, m_vPathParent[destination], path);
	path.push_back(destination);
	return r;
}

mitk::DataNode::Pointer CNavigation::getDrawingPath(const int & i, const int & j)
{
	assert(m_vPathParent.size());	//the computePath should be invoked first

	//compute the nodes involves in the path, starting from i and finished in j (included)
	std::vector<int> path;
	if (pathInGraph(i, j, path))
			path.insert(path.begin(), i);	//insert the first one at the top, to complete the path
	else 
	{
		cout << "There is no a path between " << i << " and " << j << endl;
		return nullptr;
	}
	
	std::for_each(path.begin(), path.end(), [](int i) {cout << i << " "; });

	// Create the MITK surface object
	mitk::Surface::Pointer lines_surface = mitk::Surface::New();
	lines_surface->SetVtkPolyData(m_graph.getPolyDataPath(path));

	// Create a new node in DataNode with properties
	mitk::DataNode::Pointer result = mitk::DataNode::New();
	result->SetColor(0.1, 0.6, 0.9);
	std::string nameOfOuputImage = "path";
	result->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));
	
	lines_surface->Update();
	result->SetData(lines_surface);
	float width = 3;
	result->SetFloatProperty("material.wireframeLineWidth", width);
	return result;
}