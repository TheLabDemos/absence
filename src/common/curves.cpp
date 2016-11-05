#include <cmath>
#include "curves.h"

Curve::Curve() {
	ArcParametrize = false;
	ease_curve = 0;
	Samples = 0;

	SetEaseSampleCount(100);
}

Curve::~Curve() {
	delete [] Samples;
	
}

void Curve::SetArcParametrization(bool state) {
	ArcParametrize = state;
}

#define Param	0
#define ArcLen	1

void Curve::SampleArcLengths() {
	const int SamplesPerSegment = 30;
	SampleCount = GetSegmentCount() * SamplesPerSegment;

	ArcParametrize = false;	// to be able to interpolate with the original values

	Samples = new Vector2[SampleCount];
	Vector3 prevpos;
	float step = 1.0f / (float)(SampleCount-1);
	for(int i=0; i<SampleCount; i++) {
		float t = step * (float)i;
		Vector3 pos = Interpolate(t);
		Samples[i][Param] = t;
		if(!i) {
			Samples[i][ArcLen] = 0.0f;
		} else {
			Samples[i][ArcLen] = (pos - prevpos).Length() + Samples[i-1][ArcLen];
		}
		prevpos = pos;
	}

	// normalize arc lenghts
	float maxlen = Samples[SampleCount-1][ArcLen];
	for(int i=0; i<SampleCount; i++) {
		Samples[i][ArcLen] /= maxlen;
	}

	ArcParametrize = true;
}

int BinarySearch(Vector2 *array, float key, int begin, int end) {
	int middle = begin + ((end - begin)>>1);

	if(array[middle][ArcLen] == key) return middle;
	if(end == begin) return middle;

	if(key < array[middle][ArcLen]) return BinarySearch(array, key, begin, middle);
	if(key > array[middle][ArcLen]) return BinarySearch(array, key, middle+1, end);
	return -1;	// just to make the compiler shut the fuck up
}

float Curve::Parametrize(float t) {
	if(!Samples) SampleArcLengths();

	int samplepos = BinarySearch(Samples, t, 0, SampleCount);
	float par = Samples[samplepos][Param];
	float len = Samples[samplepos][ArcLen];
	if((len - t) < XSmallNumber) return par;

	if(len < t) {
		if(!samplepos) return par;
		float prevlen = Samples[samplepos-1][ArcLen];
		float prevpar = Samples[samplepos-1][Param];
		float p = (t - prevlen) / (len - prevlen);
		return prevpar + (par - prevpar) * p;
	} else {
		if(samplepos >= SampleCount) return par;
		float nextlen = Samples[samplepos+1][ArcLen];
		float nextpar = Samples[samplepos+1][Param];
		float p = (t - len) / (nextlen - len);
		return par + (nextpar - par) * p;
	}

	return par;	// not gonna happen
}


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

float Curve::Ease(float t) {
	if(!ease_curve) return t;

	ease_curve->SetArcParametrization(true);
	float et = ease_curve->Interpolate(t).y;

	return MIN(MAX(et, 0.0f), 1.0f);
}


void Curve::AddControlPoint(const Vector3 &cp) {
	ControlPoints.PushBack(cp);
	delete [] Samples;
	Samples = 0;
}

void Curve::SetEaseCurve(Curve *curve) {
	ease_curve = curve;
}

void Curve::SetEaseSampleCount(int count) {
	ease_sample_count = count;
	ease_step = 1.0f / ease_sample_count;
}


///////////////// B-Spline implementation ////////////////////

int BSpline::GetSegmentCount() const {
	return ControlPoints.Size() - 3;
}

Vector3 BSpline::Interpolate(float t) {

	if(ControlPoints.Size() < 4) return Vector3(0, 0, 0);

	if(ArcParametrize) {
		t = Ease(Parametrize(t));
	}

	// find the appropriate segment of the spline that t lies and calculate the piecewise parameter
	t = (float)(ControlPoints.Size() - 3) * t;
	int seg = (int)t;
	t -= (float)floor(t);
	if(seg >= GetSegmentCount()) {
		seg = GetSegmentCount() - 1;
		t = 1.0f;
	}
	
	ListNode<Vector3> *iter = ControlPoints.Begin();
	for(int i=0; i<seg; i++) iter = iter->next;

	Vector3 Cp[4];
	for(int i=0; i<4; i++) {
        Cp[i] = iter->data;
		iter = iter->next;
	}

	Matrix4x4 BSplineMat(-1, 3, -3, 1, 3, -6, 3, 0, -3, 0, 3, 0, 1, 4, 1, 0);
	BSplineMat.Transpose();
	Vector4 Params(t*t*t, t*t, t, 1);
	Vector4 CpX(Cp[0].x, Cp[1].x, Cp[2].x, Cp[3].x);
	Vector4 CpY(Cp[0].y, Cp[1].y, Cp[2].y, Cp[3].y);
	Vector4 CpZ(Cp[0].z, Cp[1].z, Cp[2].z, Cp[3].z);

	CpX.Transform(BSplineMat);
	CpY.Transform(BSplineMat);
	CpZ.Transform(BSplineMat);

	CpX /= 6.0f;
	CpY /= 6.0f;
	CpZ /= 6.0f;

	Vector3 res;

	res.x = Params.DotProduct(CpX);
	res.y = Params.DotProduct(CpY);
	res.z = Params.DotProduct(CpZ);

	return res;
}

//////////////// Catmull-Rom Spline implementation //////////////////

int CatmullRomSpline::GetSegmentCount() const {
	return ControlPoints.Size() - 1;
}

Vector3 CatmullRomSpline::Interpolate(float t) {

	if(ControlPoints.Size() < 2) return Vector3(0, 0, 0);

	if(ArcParametrize) {
		t = Ease(Parametrize(t));
	}

	// find the appropriate segment of the spline that t lies and calculate the piecewise parameter
	t = (float)(ControlPoints.Size() - 1) * t;
	int seg = (int)t;
	t -= (float)floor(t);
	if(seg >= GetSegmentCount()) {
		seg = GetSegmentCount() - 1;
		t = 1.0f;
	}

	Vector3 Cp[4];
	ListNode<Vector3> *iter = ControlPoints.Begin();
	for(int i=0; i<seg; i++) iter = iter->next;

	Cp[1] = iter->data;
	Cp[2] = iter->next->data;
	
	if(!seg || !iter->prev) {
		Cp[0] = Cp[1];
	} else {
		Cp[0] = iter->prev->data;
	}
	
	if(seg == ControlPoints.Size() - 2) {
		Cp[3] = Cp[2];
	} else {
		Cp[3] = iter->next->next->data;
	}

	Matrix4x4 BSplineMat(-1, 3, -3, 1, 2, -5, 4, -1, -1, 0, 1, 0, 0, 2, 0, 0);
	BSplineMat.Transpose();
	Vector4 Params(t*t*t, t*t, t, 1);
	Vector4 CpX(Cp[0].x, Cp[1].x, Cp[2].x, Cp[3].x);
	Vector4 CpY(Cp[0].y, Cp[1].y, Cp[2].y, Cp[3].y);
	Vector4 CpZ(Cp[0].z, Cp[1].z, Cp[2].z, Cp[3].z);

	CpX.Transform(BSplineMat);
	CpY.Transform(BSplineMat);
	CpZ.Transform(BSplineMat);

	CpX /= 2.0f;
	CpY /= 2.0f;
	CpZ /= 2.0f;

	Vector3 res;

	res.x = Params.DotProduct(CpX);
	res.y = Params.DotProduct(CpY);
	res.z = Params.DotProduct(CpZ);

	return res;
}