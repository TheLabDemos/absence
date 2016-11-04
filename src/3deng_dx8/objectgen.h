#ifndef _OBJECTGEN_H_
#define _OBJECTGEN_H_

#include "3dgeom.h"

namespace ObjGen {
	void CreatePlane(GraphicsContext *gc, float size, dword subdivisions, TriMesh **mesh, byte LODLevels = 0);
	void CreateCube(GraphicsContext *gc, float size, TriMesh **mesh, byte LODLevels = 0);
	void CreateCylinder(float radius, float height, int sides, int hsegs, TriMesh **mesh, byte LODLevels = 0);
	void CreateSphere(float radius, float segments, bool caps, TriMesh **mesh, byte LODLevels = 0);
}

#endif	// _OBJECTGEN_H_