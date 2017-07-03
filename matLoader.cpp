#include "matLoader.h"
#include <vector>
#include <iostream>

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

	//read the main structure
	mxArray *pArr = matGetVariable(m_pFileMat, "G");

	if ((pArr != nullptr) && !mxIsEmpty(pArr) && mxIsStruct(pArr))
	{
		mwSize num = mxGetNumberOfFields(pArr);	//number of fields

																						//reading the 1st field (in this case G.e)
		std::vector<double> v;
		//get the field
		mxArray *pArrToE = mxGetField(pArr, 0, "e");
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

	//cleanup
	mxDestroyArray(pArr);
	matClose(m_pFileMat);
	return true;
}