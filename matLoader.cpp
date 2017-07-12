#include "matLoader.h"
#include <vector>
#include <iostream>
#include "utility.h"

CMatLoader::CMatLoader()
{
	m_pFileMat = nullptr;
}

CMatLoader::~CMatLoader()
{}

/// Function to load the file
/// @param strFilename Name of the file to open
/// @return true is file was open correctly, false otherwise
bool CMatLoader::OpenFile(std::string strFilename, CGraph & theData)
{
	try 
	{
		m_pFileMat = matOpen(strFilename.c_str(), "r");
		if (m_pFileMat == nullptr)	//for any reason, can not open it
			return false;

		///////////////////////////////////////////////////
		////read the skel
		///////////////////////////////////////////////////
		mxArray *pArrToSkel = matGetVariable(m_pFileMat, "skel");
		size_t nDim;
		const size_t* dime;
		if ((pArrToSkel != nullptr) && !mxIsEmpty(pArrToSkel) && mxIsLogical(pArrToSkel))
		{
		//	std::cout << "Status: Read from Graph the skel value ..." << endl;

		//	mwSize iDimSkel = mxGetNumberOfElements(pArrToSkel);
		//	bool *prLog = mxGetLogicals(pArrToSkel);

			nDim = mxGetNumberOfDimensions(pArrToSkel);	//number of dimensions 2D, 3D (i.e. 2, 3)
			dime = mxGetDimensions(pArrToSkel);		//get the array of size nDim with the sizes

		//	int* values = new int[nDim];
		//	int iIndex = 0;
		//	//store only all true-logic points
		//	for (size_t i = 0; i < iDimSkel; ++i)
		//	{
		//		if (prLog[i])	//the only who is true/remarkable
		//		{
		//			CUtility::getInstance()->ind2sub(dime, nDim, i, values);	//function from index to [x, y, z]
		//			//store points as vertexes into the graph
		//			theData.addGraphVertex(CGraphVertex(iIndex++, values[1], values[0], values[2]));
		//		}
		//	}
		//	std::cout << "Status: Compute distances ..." << endl;
		//	//related to the graph
		//	theData.computeDistances();	//calculate distance between vertexes
		//	delete values;
		}
		mxDestroyArray(pArrToSkel);

		/////////////////////////////////////////////////
		//read the main structure
		/////////////////////////////////////////////////
		mxArray *pArr = matGetVariable(m_pFileMat, "G");

		if ((pArr != nullptr) && !mxIsEmpty(pArr) && mxIsStruct(pArr))
		{
			std::cout << "Status: Read from Graph the G value ..." << endl;
			mwSize num = mxGetNumberOfFields(pArr);	//number of fields

		/////////////////////////////////////////////////
		//reading the 1st field (in this case G.e)
		/////////////////////////////////////////////////
			std::vector<double> v;
			mxArray *pArrToE = mxGetField(pArr, 0, "e");	//get the field 
			mwSize iDimE = mxGetNumberOfElements(pArrToE);
			double *pr = mxGetPr(pArrToE);
			//copy into a STL structure
			if (pr != nullptr)
			{
				v.resize(iDimE);
				v.assign(pr, pr + iDimE);
			}
			//for now, just printing
			//for (size_t i = 0; i<v.size(); ++i)
			//	std::cout << v[i] << std::endl;

			/////////////////////////////////////////////////
			//reading the G.we (3D points as indices)
			/////////////////////////////////////////////////
			//mxArray *pArrToGwe = mxGetCell(pArr, 0);
			mxArray *pArrToGwe = mxGetField(pArr, 0, "we");	//get the field 
			mwSize iDimGwe = mxGetNumberOfElements(pArrToGwe);	//get the number of elements 141x141
			int iIndex = 0;

			for (int k = 0; k < iDimGwe; k++) 
			{
				mxArray *pArrTmp = mxGetCell(pArrToGwe, k);
				mwSize size = mxGetNumberOfElements(pArrTmp);
			//	//const size_t* dime = mxGetDimensions(pArrTmp);		//get the array of size nDim with the sizes
			//	size_t dime[3];
			//	dime[0] = 
				int* values = new int[3];

				if (size != 0)	//is not empty
				{
					double *pr = mxGetPr(pArrTmp);
					for (int i = 0; i < size; i++)
					{
							CUtility::getInstance()->ind2sub(dime, nDim, pr[i], values);	//function from index to [x, y, z]
						//store points as vertexes into the graph
						theData.addGraphVertex(CGraphVertex(iIndex++, values[1], values[0], values[2]));
					}
				}
			//	//delete[] values;
			//	//mxDestroyArray(pArrTmp);
			}
			std::cout << "Status: Computing Distances ..." << endl;
			theData.computeDistances();	//calculate distance between vertexes

		}

		//cleanup
		mxDestroyArray(pArr);
		matClose(m_pFileMat);
	}
	catch (char* str) 
	{
		cout << str << endl;
		return false;
	}
	return true;
}