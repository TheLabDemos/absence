#ifndef _TREEPART_H_
#define _TREEPART_H_

#include "demosystem/demosys.h"
#include "3deng_dx8/particles.h"

class TreePart : public Part {
private:
	//Scene *scene;
	ParticleSystem *leaves, *WispParticles[4];
	Texture *WispParticle, *LeavesParticle;
	Camera *cam, *dummy[3];
	Curve *CamPath, *TargPath, *WispPath[3];
	Light *lights[4];

	Object *Trees, *Moon, *Stars;
public:

	TreePart(GraphicsContext *gc);
	~TreePart();

	void MainLoop();
};

#endif	// _TREEPART_H_