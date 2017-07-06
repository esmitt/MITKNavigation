#pragma once
class CUtility
{
private:
	static CUtility* m_pInstance;
	// Constructor
	CUtility();
public:
	// Static access method.
	const float RADIUS_DISTANCE = 20;
	static CUtility* getInstance();
	void ind2sub(const size_t *siz, int N, int idx, int *sub);
	int sub2ind(int *siz, int N, int *sub);
};