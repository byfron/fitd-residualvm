#include "CollisionUtils.hpp"
#include <iostream>

namespace CollisionUtils {

	void CalculateInterval(Vec2f Axis, Geometry::Polygon<Vec2f> P, float& min, float& max)
	{
		float d = Axis.dot(P.points[0]);
		min = max = d;
		for(int I = 0; I < P.points.size(); I ++)
		{
			float d = P.points[I].dot(Axis);
			if (d < min)
				min = d;
			else
				if(d > max)
					max = d;
		}
	}
	
	bool AxisSeparatePolygons(Vec2f& Axis, Geometry::Polygon<Vec2f> A, Geometry::Polygon<Vec2f> B)
	{
		float mina, maxa;
		float minb, maxb;

		CalculateInterval(Axis, A, mina, maxa);
		CalculateInterval(Axis, B, minb, maxb);

		if (mina > maxb || minb > maxa)
			return true;

		// find the interval overlap
		float d0 = maxa - minb;
		float d1 = maxb - mina;
		float depth = (d0 < d1)? d0 : d1;

		// convert the separation axis into a push vector (re-normalise
		// the axis and multiply by interval overlap)
		float axis_length_squared = Axis.dot(Axis);

		Axis *= depth / axis_length_squared;
		return false;
	}

	
	Vec2f FindMTD(Vec2f* PushVector, int iNumVectors)
	{
		Vec2f MTD = PushVector[0];
		float mind2 = PushVector[0].dot(PushVector[0]);
		for(int I = 1; I < iNumVectors; I++)
		{
			float d2 = PushVector[I].dot(PushVector[I]);
			if (d2 < mind2)
			{
				mind2 = d2;
				MTD = PushVector[I];
			}
		}
		return MTD;
	}

	bool IntersectMTD(Geometry::Polygon<Vec2f> A, Geometry::Polygon<Vec2f> B, Vec2f& MTD)
	{
		// potential separation axes. they get converted into push
		Vec2f Axis[32];
		// max of 16 vertices per polygon
		int iNumAxis = 0;
		int J,I;
		for(J = A.points.size()-1, I = 0; I < A.points.size(); J = I, I ++)
		{
			Vec2f E = A.points[I] - A.points[J];
			Axis[iNumAxis++] = Vec2f(-E(1), E(0));

			if (AxisSeparatePolygons(Axis[iNumAxis-1], A, B))
				return false;
		}
		for(J = B.points.size()-1, I = 0; I < B.points.size(); J = I, I ++)
		{
			Vec2f E = B.points[I] - B.points[J];
			Axis[iNumAxis++] = Vec2f(-E(1), E(0));

			if (AxisSeparatePolygons (Axis[iNumAxis-1], A, B))
				return false;
		}

		// find the MTD among all the separation vectors
		MTD = FindMTD(Axis, iNumAxis);

		// makes sure the push vector is pushing A away from B
		Vec2f D = A.centroid() - B.centroid();
		if (D.dot(MTD) > 0.0f)
			MTD = -MTD;

		return true;
	}
	
};
