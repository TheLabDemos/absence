#ifndef _CURVES_H_
#define _CURVES_H_

#include <string>
#include "n3dmath.h"
#include "linkedlist.h"

class Curve {
protected:
	LinkedList<Vector3> ControlPoints;
	Vector2 *Samples;	// used for parametrizing by arc length
	int SampleCount;
	bool ArcParametrize;

	Curve *ease_curve;	// ease in/out curve (1D, x&z discarded)
	int ease_sample_count, ease_step;
	
	void SampleArcLengths();
	float Parametrize(float t);
	float Ease(float t);

public:
	std::string name;

	Curve();
	~Curve();
	virtual void AddControlPoint(const Vector3 &cp);

	virtual int GetSegmentCount() const = 0;
	virtual void SetArcParametrization(bool state);
	virtual void SetEaseCurve(Curve *curve);
	virtual void SetEaseSampleCount(int count);

	virtual Vector3 Interpolate(float t) = 0;
};

class BSpline : public Curve {
public:
	virtual int GetSegmentCount() const;
	virtual Vector3 Interpolate(float t);	
};

class CatmullRomSpline : public Curve {
public:
	virtual int GetSegmentCount() const;
	virtual Vector3 Interpolate(float t);
};


#endif	// _CURVES_H_