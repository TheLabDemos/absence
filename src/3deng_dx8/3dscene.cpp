#include <string>
#include "3dscene.h"

using std::string;

Scene::Scene(GraphicsContext *gc) {
	this->gc = gc;

	ActiveCamera = 0;
	Shadows = false;
	LightHalos = false;
	HaloSize = 10.0f;
	UseFog = false;

	memset(lights, 0, 8 * sizeof(Light*));

	AmbientLight = Color(0.0f, 0.0f, 0.0f);
	ManageData = true;
}

Scene::~Scene() {

	if(ManageData) {
		std::list<Object*>::iterator obj = objects.begin();
		while(obj != objects.end()) {
			delete *obj++;
		}

		std::list<Camera*>::iterator cam = cameras.begin();
		while(cam != cameras.end()) {
			delete *cam++;
		}

		for(int i=0; i<8; i++) {
			delete lights[i];
		}

		std::list<Curve*>::iterator citer = curves.begin();
		while(citer != curves.end()) {
			delete *citer++;
		}

		std::list<ShadowVolume>::iterator sv = StaticShadowVolumes.begin();
		while(sv != StaticShadowVolumes.end()) {
			delete (*sv++).shadow_mesh;
		}
	}

}

void Scene::SetGraphicsContext(GraphicsContext *gc) {
	this->gc = gc;
}

void Scene::AddCamera(Camera *cam) {
	cameras.push_back(cam);
	if(!ActiveCamera) ActiveCamera = cam;
}

void Scene::AddLight(Light *light) {
	for(int i=0; i<8; i++) {
		if(!lights[i]) {
			lights[i] = light;
			break;
		}
	}
}

void Scene::AddObject(Object *obj) {
	if(obj->material.Alpha < 1.0f) {
        objects.push_back(obj);
	} else {
		objects.push_front(obj);
	}
}

void Scene::AddStaticShadowVolume(TriMesh *mesh, const Light *light) {
	ShadowVolume svol;
	svol.shadow_mesh = mesh;
	svol.light = light;
	StaticShadowVolumes.push_back(svol);
}

void Scene::AddCurve(Curve *curve) {
	curves.push_back(curve);
}


void Scene::RemoveObject(const Object *obj) {
	std::list<Object *>::iterator iter = objects.begin();
	while(iter != objects.end()) {
		if(obj == *iter) {
			objects.erase(iter);
			return;
		}
		iter++;
	}
}

void Scene::RemoveLight(const Light *light) {
	for(int i=0; i<8; i++) {
		if(light = lights[i]) {
			lights[i] = 0;
			return;
		}
	}
}


Camera *Scene::GetCamera(const char *name) {
	std::list<Camera *>::iterator iter = cameras.begin();
	while(iter != cameras.end()) {
		if(!strcmp((*iter)->name.c_str(), name)) return *iter;
		iter++;
	}
	return 0;
}

Light *Scene::GetLight(const char *name) {
	for(int i=0; i<8; i++) {
		if(!strcmp(lights[i]->name.c_str(), name)) return lights[i];
	}
	return 0;
}

Object *Scene::GetObject(const char *name) {
	std::list<Object *>::iterator iter = objects.begin();
	while(iter != objects.end()) {
		if(!strcmp((*iter)->name.c_str(), name)) return *iter;
		iter++;
	}
	return 0;
}

Curve *Scene::GetCurve(const char *name) {
	std::list<Curve *>::iterator iter = curves.begin();
	while(iter != curves.end()) {
		if(!strcmp((*iter)->name.c_str(), name)) return *iter;
		iter++;
	}
	return 0;
}


std::list<Object*> *Scene::GetObjectsList() {
	return &objects;
}


void Scene::SetActiveCamera(Camera *cam) {
	ActiveCamera = cam;
}

Camera *Scene::GetActiveCamera() const {
	return ActiveCamera;
}

void Scene::SetShadows(bool enable) {
	Shadows = enable;
}

void Scene::SetHaloDrawing(bool enable) {
	LightHalos = enable;
}

void Scene::SetHaloSize(float size) {
	HaloSize = size;
}

void Scene::SetAmbientLight(Color ambient) {
	AmbientLight = ambient;
}

Color Scene::GetAmbientLight() const {
	return AmbientLight;
}

void Scene::SetFog(bool enable, Color FogColor, float Near, float Far) {
	UseFog = enable;
	if(enable) {
		this->FogColor = FogColor;
		NearFogRange = Near;
		FarFogRange = Far;
	}
}


void Scene::SetupLights() const {
	int LightIndex = 0;
	for(int i=0; i<8; i++) {
		if(lights[i]) {
			lights[i]->SetLight(LightIndex++, gc);
		}
	}
	gc->D3DDevice->LightEnable(LightIndex, false);
}


void Scene::RenderShadows() const {
	
	for(int i=0, slight=0; i<8; i++) {
		if(!lights[i] || !lights[i]->GetShadowCasting()) continue;

		// disable shadow casting light and render the scene (first pass)
		gc->SetAlphaBlending(true);
		gc->D3DDevice->LightEnable(i, false);
		std::list<Object*>::const_iterator iter = objects.begin();
		while(iter != objects.end()) {
			Object *obj = *iter++;
			obj->Render();
		}
		gc->D3DDevice->LightEnable(i, true);
		gc->SetAlphaBlending(false);

		// shadow volume front faces
		gc->SetZWrite(false);
		gc->SetColorWrite(false, false, false, false);
		gc->SetLighting(false);
	
		gc->SetStencilBuffering(true);
		gc->SetStencilFunc(CMP_ALWAYS);
		gc->SetStencilOp(SOP_KEEP, SOP_KEEP, SOP_INC);

		iter = objects.begin();
		while(iter != objects.end()) {
			Object *obj = *iter++;
			TriMesh *mesh = obj->GetShadowVolume(slight);
			if(mesh) {
				gc->SetWorldMatrix(obj->GetWorldTransform());
				gc->Draw(const_cast<Vertex*>(mesh->GetVertexArray()), mesh->GetVertexCount());

				// back faces pass
				//gc->SetFrontFace(CounterClockwise);
				//gc->SetStencilOp(SOP_KEEP, SOP_KEEP, SOP_DEC);

				//gc->Draw(const_cast<Vertex*>(mesh->GetVertexArray()), mesh->GetVertexCount());
			}
		}

		// static shadow volumes
		gc->SetWorldMatrix(Matrix4x4());
		std::list<ShadowVolume>::const_iterator shadow_iter = StaticShadowVolumes.begin();
		while(shadow_iter != StaticShadowVolumes.end()) {
			if(shadow_iter->light == lights[i]) {
				gc->Draw(const_cast<Vertex*>(shadow_iter->shadow_mesh->GetVertexArray()), shadow_iter->shadow_mesh->GetVertexCount());
			}
			shadow_iter++;
		}

		// back faces pass
		gc->SetFrontFace(CounterClockwise);
		gc->SetStencilOp(SOP_KEEP, SOP_KEEP, SOP_DEC);

		iter = objects.begin();
		while(iter != objects.end()) {
			Object *obj = *iter++;
			TriMesh *mesh = obj->GetShadowVolume(slight);
			if(mesh) {
				gc->SetWorldMatrix(obj->GetWorldTransform());
				gc->Draw(const_cast<Vertex*>(mesh->GetVertexArray()), mesh->GetVertexCount());
			}
		}

		// static shadow volumes
		gc->SetWorldMatrix(Matrix4x4());
		shadow_iter = StaticShadowVolumes.begin();
		while(shadow_iter != StaticShadowVolumes.end()) {
			if(shadow_iter->light == lights[i]) {
				gc->Draw(const_cast<Vertex*>(shadow_iter->shadow_mesh->GetVertexArray()), shadow_iter->shadow_mesh->GetVertexCount());
			}
			shadow_iter++;
		}

		gc->SetFrontFace(Clockwise);

		gc->SetLighting(true);
		gc->SetZWrite(true);
		gc->SetColorWrite(true, true, true, true);

		gc->SetStencilOp(SOP_KEEP, SOP_KEEP, SOP_KEEP);
		gc->SetStencilFunc(CMP_EQUAL);
		gc->SetStencilReference(0);

		gc->SetLighting(true);
		//SetupLights();

		slight++;
	}
}

void Scene::Render() const {
	gc->SetAmbientLight(AmbientLight);

	// set camera
	if(!ActiveCamera) return;
	ActiveCamera->CreateCameraMatrix();
	gc->SetViewMatrix(ActiveCamera->GetCameraMatrix());

	// set projection matrix
	float NearClip, FarClip;
	Matrix4x4 ProjMat;
	ActiveCamera->GetClippingPlanes(&NearClip, &FarClip);
	CreateProjectionMatrix(&ProjMat, ActiveCamera->GetFOV(), 1.3333333f, NearClip, FarClip);
	gc->SetProjectionMatrix(ProjMat);	

	SetupLights();

	// render shadows
	if(Shadows) {

		// make array of shadow-casting lights
		Light *ShadowCasters[8];
		Light **lptr = ShadowCasters;
		for(int i=0; i<8; i++) {
			if(lights[i] && lights[i]->GetShadowCasting()) {
				*lptr++ = lights[i];
			}
		}
		int ShadowCasterCount = (int)(lptr - ShadowCasters);

		std::list<Object *>::const_iterator iter = objects.begin();
		while(iter != objects.end()) {
			Object *obj = *iter++;
			
			if(obj->GetShadowCasting()) {
                obj->CalculateShadows((const Light**)ShadowCasters, ShadowCasterCount);
			}
		}

		RenderShadows();
	}

	/* RESTORATION: render transparent objects last.
	 * I don't know how this used to work, maybe the earlier engine version
	 * used to split them during loading? no idea.
	 */
	// render opaque objects
	std::list<Object *>::const_iterator iter = objects.begin();
	while(iter != objects.end()) {
		Object *obj = *iter++;

		if(obj->material.Alpha > 0.991f && !obj->material.HasTransparentTex) {
			obj->Render();
		}
	}
	// render transparent objects
	iter = objects.begin();
	while(iter != objects.end()) {
		Object *obj = *iter++;

		if(!(obj->material.Alpha > 0.991f && !obj->material.HasTransparentTex)) {
			obj->SetWriteZBuffer(false);
			obj->Render();
		}
	}

	if(Shadows) {
		gc->SetStencilBuffering(false);
		gc->SetStencilFunc(CMP_ALWAYS);
	}

	if(LightHalos) {
		for(int i=0; i<8; i++) {
			if(lights[i]) lights[i]->Draw(gc, HaloSize);
		}
	}
}