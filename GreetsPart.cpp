#include "greetspart.h"
#include "nwt/widget.h"

extern Widget *win;

GreetsPart::GreetsPart(GraphicsContext *gc) {
	this->gc = gc;

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);


	SceneLoader::SetNormalFileSaving(false);
	SceneLoader::SetDataPath("data/textures/");
	SceneLoader::LoadScene("data/geometry/greets.3ds", &scene);
	SceneLoader::SetNormalFileSaving(true);

	Parchment = scene->GetObject("Plane01");
	//Scroll1 = scene->GetObject("Object01");
	//Scroll2 = scene->GetObject("Object02");

	// load the flame textures
	for(int i=0; i<52; i++) {
		char num[3];
		itoa(i, num, 10);
		string fname = string("data/textures/flame/flame") + (i < 10 ? string("0") : string("")) + string(num) + string(".jpg");
		FlameTex[i] = gc->texman->LoadTexture(fname.c_str());
	}

	flame = scene->GetObject("Plane02");
	scene->RemoveObject(flame);

	Parchment->SetScaling(9.0f, 1.0f, 1.0f);

	Bottle = scene->GetObject("Bottle");
	scene->RemoveObject(Bottle);

	light = scene->GetLight("Omni01");

    scene->SetShadows(true);
	light->SetShadowCasting(true);
	//scene->GetObject("CHolder")->SetShadowCasting(true);
	scene->GetObject("Candle")->SetShadowCasting(true);
	//scene->GetObject("Bottle01")->SetShadowCasting(true);

	const Light *l[] = {light};
	scene->GetObject("Cylinder01")->CalculateShadows(l, 1);
	scene->GetObject("Cylinder02")->CalculateShadows(l, 1);
	scene->GetObject("Bottle01")->CalculateShadows(l, 1);
	scene->GetObject("Plane01")->CalculateShadows(l, 1);

	scene->SetAmbientLight(Color(0.58f, 0.3f, 0.3f));

	LightPos = light->GetPosition();
	light->SetIntensity(0.5f);
	light->SetAttenuation(0.0f, 0.1f, 0.0f);

}

GreetsPart::~GreetsPart() {
	delete scene;
}

void GreetsPart::MainLoop() {

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);

	Vector3 LightPosDiff(frand(0.005f) - 0.0025f, frand(0.005f) - 0.0025f, frand(0.005f) - 0.0025f);
	light->SetPosition(LightPos + LightPosDiff);

	light->SetIntensity(0.5f + (frand(0.1f) - 0.05f));

	dword msec = timer.GetMilliSec();
	float t = msec / 1000.0f;

	scene->Render();

	Bottle->Render();

	gc->SetLighting(false);
	int ftexnum = (int)((float)msec / 33.35f) % 52;
	gc->SetAlphaBlending(true);
	gc->SetBackfaceCulling(false);
	gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);
	gc->SetTextureStageColor(0, TexBlendSelectArg1, TexArgTexture, TexArgCurrent);
	gc->SetTextureStageAlpha(0, TexBlendSelectArg1, TexArgTexture, TexArgTexture);
	gc->SetMaterial(Material(1.0f, 1.0f, 1.0f));
	gc->SetTextureCoordIndex(0, 0);
	gc->SetTexture(1, 0);
    gc->SetTexture(0, FlameTex[ftexnum]);
	gc->SetWorldMatrix(flame->GetWorldTransform());
	flame->RenderBare();
	gc->SetBackfaceCulling(true);
	gc->SetAlphaBlending(false);
	gc->SetLighting(true);	
}