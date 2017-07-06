#include "utility.h"
#include <math.h>
#include <vector>
#include <iostream>

// Null, because instance will be initialized on demand.
CUtility* CUtility::m_pInstance = 0;

CUtility* CUtility::getInstance()
{
	if (m_pInstance == 0)
	{
		m_pInstance = new CUtility();
	}

	return m_pInstance;
}

CUtility::CUtility()
{}

/// Determines the equivalent subscript values corresponding to the absolute index dimension of a multidimensional array.
/// @param siz size of the N-dimensional matrix
/// @param N the dimensions of the matrix
/// @param idx index in linear format
/// @param sub the output - subscript values written into an N - dimensional integer array (created by caller)
void CUtility::ind2sub(const size_t *siz, int N, int idx, int *sub)
{
	// Calculate the cumulative product of the size vector
	std::vector<int> cumProd(N);
	int nextProd = 1;
	for (int k = 0; k < N; k++)
	{
		cumProd[k] = nextProd;
		nextProd *= siz[k];
	}

	// Validate range of linear index
	int maxInd = cumProd[N - 1] * siz[N - 1] - 1;	//assuming that fit into a int data type
	if ((idx < 0) || (idx > maxInd))
	{
		std::cout << "bad linear index maxsum::ind2sub" << std::endl;	//some kind of message
		return;
	}
	
	//the computation
	int remainder = idx;
	for (int k = N - 1; k >= 0; k--)
	{
		sub[k] = remainder / cumProd[k];
		remainder = remainder % cumProd[k];
	}
}

//*********************************************
/*!
\fn				int sub2ind(int *siz, int N, int *sub)
\brief			This function determines the equivalent index value corresponding
to the subscript values of a multidimensional array.

\author			Kriti Sen Sharma
\date			2011-05-04
\param[in]	siz		size of the N-dimensional matrix
\param[in]	N		the dimensions of the matrix
\param[out]	sub		subscripts stored in an N-dimensional array
\return		(integer) the linear index corresponding to the subscript

\details			Example for a 2-D array of size [3, 5] : siz[] = {3, 5}; N = 2
- if subscript is (0, 0), linear index is 0
- if subscript is (0, 1), linear index is 1
- if subscript is (1, 0), linear index is 5
- if subscript is (2, 4), linear index is 14

*/
int CUtility::sub2ind(int *siz, int N, int *sub)
{
	int idx = 0;
	if (0)
		idx = sub[0] * siz[3] * siz[2] * siz[1]
		+ sub[1] * siz[3] * siz[2]
		+ sub[2] * siz[3]
		+ sub[3];
	else
	{
		for (int i = 0; i < N; i++)
		{
			int prod = 1;
			for (int j = N - 1; j > i; j--)
				prod *= siz[j];
			idx += sub[i] * prod;
		}
	}
	return idx;
}

//template<class SizIt, class SubIt> ValIndex
//sub2ind(SizIt sizFirst, SizIt sizEnd, SubIt subFirst, SubIt subEnd)
//{
//	ValIndex skipSize = 1;
//	ValIndex result = 0;
//
//	SizIt siz = sizFirst;
//	SubIt sub = subFirst;
//
//	while ((siz != sizEnd) && (sub != subEnd))
//	{
//		if ((0>*sub) || (*siz <= *sub))
//		{
//			throw OutOfRangeException("bad subindex", "maxsum::sub2ind");
//		}
//		result += (*sub) * skipSize;
//		skipSize *= *siz;
//
//		++siz;
//		++sub;
//	}
//
//	return result;
//
//} // sub2ind