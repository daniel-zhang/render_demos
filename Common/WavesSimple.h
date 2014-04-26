// Generate waves
#ifndef WAVES_SIMPLE_H
#define WAVES_SIMPLE_H

#include <Windows.h>
#include <xnamath.h>

class WavesSimple
{
public:
	WavesSimple();
	~WavesSimple();

	// Getters
	UINT rowCount() const;
	UINT columnCount() const;
	UINT vertexCount() const;
	UINT triangleCount() const;

	const XMFLOAT3& operator[](int i) const
	{
		return mCurrSolution[i];
	}

	void init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void update(float dt);
	void disturb(UINT i, UINT j, float magnitude);

private:
	UINT mNumRows;
	UINT mNumCols;
	UINT mNumVertices;
	UINT mNumTriangles;

	float mK1, mK2, mK3;
	float mTimeStep;
	float mSpatialStep;


	XMFLOAT3* mPrevSolution;
	XMFLOAT3* mCurrSolution;;

};

#endif