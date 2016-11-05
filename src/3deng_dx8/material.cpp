#include "material.h"

Material::Material() {
	memset(this, 0, sizeof(Material));
	Diffuse.r = Ambient.r = 1.0f;
	Diffuse.g = Ambient.g = 1.0f;
	Diffuse.b = Ambient.b = 1.0f;
	Diffuse.a = Ambient.a = 1.0f;
	Specular.r = Specular.g = Specular.b = 1.0f;
	EnvBlend = 1.0f;
	Alpha = 1.0f;
	HasTransparentTex = false;
}

Material::Material(float r, float g, float b, float a) {

	memset(this, 0, sizeof(Material));
	Diffuse.r = Ambient.r = r;
	Diffuse.g = Ambient.g = g;
	Diffuse.b = Ambient.b = b;
	Diffuse.a = Ambient.a = a;
	Specular.r = Specular.g = Specular.b = 1.0f;
	EnvBlend = 1.0f;
	Alpha = 1.0f;
	HasTransparentTex = false;
}

void Material::SetAmbient(float r, float g, float b) {
	Ambient.r = r;
	Ambient.g = g;
	Ambient.b = b;
	Ambient.a = 1.0f;
}

void Material::SetAmbient(const Color &col) {
	Ambient.r = col.r;
	Ambient.g = col.g;
	Ambient.b = col.b;
	Ambient.a = col.a;
}


void Material::SetDiffuse(float r, float g, float b) {
	Diffuse.r = r;
	Diffuse.g = g;
	Diffuse.b = b;
	Diffuse.a = 1.0f;
}

void Material::SetDiffuse(const Color &col) {
	Diffuse.r = col.r;
	Diffuse.g = col.g;
	Diffuse.b = col.b;
	Diffuse.a = col.a;
}


void Material::SetSpecular(float r, float g, float b) {
	Specular.r = r;
	Specular.g = g;
	Specular.b = b;
	Specular.a = 1.0f;
}

void Material::SetSpecular(const Color &col) {
	Specular.r = col.r;
	Specular.g = col.g;
	Specular.b = col.b;
	Specular.a = col.a;
}

void Material::SetEmissive(float r, float g, float b) {
	Emissive.r = r;
	Emissive.g = g;
	Emissive.b = b;
	Emissive.a = 1.0f;
}

void Material::SetEmissive(const Color &col) {
	Emissive.r = col.r;
	Emissive.g = col.g;
	Emissive.b = col.b;
	Emissive.a = col.a;
}

void Material::SetEnvBlend(float value) {
	EnvBlend = value;
}

void Material::SetSpecularPower(float pow) {
	Power = pow;
	SpecularEnable = pow > 0.0f ? true : false;
}

void Material::SetAlpha(float alpha) {
	Alpha = alpha;
}

void Material::SetTexture(Texture *texture, TextureType type) {
    Maps[type] = texture;	
}