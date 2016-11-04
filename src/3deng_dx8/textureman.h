#ifndef _TEXTUREMAN_H_
#define _TEXTUREMAN_H_

#include <string>
#include "hashtable.h"
#include "typedefs.h"
#include "d3d8.h"
#include "3dengtypes.h"

class GraphicsContext;

const dword UsageRenderTarget	= D3DUSAGE_RENDERTARGET;
const dword UsageDepthStencil	= D3DUSAGE_DEPTHSTENCIL;

class TextureManager {
private:
	GraphicsContext *gc;
	HashTable<std::string, Texture*> textures;
	int notfilecount;

	// private copy constructor and assignment op, to prohibit copying
	TextureManager(const TextureManager &tm) {}
	void operator =(const TextureManager &tm) {}

	void CreateStockTextures();

public:
	TextureManager(GraphicsContext *gc = 0);
	~TextureManager();

	void SetGraphicsContext(GraphicsContext *gc);

	Texture *LoadTexture(const char *fname);
	Texture *CreateTexture(dword x, dword y, dword usage, bool mipmaps, bool local=false);
	Texture *AddTexture(Texture *tex, const char *name=0);
	Texture *AddTexture(const char *fname);
};

#endif	// _TEXTUREMAN_H_