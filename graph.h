#pragma once
#include "vtkPoints.h"
#include "utility.h"
#include <vtkMath.h>
#include <set>
#include <vector>
#include <algorithm>
#include <mitkStandaloneDataStorage.h>
#include <mitkIOUtil.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkLabeledDataMapper.h>
#include <vtkVectorText.h>
#include <vtkGlyph3D.h>
#include <vtkLineSource.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>

// @author Esmitt Ramirez
// @date 07/2017

/// Struct to handle a graph's vertex. It stores an ID, a 3D point, and a set of linked neighbours
struct CGraphVertex
{
private:
	int m_iIndex;	//index of the CGraphVertex
	std::set <std::pair<int, double>> m_vNeighbours;	//store a pair of values (id, distance) into a set
	double m_Point[3];	//point 3D

public:
	CGraphVertex()
	{
		m_Point[0] = m_Point[1] = m_Point[2] = 0;
		m_iIndex = -1;
	}

	CGraphVertex(double* value)
	{
		m_Point[0] = value[0]; m_Point[1] = value[1]; m_Point[2] = value[2];
		m_iIndex = -1;
	}

	CGraphVertex(int index, double* value)
	{
		m_Point[0] = value[0]; m_Point[1] = value[1]; m_Point[2] = value[2];
		m_iIndex = index;
	}

	CGraphVertex(int index, double x, double y, double z)
	{
		m_Point[0] = x; m_Point[1] = y; m_Point[2] = z;
		m_iIndex = index;
	}

	CGraphVertex(const CGraphVertex &v)
	{
		m_Point[0] = v.m_Point[0];
		m_Point[1] = v.m_Point[1];
		m_Point[2] = v.m_Point[2];
		m_iIndex = v.m_iIndex;
	}

	~CGraphVertex() {}

	//functions
	double & operator[](int index) { return m_Point[index]; }

	//add an edge
	void link(int index, double distance = 0)
	{
		m_vNeighbours.insert(std::make_pair(index, distance));
	}

	inline int getIndex() { return m_iIndex; }

	double* get_ptr() { return m_Point; }

	std::set<std::pair<int, double>>& getNeighbours()
	{
		return m_vNeighbours;
	}

	void printNeighbours()
	{
		auto iIt = m_vNeighbours.begin();
		while (iIt != m_vNeighbours.end())
		{
			std::cout << "(" << m_iIndex << ", " << iIt->first << ") = " << iIt->second << endl;
			iIt++;
		}
	}
};

/// Class to handle a directed/undirected graph using an adjacency list.
/// The vertexes are CGraphVertex datatype, also stores vtkPoints to render
class CGraph
{
private:
	std::vector<CGraphVertex> m_vGraphVertexes;
	vtkSmartPointer<vtkPoints> m_vtkPoints;
	typedef std::pair<int, int> tEdge;	//to define an edge as the link between two nodes represented by their indexes

public:
	void addGraphVertex(CGraphVertex v);
	void constructVTKPoints();
	void addEdgeIndex(int index1, int index2);
	
	// Get the polydata associated with the path in the vector
	vtkSmartPointer<vtkPolyData> getPolyDataPath(std::vector<int> path);

	// Compute the euclidean distance between all pair of nodes nodes/vertexes
	void computeDistances(double epsilon = CUtility::getInstance()->RADIUS_DISTANCE);	

	// Return a MITK node to be added into the drawing pipeline
	mitk::DataNode::Pointer getDrawableLines();

	// Return a MITK node to be added into the drawing pipeline
	mitk::DataNode::Pointer getDrawablePoints();

	// Find the shortest path from node to all nodex
	std::vector<int> shortestPath(const int & indexI = 0);

	//print for testing
	void print()
	{
		std::for_each(m_vGraphVertexes.begin(), m_vGraphVertexes.end(), [](CGraphVertex v)
		{
			std::cout << v.getIndex() << std::endl;
		});
	}

	//return the ID closer to the point (x,y,z) inside the graph (if exists)
	int getID(double x, double y, double z) 
	{
		
	}

	CGraph();
	~CGraph();
	
};

//std::vector<Vertex>::iterator findVertexIndex(double* val, bool& res)
//{
//	std::vector<Vertex>::iterator it;
//	Vertex v(val);
//	it = std::find(m_vVertexes.begin(), m_vVertexes.end(), v);
//	if (it != m_vVertexes.end()) {
//		res = true;
//		return it;
//	}
//	else {
//		res = false;
//		return m_vVertexes.end();
//	}
//}

//void addEdge(int n1, int n2)
//{
//	bool foundNet1 = false, foundNet2 = false;
//	auto vit1 = findVertexIndex(n1, foundNet1);
//	int node1Index = -1, node2Index = -1;
//	if (!foundNet1)
//	{
//		Vertex v1(n1);
//		m_vVertexes.push_back(v1);
//		node1Index = m_vVertexes.size() - 1;
//	}
//	else
//	{
//		node1Index = vit1 - m_vVertexes.begin();
//	}
//	Vertices::iterator vit2 = findVertexIndex(n2, foundNet2);
//	if (!foundNet2)
//	{
//		Vertex v2(n2);
//		m_vVertexes.push_back(v2);
//		node2Index = m_vVertexes.size() - 1;
//	}
//	else
//	{
//		node2Index = vit2 - vertices.begin();
//	}

//	assert((node1Index > -1) && (node1Index <  vertices.size()));
//	assert((node2Index > -1) && (node2Index <  vertices.size()));

//	addEdgeIndices(node1Index, node2Index);
//}