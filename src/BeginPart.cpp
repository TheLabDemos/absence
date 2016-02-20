#include "beginpart.h"

BeginPart::BeginPart(GraphicsContext *gc) {
	this->gc = gc;

	this->gc = gc;

	SceneLoader::SetNormalFileSaving(true);
	SceneLoader::SetDataPath("data/textures/");
	SceneLoader::LoadScene("data/geometry/begin.3ds", &scene);

	light = new DirLight(Vector3(0.0f, 0.0f, 1.0f));
	light->SetIntensity(1.5f);
	scene->AddLight(light);

	// get stuff
	Logo1 = scene->GetObject("LogoT");
	scene->RemoveObject(Logo1);

	Logo2 = scene->GetObject("LogoB");
	scene->RemoveObject(Logo2);

	VolumeLogo = scene->GetObject("VolumeLogo");
	scene->RemoveObject(VolumeLogo);
	
	Latin1 = scene->GetObject("Latin1");	
	Latin2 = scene->GetObject("Latin2");	
	Lines[0] = scene->GetObject("Lines1");
	Lines[1] = scene->GetObject("Lines2");
	TheLabText[0] = scene->GetObject("LabText1");
	TheLabText[1] = scene->GetObject("LabText2");

	scene->RemoveObject(scene->GetObject("Plane01"));

	gc->SetTextureTransformState(0, TexTransform2D);
}

BeginPart::~BeginPart() {
	delete scene;
}

#define psin(x) (sinf(x) / 2.0f + 0.5f)
#define pcos(x) (cosf(x) / 2.0f + 0.5f)

void BeginPart::MainLoop() {
	dword msec = timer.GetMilliSec();
	float t = (float)msec / 1000.0f;

	const float StartCycle = 2.5f;
	const float EndCycle = 5.0f;
	const float StartFade = 9.0f;


	Latin1->SetTextureMatrix(Matrix3x3(1,0,0, 0,1,0, 0,t/4,1));
	Latin2->SetTextureMatrix(Matrix3x3(1,0,0, 0,1,0, 0,t/2,1));
	Lines[0]->SetTextureMatrix(Matrix3x3(1,0,0, 0,1,0, t*2.0f,0,1));
	Lines[1]->SetTextureMatrix(Matrix3x3(1,0,0, 0,1,0, t/2.0f,0,1));
	TheLabText[0]->SetTextureMatrix(Matrix3x3(1,0,0, 0,1,0, t,0,1));
	TheLabText[1]->SetTextureMatrix(Matrix3x3(1,0,0, 0,1,0, t/3,0,1));

	gc->Clear(0);
	gc->ClearZBufferStencil(1.0f, 0);

	gc->SetZBuffering(false);

	scene->Render();

	gc->SetAlphaBlending(true);
	gc->SetTextureStageColor(0, TexBlendModulate, TexArgCurrent, TexArgTexture);
	gc->SetTextureStageAlpha(0, TexBlendModulate, TexArgCurrent, TexArgTexture);
	gc->SetBlendFunc(BLEND_ONE, BLEND_ONE);

	Matrix3x3 TexMat;
	
	// render shit
	Logo1->SetTranslation(0.0f, max(0.8f - t, 0) * 50.0f, 0.0f);
	Logo2->SetTranslation(0.0f, -max(0.8f - t, 0) * 50.0f, 0.0f);
	
	if(t > StartCycle) {
        Logo1->material.Diffuse.r += (t-StartCycle) * 0.005f;
		Logo1->material.Diffuse.g -= (t-StartCycle) * 0.005f;
		Logo1->material.Diffuse.b -= (t-StartCycle) * 0.005f;
		if(Logo1->material.Diffuse.r > 0.5f) Logo1->material.Diffuse.r = 0.5f;
		if(Logo1->material.Diffuse.g < 0.0f) Logo1->material.Diffuse.g = 0.0f;
		if(Logo1->material.Diffuse.b < 0.0f) Logo1->material.Diffuse.b = 0.0f;

		Logo2->material.Diffuse.r += (t-StartCycle) * 0.005f;
		Logo2->material.Diffuse.g -= (t-StartCycle) * 0.005f;
		Logo2->material.Diffuse.b -= (t-StartCycle) * 0.005f;
		if(Logo2->material.Diffuse.r > 0.5f) Logo2->material.Diffuse.r = 0.5f;
		if(Logo2->material.Diffuse.g < 0.0f) Logo2->material.Diffuse.g = 0.0f;
		if(Logo2->material.Diffuse.b < 0.0f) Logo2->material.Diffuse.b = 0.0f;
	}
	
	gc->SetMaterial(Logo1->material);
	gc->SetTexture(0, Logo1->material.Maps[TextureMap]);
	
	gc->SetWorldMatrix(Logo1->GetWorldTransform());
	Logo1->RenderBare();

	gc->SetWorldMatrix(Logo2->GetWorldTransform());
	Logo2->RenderBare();

	if(t > EndCycle) {
		Color color(Logo1->material.Diffuse.r, Logo1->material.Diffuse.g, Logo1->material.Diffuse.b);
		VolumeLogo->material.SetDiffuse(color);
		gc->SetTexture(0, VolumeLogo->material.Maps[TextureMap]);
		
		float xoffs = sinf(t/2.0f) * 4.0f;
		float yoffs = sinf(t) * 1.5f;//-(t - 2.0f*EndCycle);
		for(int i=0; i<min(10, (int)((t-EndCycle)*30.0f)); i++) {
			Matrix4x4 mat;
			mat.SetTranslation(xoffs * ((float)i * 0.5f), yoffs * ((float)i * 0.5f), 0.0f);

			float ScaleFactor = 1.0f + (float)i * 0.2f;
			Matrix4x4 ScaleMat;
			ScaleMat.SetScaling(ScaleFactor, ScaleFactor, ScaleFactor);

			VolumeLogo->material.Diffuse.r -= 0.08f;
			VolumeLogo->material.Diffuse.g -= 0.08f;
			VolumeLogo->material.Diffuse.b -= 0.08f;

			gc->SetWorldMatrix(VolumeLogo->GetWorldTransform() * mat * ScaleMat);
			VolumeLogo->RenderBare();
		}
	}

	Material OrigMat = VolumeLogo->material;
	VolumeLogo->material = Material(0.0f, 0.0f, 0.0f, max((t - StartFade) / 2.0f, 0.0f));
	VolumeLogo->Render();
	VolumeLogo->material = OrigMat;

	gc->SetZBuffering(true);
	gc->SetAlphaBlending(false);
}