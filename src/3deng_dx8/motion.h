#ifndef _MOTION_H_
#define _MOTION_H_

#include "n3dmath.h"
#include "curves.h"

class MotionController {
private:
	Matrix4x4 MotionXForm;
	bool FollowPath;
	float ScaleInputTrans, ScaleInputRot, ScaleInputScale;
	
	float (*XTranslation)(float);
	float (*YTranslation)(float);
	float (*ZTranslation)(float);
	float (*XRotation)(float);
	float (*YRotation)(float);
	float (*ZRotation)(float);
	float (*XScaling)(float);
	float (*YScaling)(float);
	float (*ZScaling)(float);

public:

	MotionController();
	
	void SetTranslation(float (*xtrans)(float), float (*ytrans)(float), float (*ztrans)(float), float ScaleInput = 1.0f);
	void SetRotation(float (*xrot)(float), float (*yrot)(float), float (*zrot)(float), float ScaleInput = 1.0f);
	void SetScaling(float (*xscale)(float), float (*yscale)(float), float (*zscale)(float), float ScaleInput = 1.0f);
	void SetPath(const Curve &path);

	const Matrix4x4 &GetTransformation(float t);
};



#endif	// _MOTION_H_