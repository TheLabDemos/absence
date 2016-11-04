#include "textureman.h"
#include "3dengine.h"
#include "d3dx8.h"

using std::string;

template <class KeyType>
unsigned int Hash(const KeyType &key, unsigned long size) {
	string str = (string)key;
	return ((unsigned int)str[0] + (unsigned int)str[1] + (unsigned int)str[2]) % size;
}

TextureManager::TextureManager(GraphicsContext *gc) {
	this->gc = gc;
	notfilecount = 0;

	textures.SetHashFunction(Hash);

	CreateStockTextures();
}

TextureManager::~TextureManager() {}

#define STOCKSIZE	256
#include <cassert>

void TextureManager::CreateStockTextures() {
	Surface *surf;

	// ----- 1/d^2 blob texture -----
	Texture *tmp = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true, true);
	tmp->GetSurfaceLevel(0, &surf);
	
	dword *ptr, offs;
	D3DLOCKED_RECT d3dlock;
	assert(surf->LockRect(&d3dlock, 0, 0) == D3D_OK);
	ptr = (dword*)d3dlock.pBits;
	offs = (d3dlock.Pitch >> 2) - STOCKSIZE;

	Vector2 Center((float)(STOCKSIZE>>1), (float)(STOCKSIZE>>1));

	for(int y=0; y<STOCKSIZE; y++) {
		for(int x=0; x<STOCKSIZE; x++) {
			Vector2 pos((float)x, (float)y);
			float p = 100.0f / (pos - Center).LengthSq();

			Color col(p, p, p, p);
			*ptr++ = col.GetPacked32();
		}
		ptr += offs;
	}

	surf->UnlockRect();

	Texture *Blob = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true);
	gc->D3DDevice->UpdateTexture(tmp, Blob);

	UpdateMipmapChain(Blob);

	AddTexture(Blob, "STOCKTEX_BLOB");
	tmp->Release();

	// ----- chessboard texture -----

	tmp = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true, true);
	tmp->GetSurfaceLevel(0, &surf);
	
	assert(surf->LockRect(&d3dlock, 0, 0) == D3D_OK);
	ptr = (dword*)d3dlock.pBits;
	offs = (d3dlock.Pitch >> 2) - STOCKSIZE;

	for(int y=0; y<STOCKSIZE; y++) {
		for(int x=0; x<STOCKSIZE; x++) {
			int dx = x - (STOCKSIZE>>1);
			int dy = y - (STOCKSIZE>>1);
			if((dx > 0 && dy > 0) || (dx < 0 && dy < 0)) {
				*ptr++ = 0xffffffff;
			} else {
				*ptr++ = 0xff000000;
			}
		}
		ptr += offs;
	}

	surf->UnlockRect();

	Texture *ChessBoard = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true);
	gc->D3DDevice->UpdateTexture(tmp, ChessBoard);

	UpdateMipmapChain(ChessBoard);

	AddTexture(ChessBoard, "STOCKTEX_CHESSBOARD");
	tmp->Release();

	// ----- grid texture -----

	tmp = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true, true);
	tmp->GetSurfaceLevel(0, &surf);
	
	assert(surf->LockRect(&d3dlock, 0, 0) == D3D_OK);
	ptr = (dword*)d3dlock.pBits;
	offs = (d3dlock.Pitch >> 2) - STOCKSIZE;

	for(int y=0; y<STOCKSIZE; y++) {
		for(int x=0; x<STOCKSIZE; x++) {
			if(x == 0 || x == STOCKSIZE-1 || y == 0 || y == STOCKSIZE-1) {
				*ptr++ = 0xffffffff;
			} else {
				*ptr++ = 0xff000000;
			}
		}
		ptr += offs;
	}

	surf->UnlockRect();

	Texture *Grid = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true);
	gc->D3DDevice->UpdateTexture(tmp, Grid);

	UpdateMipmapChain(Grid);

	AddTexture(Grid, "STOCKTEX_GRID");
	tmp->Release();


	// ---------- biased linear falloff ---------
	tmp = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true, true);
	tmp->GetSurfaceLevel(0, &surf);
	
	assert(surf->LockRect(&d3dlock, 0, 0) == D3D_OK);
	ptr = (dword*)d3dlock.pBits;
	offs = (d3dlock.Pitch >> 2) - STOCKSIZE;

	Center = Vector2((float)(STOCKSIZE>>1), (float)(STOCKSIZE>>1));

	for(int y=0; y<STOCKSIZE; y++) {
		for(int x=0; x<STOCKSIZE; x++) {
			Vector2 pos((float)x, (float)y);
			float dist = (pos - Center).Length();
			// f[x] / ((1/b - 2) * (1 - f[x]) + 1)
			float b = 0.3f;
			float p = dist ? ((STOCKSIZE - dist) / STOCKSIZE) / ((1/b-2) * (1-((STOCKSIZE - dist) / STOCKSIZE)) + 1) : 1.0f;
			
			Color col(p, p, p, p);
			*ptr++ = col.GetPacked32();
		}
		ptr += offs;
	}

	surf->UnlockRect();

	Texture *Blofm = CreateTexture(STOCKSIZE, STOCKSIZE, 0, true);
	gc->D3DDevice->UpdateTexture(tmp, Blofm);

	UpdateMipmapChain(Blofm);

	AddTexture(Blofm, "STOCKTEX_BLOFM");
	tmp->Release();
}

void TextureManager::SetGraphicsContext(GraphicsContext *gc) {
	this->gc = gc;
}

Texture *TextureManager::LoadTexture(const char *fname) {
	if(!gc || !fname) return 0;

	Texture *tex;

	Pair<string, Texture*> *p;
	if(!(p = textures.Find(fname))) {
        if(D3DXCreateTextureFromFile(gc->D3DDevice, fname, &tex) != D3D_OK) return 0;
		textures.Insert(fname, tex);
	} else {
		tex = p->val;
	}

	return tex;
}

Texture *TextureManager::CreateTexture(dword x, dword y, dword usage, bool mipmaps, bool local) {
	if(!gc) return 0;
	Texture *tex;
	D3DPOOL pool = local ? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;
	D3DFORMAT fmt = usage == UsageDepthStencil ? gc->ZFormat : D3DFMT_A8R8G8B8;
	if(usage == UsageDepthStencil) pool = D3DPOOL_MANAGED;
	if(gc->D3DDevice->CreateTexture(x, y, (int)(!mipmaps), usage, fmt, pool, &tex) != D3D_OK) return 0;
	return tex;
}

Texture *TextureManager::AddTexture(Texture *tex, const char *name) {
	string fname = name ? name : "not_a_file_" + notfilecount;
	textures.Insert(fname, tex);
	return tex;
}

Texture *TextureManager::AddTexture(const char *fname) {
	return LoadTexture(fname);
}