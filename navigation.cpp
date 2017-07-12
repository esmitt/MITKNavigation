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

void CNavigation::computeMST(const int& index)
{
	cout << "Status: Computing MST path ..." << std::endl;
	m_vPathMST = m_graph.primtMST(index);
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

void CNavigation::pathInMST(int source, int destination, std::vector<int>& path) 
{
	if (m_vPathMST[destination] != -1)	//no parent
	{
		if (m_vPathMST[destination] != source)	//base case
		{
			path.push_back(destination);
			pathInMST(source, m_vPathMST[destination], path);
		}
	}
}

mitk::DataNode::Pointer CNavigation::getMSTDrawingPath()
{
	// Create the MITK surface object
	mitk::Surface::Pointer lines_surface = mitk::Surface::New();
	lines_surface->SetVtkPolyData(m_graph.getPolyMSTComplete(m_vPathMST));

	// Create a new node in DataNode with properties
	mitk::DataNode::Pointer result = mitk::DataNode::New();
	result->SetColor(0, 0.56, 1);
	std::string nameOfOuputImage = "complete-path-MST";
	result->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));

	lines_surface->Update();
	result->SetData(lines_surface);
	result->SetFloatProperty("material.wireframeLineWidth", 3);	//3 as width of the line
	return result;
}
mitk::DataNode::Pointer CNavigation::getMSTDrawingPath(const int & i, const int & j) 
{
	//compute the nodes involves in the path, starting from i and finished in j (included)
	std::vector<int> path;
	pathInMST(i, j, path);
	// Create the MITK surface object
	mitk::Surface::Pointer lines_surface = mitk::Surface::New();
	lines_surface->SetVtkPolyData(m_graph.getMSTDataPath(path));

	// Create a new node in DataNode with properties
	mitk::DataNode::Pointer result = mitk::DataNode::New();
	result->SetColor(1, 0.56, 0);
	std::string nameOfOuputImage = "path-MST";
	result->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));

	lines_surface->Update();
	result->SetData(lines_surface);
	result->SetFloatProperty("material.wireframeLineWidth", 3);	//3 as width of the line
	return result;
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
	
	//std::for_each(path.begin(), path.end(), [](int i) {cout << i << " "; });	//debug path

	// Create the MITK surface object
	mitk::Surface::Pointer lines_surface = mitk::Surface::New();
	lines_surface->SetVtkPolyData(m_graph.getPolyDataPath(path));

	// Create a new node in DataNode with properties
	mitk::DataNode::Pointer result = mitk::DataNode::New();
	//result->SetColor(0.1, 0.6, 0.9);
	result->SetColor(1.0, 0.6, 0.9);
	std::string nameOfOuputImage = "path";
	result->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));
	
	lines_surface->Update();
	result->SetData(lines_surface);
	result->SetFloatProperty("material.wireframeLineWidth", 3);	//3 as width of the line
	return result;
}