#ifndef _MAT_LOADER_H_
#define _MAT_LOADER_H_

//matlab
#include "matlab/mat.h"
#pragma comment(lib, "libmex.lib")
#pragma comment(lib, "libmx.lib")
#pragma comment(lib, "libmat.lib")

#include <string>
#include <vector>

///Class to handle a Matlab (.mat) files from C++
class CMatLoader
{
private:
	MATFile* m_pFileMat;	//
public:
	CMatLoader();
	~CMatLoader();
	bool OpenFile(std::string strFilename);
};

#endif	//_MAT_LOADER_H_