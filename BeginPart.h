#ifndef _BEGINPART_H_
#define _BEGINPART_H_

#include "demosystem/demosys.h"

class BeginPart : public Part {
private:
    //Scene *scene;

	Object *Logo1, *Logo2, *Latin1, *Latin2, *Lines[2], *TheLabText[2], *VolumeLogo;
	DirLight *light;
	Camera *cam;

public:

	BeginPart(GraphicsContext *gc);
	~BeginPart();

	void MainLoop();
};


#endif	// _BEGINPART_H_