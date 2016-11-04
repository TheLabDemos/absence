#ifndef _SCENELOADER_H_
#define _SCENELOADER_H_

#include "3deng_dx8/objects.h"
#include "3deng_dx8/3dscene.h"
#include "3deng_dx8/material.h"

namespace SceneLoader {
	void SetGraphicsContext(GraphicsContext *gfx);
	void SetDataPath(const char *path);
	void SetNormalFileSaving(bool enable);

	bool LoadObject(const char *fname, const char *ObjectName, Object **obj);
	bool LoadScene(const char *fname, Scene **scene);
	bool LoadMaterials(const char *fname, Material **materials);
}

#endif	// _SCENELOADER_H_