#include "motion.h"

MotionController::MotionController() {
	memset(this, 0, sizeof(MotionController));
}

void MotionController::SetTranslation(float (*xtrans)(float), float (*ytrans)(float), float (*ztrans)(float), float ScaleInput) {
	XTranslation = xtrans;
	YTranslation = ytrans;
	ZTranslation = ytrans;
	ScaleInputTrans = ScaleInput;
}

void MotionController::SetRotation(float (*xrot)(float), float (*yrot)(float), float (*zrot)(float), float ScaleInput) {
	XRotation = xrot;
	YRotation = yrot;
	ZRotation = zrot;
	ScaleInputRot = ScaleInput;
}

void MotionController::SetScaling(float (*xscale)(float), float (*yscale)(float), float (*zscale)(float), float ScaleInput) {
	XScaling = xscale;
	YScaling = yscale;
	ZScaling = zscale;
	ScaleInputScale = ScaleInput;
}

void MotionController::SetPath(const Curve &path) {
	FollowPath = true;
	// TODO
}

const Matrix4x4 &MotionController::GetTransformation(float t) {
	MotionXForm.ResetIdentity();

	if(FollowPath) {
		// TODO
	} else {
		float tt = t * ScaleInputTrans;
		float tr = t * ScaleInputRot;
		float ts = t * ScaleInputScale;
        MotionXForm.Scale(XScaling ? XScaling(tt) : 0, YScaling ? YScaling(tt) : 0, ZScaling ? ZScaling(tt) : 0);
		MotionXForm.Rotate(XRotation ? XRotation(tr) : 0, YRotation ? YRotation(tr) : 0, ZRotation ? ZRotation(tr) : 0);
		MotionXForm.Translate(XTranslation ? XTranslation(ts) : 0, YTranslation ? YTranslation(ts) : 0, ZTranslation ? ZTranslation(ts) : 0);
	}

	return MotionXForm;
}

	
