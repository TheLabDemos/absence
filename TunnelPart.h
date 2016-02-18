#ifndef _TUNNELPART_H_
#define _TUNNELPART_H_

#include "demosystem/demosys.h"

class TunnelPart : public Part {
private:
	Curve *CamPath, *TargPath;
	Camera *cam;

public:

	TunnelPart(GraphicsContext *gc);
	~TunnelPart();

	void MainLoop();
};

#endif	// _TUNNELPART_H_