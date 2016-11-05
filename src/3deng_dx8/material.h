#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <string>
#include <vector>
#include "d3d8.h"
#include "3dengtypes.h"
#include "color.h"

enum TextureType { 
	TextureMap = 0,
	DetailMap,
	OpacityMap, 
	LightMap, 
	BumpMap, 
	EnvironmentMap,
	SpecularMap
};

const int NumberOfTextureTypes = 7;

class Material : public D3DMATERIAL8 {
public:
	std::string name;
	Texture *Maps[NumberOfTextureTypes];
	float EnvBlend, BumpIntensity;
	float Alpha;
	bool SpecularEnable;
	bool HasTransparentTex;	// RESTORATION: hack

	Material();
	Material(float r, float g, float b, float a=1.0f);

	void SetAmbient(float r, float g, float b);
	void SetAmbient(const Color &col);
	void SetDiffuse(float r, float g, float b);
	void SetDiffuse(const Color &col);
	void SetSpecular(float r, float g, float b);
	void SetSpecular(const Color &col);
	void SetEmissive(float r, float g, float b);
	void SetEmissive(const Color &col);
	void SetSpecularPower(float pow);
	void SetAlpha(float alpha);
	void SetEnvBlend(float value);	

	void SetTexture(Texture *texture, TextureType type);
};


#endif	// _MATERIAL_H_