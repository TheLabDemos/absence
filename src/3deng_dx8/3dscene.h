#ifndef _3DSCENE_H_
#define _3DSCENE_H_

#include <list>
#include "3dengine.h"
#include "camera.h"
#include "lights.h"
#include "objects.h"
#include "curves.h"

struct ShadowVolume {
	TriMesh *shadow_mesh;
	const Light *light;
};

class Scene {
private:
	GraphicsContext *gc;

	Light *lights[8];
	std::list<Camera *> cameras;
	std::list<Object *> objects;
	std::list<ShadowVolume> StaticShadowVolumes;
	std::list<Curve *> curves;
	bool ManageData;

	Camera *ActiveCamera;

	bool Shadows;
	bool LightHalos;
	float HaloSize;

	Color AmbientLight;
	
	bool UseFog;
	Color FogColor;
	float NearFogRange, FarFogRange;
		
public:

	Scene(GraphicsContext *gc = 0);
	~Scene();

	void SetGraphicsContext(GraphicsContext *gc);

	void AddCamera(Camera *cam);
	void AddLight(Light *light);
	void AddObject(Object *obj);
	void AddStaticShadowVolume(TriMesh *mesh, const Light *light);
	void AddCurve(Curve *curve);

	void RemoveObject(const Object *obj);
	void RemoveLight(const Light *light);

	Camera *GetCamera(const char *name);
	Light *GetLight(const char *name);
	Object *GetObject(const char *name);
	Curve *GetCurve(const char *name);

	std::list<Object*> *GetObjectsList();

	void SetActiveCamera(Camera *cam);
	Camera *GetActiveCamera() const;

	void SetShadows(bool enable);
	void SetHaloDrawing(bool enable);
	void SetHaloSize(float size);
	void SetAmbientLight(Color ambient);
	Color GetAmbientLight() const;
	void SetFog(bool enable, Color FogColor = Color(0l), float Near = 0.0f, float Far = 1000.0f);

	// render states
	void SetupLights() const;

	void RenderShadows() const;
	void Render() const;
};
	


#endif	// _3DSCENE_H_