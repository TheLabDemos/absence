#ifndef _3DENGTYPES_H_
#define _3DENGTYPES_H_

#include "d3d8.h"
#include "typedefs.h"

typedef IDirect3DSurface8 Surface;
typedef IDirect3DTexture8 Texture;
typedef IDirect3DVertexBuffer8 VertexBuffer;
typedef IDirect3DIndexBuffer8 IndexBuffer;

#define FixedFunction VertexFormat

//const dword VertexFormat = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX4;
//const dword VertexFormat = D3DFVF_XYZ | D3DFVF_XYZB1 | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX4;
const dword VertexFormat = D3DFVF_XYZB2 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX4;
const D3DFORMAT IndexFormat = D3DFMT_INDEX16;
const dword IndexSize = 2;
typedef uint16 Index;


#endif	// _3DENGTYPES_H_