#ifndef _GREETSPART_H_
#define _GREETSPART_H_

#include "demosystem/demosys.h"

class GreetsPart : public Part {
private:
	Camera *cam;
	Object *Parchment, *Scroll1, *Scroll2;
	Texture *FlameTex[52];
	Object *flame, *Bottle;

	Light *light;
	Vector3 LightPos;
    

public:
	GreetsPart(GraphicsContext *gc);
	~GreetsPart();

	void MainLoop();
};

#endif	// _GREETSPART_H_