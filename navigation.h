#pragma once

#include <mitkStandaloneDataStorage.h>
#include <string>
#include "matLoader.h"
#include "graph.h"

class CNavigation 
{
private:
	CMatLoader m_matObject;
	CGraph m_graph;
	std::vector<int> m_vPathParent;
	std::vector<int> m_vPathMST;

protected:
	bool pathInGraph(int source, int destination, std::vector<int>& path);
	bool pathInMST(int source, int destination, std::vector<int>& path);

public:
	CNavigation();
	~CNavigation();

	// Open a .MAT file
	bool openMATFile(const std::string & strFilename);

	// Compute the shortest distance from vertex-index
	void computePath(const int& index = 0);

	void computeMST(const int& index = 0);

	// Return the datanode which contains the path between node i and node j
	mitk::DataNode::Pointer getDrawingPath(const int & i, const int & j);
	mitk::DataNode::Pointer getMSTDrawingPath(const int & i, const int & j);

	mitk::DataNode::Pointer getDrawablePoints() { return m_graph.getDrawablePoints(); }
	mitk::DataNode::Pointer getDrawableLines() { return m_graph.getDrawableLines(); }
	
	//testing
	CGraph* getGraph() { return &m_graph; }
};