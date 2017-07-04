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
bool CMatLoader::OpenFile(std::string strFilename)
{
	m_pFileMat = matOpen(strFilename.c_str(), "r");
	if (m_pFileMat == nullptr)	//for any reason, can not open it
		return false;

	/////////////////////////////////////////////////
	//read the main structure
	/////////////////////////////////////////////////
	mxArray *pArr = matGetVariable(m_pFileMat, "G");

	if ((pArr != nullptr) && !mxIsEmpty(pArr) && mxIsStruct(pArr))
	{
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
	}

	/////////////////////////////////////////////////
	//read the skel
	/////////////////////////////////////////////////
	mxArray *pArrToSkel = matGetVariable(m_pFileMat, "skel");
	if ((pArrToSkel != nullptr) && !mxIsEmpty(pArrToSkel) && mxIsLogical(pArrToSkel))
	{
		mwSize iDimSkel = mxGetNumberOfElements(pArrToSkel);
		bool *prLog = mxGetLogicals(pArrToSkel);

		size_t nDim = mxGetNumberOfDimensions(pArrToSkel);	//number of dimensions 2D, 3D (i.e. 2, 3)
		const size_t* dime = mxGetDimensions(pArrToSkel);		//get the array of size nDim with the sizes
		
		m_vPointsSkel =	vtkSmartPointer<vtkPoints>::New();
		int* values = new int[nDim];
		
		//store only all true-logic points
		for (size_t i = 0; i < iDimSkel; ++i)
		{
			if (prLog[i])	//the only who is true/remarkable
			{
				CUtility::getInstance()->ind2sub(dime, nDim, i, values);	//function from index to [x, y, z]
				m_vPointsSkel->InsertNextPoint(values[1], values[0], values[2]);
			}
		}
		delete values;
	}

	//cleanup
	mxDestroyArray(pArr);
	matClose(m_pFileMat);
	return true;
}