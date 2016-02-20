#ifndef _DUNGEONPART_H_
#define _DUNGEONPART_H_

#include "demosystem/demosys.h"

#define FLAME_TEXTURES 52

class DungeonPart : public Part {
private:
	//Scene *scene;
	Texture *FlameTex[FLAME_TEXTURES];

	Curve *CamPath[4], *TargPath[2];
	Camera *cam[4];

	Object *Flame[16], *LavaCrust, *ShadowObj[2], *LightRays;
	Object *Floor[3], *Obj, *mobj, *Crystals[5];

	Object *Name, *Fade;
	Texture *NameTex[8];

	dword CamTimeSeg[3][2];

public:

	DungeonPart(GraphicsContext *gc);
	~DungeonPart();

	virtual void MainLoop();
};

#endif	// _DUNGEONPART_H_