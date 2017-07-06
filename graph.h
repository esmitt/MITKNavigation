#pragma once
#include "vtkPoints.h"
#include <vtkMath.h>
#include <set>
#include <vector>
#include <algorithm>
#include <mitkStandaloneDataStorage.h>
#include <mitkIOUtil.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkLineSource.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>

class GraphVertex
{
private:
	int m_iIndex;	//index of the GraphVertex
	std::set <std::pair<int, double>> m_vNeighbours;	//store a pair of values (id, distance) into a set
	double m_Point[3];	//point 3D

public:
	GraphVertex()
	{
		m_Point[0] = m_Point[1] = m_Point[2] = 0;
		m_iIndex = -1;
	}

	GraphVertex(double* value)
	{
		m_Point[0] = value[0]; m_Point[1] = value[1]; m_Point[2] = value[2];
		m_iIndex = -1;
	}

	GraphVertex(int index, double* value)
	{
		m_Point[0] = value[0]; m_Point[1] = value[1]; m_Point[2] = value[2];
		m_iIndex = index;
	}

	GraphVertex(int index, double x, double y, double z)
	{
		m_Point[0] = x; m_Point[1] = y; m_Point[2] = z;
		m_iIndex = index;
	}

	GraphVertex(const GraphVertex &v)
	{
		m_Point[0] = v.m_Point[0];
		m_Point[1] = v.m_Point[1];
		m_Point[2] = v.m_Point[2];
		m_iIndex = v.m_iIndex;
	}

	~GraphVertex() {}

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

struct Graph
{
private:
	std::vector<GraphVertex> m_vGraphVertexes;
	vtkSmartPointer<vtkPoints> m_vtkPoints;

public:
	void addGraphVertex(GraphVertex v)
	{
		m_vGraphVertexes.push_back(v);
		m_vtkPoints->InsertNextPoint(v.get_ptr());
	}

	void constructVTKPoints()
	{
		m_vtkPoints->SetNumberOfPoints(m_vGraphVertexes.size());	//set the number of vertexes for efficiency
																															//copy points into
		std::for_each(m_vGraphVertexes.begin(), m_vGraphVertexes.end(), [&](GraphVertex& v)
		{
			m_vtkPoints->SetPoint(v.getIndex(), v.get_ptr());
		});
	}

	void addEdgeIndex(int index1, int index2)
	{
		m_vGraphVertexes[index1].link(index2);
		m_vGraphVertexes[index2].link(index1);	//bidirectional?
	}

	// Compute the euclidean distance between all pair of nodes nodes/vertexes
	void computeDistances(double epsilon = 99)
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

	//print for testing
	void print()
	{
		std::for_each(m_vGraphVertexes.begin(), m_vGraphVertexes.end(), [](GraphVertex v)
		{
			std::cout << v.getIndex() << std::endl;
		});
	}

	// Return a MITK node to be added into the drawing pipeline
	mitk::DataNode::Pointer getDrawableLines()
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
		linesResult->SetColor(0, 1, 0);
		std::string nameOfOuputImage = "lines-in-path";
		linesResult->SetProperty("name", mitk::StringProperty::New(nameOfOuputImage));

		// Set the surface into the Datanode
		linesResult->SetData(lines_surface);
		return linesResult;
	}

	// Return a MITK node to be added into the drawing pipeline
	mitk::DataNode::Pointer getDrawableObject()
	{
		//add points into a vtkPolyData structure
		vtkSmartPointer<vtkPolyData> pointsPolydata = vtkSmartPointer<vtkPolyData>::New();
		pointsPolydata->SetPoints(m_vtkPoints);

		vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
		vertexFilter->SetInputData(pointsPolydata);
		vertexFilter->Update();

		vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
		polydata->ShallowCopy(vertexFilter->GetOutput());

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

	Graph()
	{
		m_vtkPoints = vtkSmartPointer<vtkPoints>::New();
	}

	~Graph()
	{
		//m_vtkPoints->Delete();
	}

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
};