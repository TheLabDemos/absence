#ifndef _HELLPART_H_
#define _HELLPART_H_

#include "demosystem/demosys.h"

class HellPart : public Part {
private:
	Curve *CamPath;
	Camera *cam, *cam2, *cam3;
	Object *Stones[5];
	Object *Rings[5];
	Light *Thunder;
	Object *ThundEnv[5];
	Object *Credits;
	Object *Blood, *Grail;
	Texture *dbgtex;
	Texture *CredNuc[7];
	Texture *CredAmi[7];
	Texture *CredRaw[7];
	Texture *CredAmv[7];



public:
	HellPart(GraphicsContext *gc);
	~HellPart();

	void MainLoop();
};

#endif	// _HELLPART_H_