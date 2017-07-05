#ifndef _MAT_LOADER_H_
#define _MAT_LOADER_H_

//matlab
#include "matlab/mat.h"
#include "matlab/mat.h"
#include "matlab/matrix.h"

#pragma comment(lib, "libmex.lib")
#pragma comment(lib, "libmx.lib")
#pragma comment(lib, "libmat.lib")

#include <string>
#include <vector>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include "datastructure.h"

///Class to handle a Matlab (.mat) files from C++
class CMatLoader
{
private:
	MATFile* m_pFileMat;				//variable to read the file
	std::vector<bool> m_vSkel;	//vector of boolean to store the skeleton
	
public:
	//vtkSmartPointer<vtkPoints> m_vPointsSkel;	//points to store the skeleton
	CMatLoader();
	~CMatLoader();
	bool OpenFile(std::string strFilename, Graph & theData);
};

#endif	//_MAT_LOADER_H_