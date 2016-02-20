#ifndef _DEMONPART_H_
#define _DEMONPART_H_

#include "demosystem/demosys.h"

class DemonPart : public Part {
private:
	Camera *cam;
	Curve *curve;

public:
	DemonPart(GraphicsContext *gc);
	~DemonPart();

	void MainLoop();
};

#endif	// _DEMONPART_H_