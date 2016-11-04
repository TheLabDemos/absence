#include <fstream>
#include <string>
#include <cmath>
#include <cassert>
#include "d3dx8.h"
#include "3dengine.h"
#include "exceptions.h"
#include "3dgeom.h"
#include "lights.h"

// local helper functions
ColorDepth GetColorDepthFromPixelFormat(D3DFORMAT fmt);
D3DFORMAT GetPixelFormatFromColorDepth(ColorDepth cd, bool noalpha=false);
D3DFORMAT GetDepthStencilFormat(int depthbits, bool stencil);

///////////// Graphics Context Member Functions /////////////////

GraphicsContext::GraphicsContext() {
	D3DDevice = 0;
	BackfaceCulling = true;
}

///////////////////////////////////
// ----==( SetDefaultStates )==----
// (Public Member Function)
// Sets the default render states
///////////////////////////////////
void GraphicsContext::SetDefaultStates() {
	D3DDevice->SetRenderState(D3DRS_LOCALVIEWER, true);
	SetPrimitiveType(TriangleList);
	SetBackfaceCulling(true);
	SetFrontFace(Clockwise);
	SetLighting(true);
	SetColorVertex(false);
	SetAmbientLight(0.0f);
	SetMipMapping(true);
	SetTextureFiltering(BilinearFiltering);
	SetBillboarding(false);
	SetSpecular(true);

	Matrix4x4 ProjMat;
	CreateProjectionMatrix(&ProjMat, QuarterPi, 1.33333333f, 1.0f, 10000.0f);
	SetProjectionMatrix(ProjMat);
}


bool GraphicsContext::CreateVertexBuffer(uint32 VertexCount, UsageFlags usage, VertexBuffer **vb) const {
	long hr = D3DDevice->CreateVertexBuffer(VertexCount * sizeof(Vertex), (dword)usage, VertexFormat, D3DPOOL_DEFAULT, vb);
	return (hr == D3D_OK);
}

bool GraphicsContext::CreateIndexBuffer(uint32 IndexCount, UsageFlags usage, IndexBuffer **ib) const {
	long hr = D3DDevice->CreateIndexBuffer(IndexCount * IndexSize, (dword)usage, IndexFormat, D3DPOOL_DEFAULT, ib);
	return (hr == D3D_OK);
}

bool GraphicsContext::CreateSurface(uint32 Width, uint32 Height, Surface **surf) const {
	long hr = D3DDevice->CreateImageSurface(Width, Height, ColorFormat, surf);
	return (hr == D3D_OK);
}

bool GraphicsContext::CreateDepthStencil(uint32 Width, uint32 Height, Surface **zsurf) const {
	long hr = D3DDevice->CreateDepthStencilSurface(Width, Height, ZFormat, (D3DMULTISAMPLE_TYPE)AASamples, zsurf);
	return (hr == D3D_OK);
}

void GraphicsContext::Clear(dword color) const {
	D3DDevice->Clear(0, 0, D3DCLEAR_TARGET, color, 1.0f, 0);
}

void GraphicsContext::ClearZBuffer(float zval) const {
	D3DDevice->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, zval, 0);
}

void GraphicsContext::ClearStencil(byte sval) const {
	D3DDevice->Clear(0, 0, D3DCLEAR_STENCIL, 0, 1.0f, sval);
}

void GraphicsContext::ClearZBufferStencil(float zval, byte sval) const {
	D3DDevice->Clear(0, 0, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, zval, sval);
}

void GraphicsContext::Flip() const {
	D3DDevice->Present(0, 0, 0, 0);
}


bool GraphicsContext::Draw(VertexBuffer *vb) {
	D3DVERTEXBUFFER_DESC desc;
	vb->GetDesc(&desc);
	unsigned int verts = desc.Size / sizeof(Vertex);

	D3DDevice->SetStreamSource(0, vb, sizeof(Vertex));
	long res = D3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)ptype, 0, verts / 3);
	D3DDevice->SetStreamSource(0, 0, 0);
    return res == D3D_OK;
}

bool GraphicsContext::Draw(Vertex *varray, unsigned int VertexCount) {
	long res = D3DDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)ptype, VertexCount / 3, varray, sizeof(Vertex));
	return res == D3D_OK;
}

bool GraphicsContext::Draw(VertexBuffer *vb, IndexBuffer *ib) {
	D3DVERTEXBUFFER_DESC vbdesc;
	vb->GetDesc(&vbdesc);
	unsigned int verts = vbdesc.Size / sizeof(Vertex);

	D3DINDEXBUFFER_DESC ibdesc;
	ib->GetDesc(&ibdesc);
	unsigned int indices = ibdesc.Size / sizeof(Index);

	D3DDevice->SetStreamSource(0, vb, sizeof(Vertex));
	D3DDevice->SetIndices(ib, 0);
	long res = D3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)ptype, 0, verts, 0, indices / 3);
	D3DDevice->SetIndices(0, 0);
	D3DDevice->SetStreamSource(0, 0, 0);
	return res == D3D_OK;
}

bool GraphicsContext::Draw(Vertex *varray, Index *iarray, unsigned int VertexCount, unsigned int IndexCount) {
	long res = D3DDevice->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)ptype, 0, VertexCount, IndexCount / 3, iarray, IndexFormat, varray, sizeof(Vertex));
	return res == D3D_OK;
}

bool GraphicsContext::Draw(Vertex *varray, Triangle *triarray, unsigned int VertexCount, unsigned int TriCount) {
	unsigned int IndexCount = TriCount * 3;
	Index *iarray = new Index[IndexCount];
	for(dword i=0; i<TriCount; i++) {
		iarray[i*3] = triarray[i].vertices[0];
		iarray[i*3+1] = triarray[i].vertices[1];
		iarray[i*3+2] = triarray[i].vertices[2];
	}
	long res = Draw(varray, iarray, VertexCount, IndexCount);
	return res == D3D_OK;
}

////////////// State Setting Interface ///////////////

void GraphicsContext::SetPrimitiveType(PrimitiveType pt) {
	ptype = pt;
}

void GraphicsContext::SetBackfaceCulling(bool enable) {
	if(enable) {
		D3DDevice->SetRenderState(D3DRS_CULLMODE, CullOrder);
	} else {
		D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}
	BackfaceCulling = enable;
}

void GraphicsContext::SetFrontFace(FaceOrder order) {
	if(order == Clockwise) {
		CullOrder = CounterClockwise;
	} else {
		CullOrder = Clockwise;
	}
	if(BackfaceCulling) SetBackfaceCulling(BackfaceCulling);
}

void GraphicsContext::SetAutoNormalize(bool enable) {
	D3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, enable);
}

void GraphicsContext::SetBillboarding(bool enable) {
	BillBoardingEnabled = enable;
	if(enable) {

		Vector3 pos;
		pos.Transform(WorldMat[0]);
	
		Matrix4x4 world;
		world.Translate(pos.x, pos.y, pos.z);
		Matrix4x4 BillBoardRot;

		Vector3 dir = ViewMat.Inverse().GetRowVector(2);
		float yangle = dir.x > 0.0f ? -atanf(dir.z / dir.x) + HalfPi : -atanf(dir.z / dir.x) - HalfPi;

		BillBoardRot.Rotate(0.0f, yangle, 0.0f);
		Vector3 xaxis = VECTOR3_I;
		Vector3 normal = -VECTOR3_K;
		xaxis.Transform(BillBoardRot);
		normal.Transform(BillBoardRot);

		// find angle between quad normal and view direction
		float xangle = acosf(DotProduct(normal, dir.Normalized()));
		BillBoardRot.Rotate(xaxis, xangle);

		world = BillBoardRot * world;

		D3DDevice->SetTransform(D3DTS_WORLD, &world);
		//D3DDevice->SetTransform(D3DTS_VIEW, &BillboardViewMatrix);
	} else {
		D3DDevice->SetTransform(D3DTS_WORLD, &WorldMat[0]);
		//D3DDevice->SetTransform(D3DTS_VIEW, &ViewMat);
	}
}

void GraphicsContext::SetColorWrite(bool red, bool green, bool blue, bool alpha) {
	dword channels = 0;
	if(red) channels |= D3DCOLORWRITEENABLE_RED;
	if(green) channels |= D3DCOLORWRITEENABLE_GREEN;
	if(blue) channels |= D3DCOLORWRITEENABLE_BLUE;
	if(alpha) channels |= D3DCOLORWRITEENABLE_ALPHA;

	D3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, channels);
}

// blending states
void GraphicsContext::SetAlphaBlending(bool enable) {
	D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, enable);
}

void GraphicsContext::SetBlendFunc(BlendingFactor src, BlendingFactor dest) {
	D3DDevice->SetRenderState(D3DRS_SRCBLEND, src);
	D3DDevice->SetRenderState(D3DRS_DESTBLEND, dest);
}

// zbuffer states
void GraphicsContext::SetZBuffering(bool enable) {
	D3DDevice->SetRenderState(D3DRS_ZENABLE, enable);
}

void GraphicsContext::SetZWrite(bool enable) {
	D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, enable);
}

void GraphicsContext::SetZFunc(CmpFunc func) {
	D3DDevice->SetRenderState(D3DRS_ZFUNC, func);
}

// set stencil buffer states
void GraphicsContext::SetStencilBuffering(bool enable) {
	D3DDevice->SetRenderState(D3DRS_STENCILENABLE, enable);
}

void GraphicsContext::SetStencilPassOp(StencilOp sop) {
	D3DDevice->SetRenderState(D3DRS_STENCILPASS, sop);
}

void GraphicsContext::SetStencilFailOp(StencilOp sop) {
	D3DDevice->SetRenderState(D3DRS_STENCILFAIL, sop);
}

void GraphicsContext::SetStencilPassZFailOp(StencilOp sop) {
	D3DDevice->SetRenderState(D3DRS_STENCILZFAIL, sop);
}

void GraphicsContext::SetStencilOp(StencilOp Fail, StencilOp StencilPassZFail, StencilOp Pass) {
	D3DDevice->SetRenderState(D3DRS_STENCILPASS, Pass);
	D3DDevice->SetRenderState(D3DRS_STENCILFAIL, Fail);
	D3DDevice->SetRenderState(D3DRS_STENCILZFAIL, StencilPassZFail);
}

void GraphicsContext::SetStencilFunc(CmpFunc func) {
	D3DDevice->SetRenderState(D3DRS_STENCILFUNC, func);
}

void GraphicsContext::SetStencilReference(dword value) {
	D3DDevice->SetRenderState(D3DRS_STENCILREF, value);
}

// texture & material states

void GraphicsContext::SetTextureFactor(dword factor) {
	D3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, factor);
}

void GraphicsContext::SetTextureFiltering(TextureFilteringType texfilter, int TextureStage) {
	dword TexFilter, MipFilter;

	switch(texfilter) {
	case PointSampling:
		TexFilter = MipFilter = D3DTEXF_POINT;
		break;

	case BilinearFiltering:
		TexFilter = D3DTEXF_LINEAR;
		MipFilter = D3DTEXF_POINT;
		break;

	case TrilinearFiltering:
		TexFilter = MipFilter = D3DTEXF_LINEAR;
		break;

	case AnisotropicFiltering:
		TexFilter = D3DTEXF_ANISOTROPIC;
		MipFilter = D3DTEXF_LINEAR;
		break;

	default: break;
	}

	this->MipFilter = MipFilter;

	if(!MipMapEnabled) MipFilter = D3DTEXF_NONE;

	if(TextureStage == 0xa11) {
		for(int i=0; i<MaxTextureStages; i++) {
			D3DDevice->SetTextureStageState(i, D3DTSS_MINFILTER, TexFilter);
			D3DDevice->SetTextureStageState(i, D3DTSS_MAGFILTER, TexFilter);
			D3DDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, MipFilter);
		}
	} else {
		D3DDevice->SetTextureStageState(TextureStage, D3DTSS_MINFILTER, TexFilter);
		D3DDevice->SetTextureStageState(TextureStage, D3DTSS_MAGFILTER, TexFilter);
		D3DDevice->SetTextureStageState(TextureStage, D3DTSS_MIPFILTER, MipFilter);
	}
}

void GraphicsContext::SetTextureAddressing(TextureAddressing uaddr, TextureAddressing vaddr, int TextureStage) {
	if(TextureStage == 0xa11) {
        for(int i=0; i<MaxTextureStages; i++) {
			D3DDevice->SetTextureStageState(i, D3DTSS_ADDRESSU, uaddr);
			D3DDevice->SetTextureStageState(i, D3DTSS_ADDRESSV, vaddr);
		}
	} else {
		D3DDevice->SetTextureStageState(TextureStage, D3DTSS_ADDRESSU, uaddr);
		D3DDevice->SetTextureStageState(TextureStage, D3DTSS_ADDRESSV, vaddr);
	}
}

void GraphicsContext::SetTextureBorderColor(dword color, int TextureStage) {
	if(TextureStage == 0xa11) {
		for(int i=0; i<MaxTextureStages; i++) {
			D3DDevice->SetTextureStageState(i, D3DTSS_BORDERCOLOR, color);
		}
	} else {
		D3DDevice->SetTextureStageState(TextureStage, D3DTSS_BORDERCOLOR, color);
	}
}

void GraphicsContext::SetTexture(int index, Texture *tex) {
	D3DDevice->SetTexture(index, tex);
}

void GraphicsContext::SetMipMapping(bool enable, int TextureStage) {
	MipMapEnabled = enable;
	dword mip = (enable ? MipFilter : D3DTEXF_NONE);

	if(TextureStage == 0xa11) {
        for(int i=0; i<MaxTextureStages; i++) {
			D3DDevice->SetTextureStageState(i, D3DTSS_MIPFILTER, mip);
		}
	} else {
		D3DDevice->SetTextureStageState(TextureStage, D3DTSS_MIPFILTER, mip);
	}
}

void GraphicsContext::SetMaterial(const Material &mat) {
	D3DDevice->SetMaterial(&mat);
}

static unsigned long TLVertexFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

struct TLVertex {
	Vector3 pos;
	float rhw;
	unsigned long color;
	TexCoord tex;

	TLVertex() {
		pos = Vector3(0.0f, 0.0f, 0.0f);
		color = 0xffffffff;
		tex.u = tex.v = 0.0f;
		rhw = pos.z;
	}

	TLVertex(const Vector3 &pos, const Color &col, float u=0.0f, float v=0.0f) {
		this->pos = pos;
		rhw = pos.z;
		color = col.GetPacked32();
		tex.u = u;
		tex.v = v;
	}
};

void GraphicsContext::BlitTexture(const Texture *texture, RECT *rect, const Color &col) {
	bool norect = rect ? false : true;
	if(norect) {
		rect = new RECT;
		rect->left = rect->top = 0;
		rect->right = ContextParams.x;
		rect->bottom = ContextParams.y;
	}

	SetVertexProgram(TLVertexFVF);

	TLVertex tlverts[4];
	tlverts[0] = TLVertex(Vector3((float)rect->right, (float)rect->top, 1.0f), col, 1.0f, 0.0f);
	tlverts[1] = TLVertex(Vector3((float)rect->right, (float)rect->bottom, 1.0f), col, 1.0f, 1.0f);
	tlverts[2] = TLVertex(Vector3((float)rect->left, (float)rect->top, 1.0f), col, 0.0f, 0.0f);
	tlverts[3] = TLVertex(Vector3((float)rect->left, (float)rect->bottom, 1.0f), col, 0.0f, 1.0f);

	EnableTextureStage(0);
	DisableTextureStage(1);
	SetTexture(0, const_cast<Texture*>(texture));
	
	SetZBuffering(false);
	D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, tlverts, sizeof(TLVertex));
	SetZBuffering(true);

	SetTexture(0, 0);

	SetVertexProgram(VertexFormat);

	if(norect) {
		delete rect;
	}
}

// multitexturing interface
void GraphicsContext::EnableTextureStage(int stage) {
	SetTextureStageColor(stage, TexBlendModulate, TexArgTexture, TexArgCurrent);
}

void GraphicsContext::DisableTextureStage(int stage) {
	D3DDevice->SetTextureStageState(stage, D3DTSS_COLOROP, D3DTOP_DISABLE);
}

void GraphicsContext::SetTextureStageColor(int stage, TextureBlendFunction op, TextureBlendArgument arg1, TextureBlendArgument arg2, TextureBlendArgument arg3) {
	D3DDevice->SetTextureStageState(stage, D3DTSS_COLOROP, op);
	D3DDevice->SetTextureStageState(stage, D3DTSS_COLORARG1, arg1);
	D3DDevice->SetTextureStageState(stage, D3DTSS_COLORARG2, arg2);
	if(arg3) D3DDevice->SetTextureStageState(stage, D3DTSS_COLORARG0, arg3);
}

void GraphicsContext::SetTextureStageAlpha(int stage, TextureBlendFunction op, TextureBlendArgument arg1, TextureBlendArgument arg2, TextureBlendArgument arg3) {
	D3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAOP, op);
	D3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG1, arg1);
	D3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG2, arg2);
	if(arg3) D3DDevice->SetTextureStageState(stage, D3DTSS_ALPHAARG0, arg3);
}

void GraphicsContext::SetTextureCoordIndex(int stage, int index) {
	D3DDevice->SetTextureStageState(stage, D3DTSS_TEXCOORDINDEX, index);
}

void GraphicsContext::SetTextureTransformState(int stage, TexTransformState TexXForm) {
	D3DDevice->SetTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, TexXForm);
}

// programmable pipeline interface
void GraphicsContext::SetVertexProgram(dword vs) {
	D3DDevice->SetVertexShader(vs);
}

void GraphicsContext::SetPixelProgram(dword ps) {
	D3DDevice->SetPixelShader(ps);
}

dword GraphicsContext::CreateVertexProgram(const char *fname) {

	// vertex format declaration
	dword VertDecl[] = {
		D3DVSD_STREAM(0), 
		D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT1),
		D3DVSD_REG(D3DVSDE_BLENDINDICES, D3DVSDT_UBYTE4),
		D3DVSD_REG(D3DVSDE_NORMAL, D3DVSDT_FLOAT3),
		D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
		D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
		D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
		D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
		D3DVSD_REG(D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT2),
		D3DVSD_END()
	};


	ID3DXBuffer *code, *errors;
	if(D3DXAssembleShaderFromFile(fname, 0, 0, &code, &errors) != D3D_OK) {
		return 0xffffffff;
	}

	dword vprog;
	if(D3DDevice->CreateVertexShader(VertDecl, (dword*)code->GetBufferPointer(), &vprog, 0) != D3D_OK) {
		// could not create hardware vertex shader, try driver emulation...
		if(D3DDevice->CreateVertexShader(VertDecl, (dword*)code->GetBufferPointer(), &vprog, D3DUSAGE_SOFTWAREPROCESSING) != D3D_OK) {
			throw EngineGeneralException("The system lacks required programmable vertex processing unit support");
		}		
	}

	return vprog;
}

void GraphicsContext::DestroyVertexProgram(dword vprog) {
	D3DDevice->DeleteVertexShader(vprog);
}

void GraphicsContext::SetVertexProgramConstant(dword creg, float val) {
	float block[4] = {val, 0.0f, 0.0f, 1.0f};
	D3DDevice->SetVertexShaderConstant(creg, &block, 1);
}

void GraphicsContext::SetVertexProgramConstant(dword creg, const Vector3 &val) {
	Vector4 vec(val);
	D3DDevice->SetVertexShaderConstant(creg, &vec, 1);
}

void GraphicsContext::SetVertexProgramConstant(dword creg, const Vector4 &val) {
	D3DDevice->SetVertexShaderConstant(creg, &val, 1);
}

void GraphicsContext::SetVertexProgramConstant(dword creg, const Color &val) {
	D3DDevice->SetVertexShaderConstant(creg, &val, 1);
}

void GraphicsContext::SetVertexProgramConstant(dword creg, const Matrix4x4 &val) {
	D3DDevice->SetVertexShaderConstant(creg, &val, 4);
}

void GraphicsContext::SetVertexProgramConstant(dword creg, const void *data, dword size) {
	D3DDevice->SetVertexShaderConstant(creg, data, size);
}


// Lighting states
void GraphicsContext::SetLighting(bool enable) {
	D3DDevice->SetRenderState(D3DRS_LIGHTING, enable);
}

void GraphicsContext::SetColorVertex(bool enable) {
	D3DDevice->SetRenderState(D3DRS_COLORVERTEX, enable);
}

void GraphicsContext::SetAmbientLight(Color AmbientColor) {
	D3DDevice->SetRenderState(D3DRS_AMBIENT, AmbientColor.GetPacked32());
}

void GraphicsContext::SetShadingMode(ShadeMode mode) {
	D3DDevice->SetRenderState(D3DRS_SHADEMODE, mode);
}

void GraphicsContext::SetSpecular(bool enable) {
	D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, enable);
}

// transformation states
void GraphicsContext::SetWorldMatrix(const Matrix4x4 &WorldMat, unsigned int BlendIndex) {
	assert(BlendIndex < 256);
	this->WorldMat[BlendIndex] = WorldMat;
	if(!BillBoardingEnabled) {
		D3DDevice->SetTransform(D3DTS_WORLDMATRIX(BlendIndex), &WorldMat);
	} else {
        SetBillboarding(true);
	}
}

void GraphicsContext::SetViewMatrix(const Matrix4x4 &ViewMat) {
	this->ViewMat = ViewMat;
	if(!BillBoardingEnabled) {
		D3DDevice->SetTransform(D3DTS_VIEW, &ViewMat);
	} else {
		SetBillboarding(true);
	}
}

void GraphicsContext::SetProjectionMatrix(const Matrix4x4 &ProjMat) {
	this->ProjMat = ProjMat;
	D3DDevice->SetTransform(D3DTS_PROJECTION, &ProjMat);
}

void GraphicsContext::SetTextureMatrix(const Matrix4x4 &TexMat, unsigned int TextureStage) {
	assert(TextureStage < 8);
	this->TexMat[TextureStage] = TexMat;
	D3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)((int)D3DTS_TEXTURE0 + TextureStage), &TexMat);
}

void GraphicsContext::SetViewport(unsigned int x, unsigned int y, unsigned int xsize, unsigned int ysize, float MinZ, float MaxZ) {
	D3DVIEWPORT8 viewport;
	viewport.X = x;
	viewport.Y = y;
	viewport.Width = xsize;
	viewport.Height = ysize;
	viewport.MinZ = MinZ;
	viewport.MaxZ = MaxZ;

	D3DDevice->SetViewport(&viewport);
}


const Matrix4x4 &GraphicsContext::GetWorldMatrix(unsigned int BlendIndex) {
	assert(BlendIndex < 256);
	return WorldMat[BlendIndex];
}

const Matrix4x4 &GraphicsContext::GetViewMatrix() {
	return ViewMat;
}

const Matrix4x4 &GraphicsContext::GetProjectionMatrix() {
	return ProjMat;
}

const Matrix4x4 &GraphicsContext::GetTextureMatrix(unsigned int TextureStage) {
	assert(TextureStage < 8);
	return TexMat[TextureStage];
}


// render target
void GraphicsContext::ResetRenderTarget() {
	D3DDevice->SetRenderTarget(MainRenderTarget.ColorSurface, MainRenderTarget.DepthStencilSurface);
}

void GraphicsContext::SetRenderTarget(RenderTarget &rtarg) {
	D3DDevice->SetRenderTarget(rtarg.ColorSurface, rtarg.DepthStencilSurface);
}

void GraphicsContext::SetRenderTarget(Texture *rtarg, Texture *ztarg) {
	Surface *rendsurf, *zsurf = 0;
	rtarg->GetSurfaceLevel(0, &rendsurf);
	if(ztarg) ztarg->GetSurfaceLevel(0, &zsurf);

	D3DDevice->SetRenderTarget(rendsurf, zsurf);
}

void GraphicsContext::SetRenderTarget(Texture *rtarg, Surface *ztarg) {
	Surface *rendsurf;
	rtarg->GetSurfaceLevel(0, &rendsurf);

	D3DDevice->SetRenderTarget(rendsurf, ztarg);
}


////////////////////////////////////////////////////
//////////// Engine3D Member Functions /////////////
////////////////////////////////////////////////////

Engine3D::Engine3D() {

	if(!(d3d = Direct3DCreate8(D3D_SDK_VERSION))) {
		// actually this will never occur, if directx8 is not available in the system then
		// the OS loader will hit the problem first when it'll try to load d3d8.dll that is
		// linked through d3d8.lib, and complain for missing imports
		throw EngineInitException("DirectX 8.1 is required to run this program");
	}

	RetrieveAdapterInfo();
}

Engine3D::~Engine3D() {

	GraphicsContexts.erase(GraphicsContexts.begin(), GraphicsContexts.end());

	if(d3d) {
        d3d->Release();
		d3d = 0;
	}
}


////////////////////////////////////
// ----==( RetrieveAdapterInfo )==----
// (Private Member Function)
// Gets the list of adapters on the system and additional
// information for each adapter including its capabilities
////////////////////////////////////
void Engine3D::RetrieveAdapterInfo() {

	// retrieve adapter list
	AdapterCount = d3d->GetAdapterCount();
	adapters = new Adapter[AdapterCount];

	for(unsigned int i=0; i<AdapterCount; i++) {

		// get adapter identifier and driver information
		D3DADAPTER_IDENTIFIER8 AdapterIdentifier;
		d3d->GetAdapterIdentifier(i, D3DENUM_NO_WHQL_LEVEL, &AdapterIdentifier);
		adapters[i].Description = new char[strlen(AdapterIdentifier.Description)+1];
		strcpy(adapters[i].Description, AdapterIdentifier.Description);
		adapters[i].Driver = new char[strlen(AdapterIdentifier.Driver)+1];
		strcpy(adapters[i].Driver, AdapterIdentifier.Driver);

		adapters[i].DeviceGUID = AdapterIdentifier.DeviceIdentifier;
		adapters[i].DeviceID = AdapterIdentifier.DeviceId;
		adapters[i].DriverVersion = AdapterIdentifier.DriverVersion.QuadPart;
		adapters[i].Revision = AdapterIdentifier.Revision;
		adapters[i].SubSysID = AdapterIdentifier.SubSysId;
		adapters[i].VentorID = AdapterIdentifier.VendorId;

		// get a list of display modes for this adapter
		LinkedList<DisplayMode> dmlist;
		adapters[i].ModeCount = d3d->GetAdapterModeCount(i);
		for(unsigned int j=0; j<adapters[i].ModeCount; j++) {
			D3DDISPLAYMODE d3dmode;
			DisplayMode mode;
			d3d->EnumAdapterModes(i, j, &d3dmode);			
			mode.XRes = d3dmode.Width;
			mode.YRes = d3dmode.Height;
			mode.RefreshRate = d3dmode.RefreshRate;
			mode.ColorFormat = GetColorDepthFromPixelFormat(d3dmode.Format);

			// check if this mode is supported from the HAL device
			D3DFORMAT dispfmt = GetPixelFormatFromColorDepth(mode.ColorFormat, true);
			if(d3d->CheckDeviceType(i, D3DDEVTYPE_HAL, dispfmt, d3dmode.Format, false) == D3D_OK) {
				dmlist.PushBack(mode);
			}
		}
		adapters[i].ModeCount = dmlist.Size();	// count of the modes that are supported through HW
		adapters[i].Modes = new DisplayMode[adapters[i].ModeCount];
		ListNode<DisplayMode> *iter = dmlist.Begin();
		int j = 0;
		while(iter) {
			adapters[i].Modes[j++] = iter->data;
			iter = iter->next;
		}

		// get the device capabilities
		d3d->GetDeviceCaps(i, D3DDEVTYPE_HAL, &adapters[i].Capabilities);
		int vsver_major = D3DSHADER_VERSION_MAJOR(adapters[i].Capabilities.VertexShaderVersion);
		int vsver_minor = D3DSHADER_VERSION_MINOR(adapters[i].Capabilities.VertexShaderVersion);
	}
}

////////////////////////////////////
// ----==( CreateModesList )==----
// (Private Member Function)
// Creates a linked list of the available modes 
// of a single adapter for further processing
////////////////////////////////////
LinkedList<DisplayMode> *Engine3D::CreateModesList(unsigned int AdapterID) const {

	if(AdapterID >= AdapterCount) return 0;

	LinkedList<DisplayMode> *newlist = new LinkedList<DisplayMode>;
	for(unsigned int i=0; i<adapters[AdapterID].ModeCount; i++) {
		newlist->PushBack(adapters[AdapterID].Modes[i]);
	}
	return newlist;
}

////////////////////////////////////
// ----==( NarrowModesList )==----
// (Private Member Function)
// Narrows down the list of available modes
// to those that have the requested item
////////////////////////////////////
void Engine3D::NarrowModesList(LinkedList<DisplayMode> *list, DisplayModeItem item, long value, long value2) const {

	ListNode<DisplayMode> *iter = list->Begin();
	while(iter) {
		switch(item) {
		case ModeItemSize:
			if(iter->data.XRes != value || iter->data.YRes != value2) {
				iter = list->Erase(iter);
			} else {
				if(iter) iter = iter->next;
			}
			break;

		case ModeItemBpp:
			if(iter->data.ColorFormat.bpp != value) {
				iter = list->Erase(iter);
			} else {
				if(iter) iter = iter->next;
			}
			break;

		case ModeItemAlpha:
			if(!iter->data.ColorFormat.alpha && value) {
				iter = list->Erase(iter);
			} else {
				if(iter) iter = iter->next;
			}
			break;

		case ModeItemRefresh:
			if(iter->data.RefreshRate != value) {
				iter = list->Erase(iter);
			} else {
				if(iter) iter = iter->next;
			}
			break;

		default: break;
		}
	}
}

///////////////////////////////////
// ----==( ChooseBestMode )==----
// (Private Member Function)
// given a (ideally) narrowed down modes list
// choose the best possible among them
///////////////////////////////////
DisplayMode Engine3D::ChooseBestMode(LinkedList<DisplayMode> *modes) const {

	DisplayMode dmode;
	memset(&dmode, 0, sizeof(DisplayMode));

	if(!modes || !modes->Size()) return dmode;

	// find the highest resolution and get only the modes with that resolution
	ListNode<DisplayMode> *iter = modes->Begin();
	unsigned int maxx = 0, maxy = 0;
	while(iter) {
		if(iter->data.XRes > maxx) maxx = iter->data.XRes;
		if(iter->data.YRes > maxy) maxy = iter->data.YRes;
		iter = iter->next;
	}
	NarrowModesList(modes, ModeItemSize, maxx, maxy);

	// find the modes with alpha if any
	iter = modes->Begin();
	bool AnyWithAlpha = false;
	while(iter) {
		if(iter->data.ColorFormat.alpha) {
			AnyWithAlpha = true;
			break;
		}
		iter = iter->next;
	}
	if(AnyWithAlpha) NarrowModesList(modes, ModeItemAlpha, 1);

	// find the modes with the highest bpp
	iter = modes->Begin();
	int maxbpp = 0;
	while(iter) {
		if(iter->data.ColorFormat.bpp > maxbpp) maxbpp = iter->data.ColorFormat.bpp;
		iter = iter->next;
	}
	NarrowModesList(modes, ModeItemBpp, maxbpp);

	// find the modes with the highest refresh rate
	iter = modes->Begin();
	unsigned int maxrefresh = 0;
	while(iter) {
		if(iter->data.RefreshRate > maxrefresh) maxrefresh = iter->data.RefreshRate;
		iter = iter->next;
	}
	NarrowModesList(modes, ModeItemRefresh, maxrefresh);

	// if there is more than one mode left, then there is a problem :)
	assert(modes->Size() == 1);
	
	dmode = modes->Begin()->data;
	return dmode;
}

//////////////////////////////////////////
// ----==( CreateGraphicsContext )==----
// (Public Member Function)
// Creates a graphics context with the specified parameters
//////////////////////////////////////////
GraphicsContext *Engine3D::CreateGraphicsContext(HWND WindowHandle, unsigned int AdapterID, ContextInitParameters *GCParams) {

	if(AdapterID >= AdapterCount) return 0;

	// check adapter's Transformation & Lighting capability
	bool hwtnl = (bool)(adapters[AdapterID].Capabilities.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT);	
	if(!(GCParams->DontCareFlags & GCPDONTCARE_TNL)) {	// bit not set, we want specific TnL mode
		if(hwtnl != GCParams->HardwareTnL && !hwtnl) return 0;	// we asked for hw tnl and is not available
		// else either we asked for sw and found hw so we continue with our initial sw setting
		// or we found exactly what we asked, so we still continue with our choice
	} else {
		GCParams->HardwareTnL = hwtnl;	// if we don't care use what we have available
	}

	// decide which mode to use
	LinkedList<DisplayMode> *modes = CreateModesList(AdapterID);
	NarrowModesList(modes, ModeItemSize, GCParams->x, GCParams->y);

	if(!(GCParams->DontCareFlags & GCPDONTCARE_BPP)) {	// we want specific bpp
		NarrowModesList(modes, ModeItemBpp, GCParams->bpp);
	}

	if(!(GCParams->DontCareFlags & GCPDONTCARE_ALPHA)) {	// alpha setting exactly as asked
		NarrowModesList(modes, ModeItemAlpha, (long)GCParams->AlphaChannel);
	}

	if(!(GCParams->DontCareFlags & GCPDONTCARE_REFRESH)) {	// specific refresh rate
		NarrowModesList(modes, ModeItemRefresh, GCParams->RefreshRate);
	}

	if(!modes->Size()) {	// didn't find any mode with the properties that we asked
		throw EngineInitException("Requested video mode parameters not available");
	}

	DisplayMode mode = ChooseBestMode(modes);
	delete modes;	// delete the list of modes

	D3DFORMAT PixelFormat = GetPixelFormatFromColorDepth(mode.ColorFormat);

	// find out if we have the requested zbuffer format avaialble
	if(!GCParams->DepthBits) GCParams->DepthBits = 32;	// not specified, trying highest possible first
	D3DFORMAT zfmt = GetDepthStencilFormat(GCParams->DepthBits, true);

	bool res = !(bool)d3d->CheckDeviceFormat(AdapterID, (D3DDEVTYPE)GCParams->DevType, PixelFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, zfmt);
	if(!res) {	// try a format without stencil
		zfmt = GetDepthStencilFormat(GCParams->DepthBits, false);
		res = !(bool)d3d->CheckDeviceFormat(AdapterID, (D3DDEVTYPE)GCParams->DevType, PixelFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, zfmt);
	}

	if(!res) {	// didn't find requested zbuffer even with no stencil
		// if we asked for specific zformat and didn't find it or if the zbuffer that
		// we failed to set was 16bits and can't go less, bail out
		if(!(GCParams->DontCareFlags & GCPDONTCARE_DEPTH) || GCParams->DepthBits == 16) return 0;

		// try to set a smaller zbuffer with stencil
		GCParams->DepthBits = 16;
		zfmt = GetDepthStencilFormat(GCParams->DepthBits, true);
		res = !(bool)d3d->CheckDeviceFormat(AdapterID, (D3DDEVTYPE)GCParams->DevType, PixelFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, zfmt);
		if(!res) {	// try a format without stencil
			zfmt = GetDepthStencilFormat(GCParams->DepthBits, false);
			res = !(bool)d3d->CheckDeviceFormat(AdapterID, (D3DDEVTYPE)GCParams->DevType, PixelFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, zfmt);
		}

		if(!res) throw EngineInitException("Requested ZBuffer depth not available");
	}

	int AASamples = 0;
	if(GCParams->Antialiasing) {
		if(GCParams->BestAA) {
            // find the best supported AA mode
			for(AASamples=16; AASamples > 0; AASamples--) {
				long result = d3d->CheckDeviceMultiSampleType(AdapterID, (D3DDEVTYPE)GCParams->DevType, PixelFormat, !GCParams->FullScreen, (D3DMULTISAMPLE_TYPE)AASamples);
				if(result == D3D_OK) break;
			}
		} else {
			// check for cheap AA
			AASamples = 2;
			long result = d3d->CheckDeviceMultiSampleType(AdapterID, (D3DDEVTYPE)GCParams->DevType, PixelFormat, !GCParams->FullScreen, (D3DMULTISAMPLE_TYPE)AASamples);
			if(result != D3D_OK) AASamples = 0;
		}

		if(!AASamples && !(GCParams->DontCareFlags & GCPDONTCARE_AA)) {
			throw EngineInitException("Requested Antialiasing mode not available");
		}
	}	

	D3DFORMAT FinalColorFormat;
	if(GCParams->FullScreen) {
        FinalColorFormat = GetPixelFormatFromColorDepth(mode.ColorFormat);
	} else {
		D3DDISPLAYMODE CurrentMode;
		d3d->GetAdapterDisplayMode(AdapterID, &CurrentMode);
		FinalColorFormat = CurrentMode.Format;
	}

	// if everything went well, now we can set that mode
	D3DPRESENT_PARAMETERS d3dppar;
	d3dppar.BackBufferWidth = GCParams->x;
	d3dppar.BackBufferHeight = GCParams->y;
	d3dppar.BackBufferFormat = FinalColorFormat;
	d3dppar.BackBufferCount = (unsigned int)GCParams->Buffers;
	d3dppar.MultiSampleType = (D3DMULTISAMPLE_TYPE)AASamples;
	d3dppar.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dppar.hDeviceWindow = WindowHandle;
	d3dppar.Windowed = !GCParams->FullScreen;
	d3dppar.EnableAutoDepthStencil = true;
	d3dppar.AutoDepthStencilFormat = zfmt;
	d3dppar.Flags = 0;
	d3dppar.FullScreen_RefreshRateInHz = (GCParams->FullScreen) ? mode.RefreshRate : 0;
	if(GCParams->FullScreen) {
		d3dppar.FullScreen_PresentationInterval = (GCParams->VSync) ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
	} else {
		d3dppar.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	}

	// create the rendering context
	GraphicsContext *gc = new GraphicsContext;

	TnLMode tnlmode = (GCParams->HardwareTnL) ? HardwareTnL : SoftwareTnL;

	long result = d3d->CreateDevice(AdapterID, (D3DDEVTYPE)GCParams->DevType, WindowHandle, tnlmode, &d3dppar, &gc->D3DDevice);
	if(result != D3D_OK) {
		if(d3dppar.BackBufferCount != GCParams->Buffers) {
            if((GCParams->DontCareFlags & GCPDONTCARE_BUFFERS)) {
				result = d3d->CreateDevice(AdapterID, (D3DDEVTYPE)GCParams->DevType, WindowHandle, tnlmode, &d3dppar, &gc->D3DDevice);
			} else {
				throw EngineInitException("Could not create Direct3D device");
			}
		}
	}

	if(result != D3D_OK) {
		throw EngineInitException("Could not create Direct3D device");
	}

	gc->WindowHandle = WindowHandle;
	gc->D3DDevice->GetRenderTarget(&gc->MainRenderTarget.ColorSurface);
	gc->D3DDevice->GetDepthStencilSurface(&gc->MainRenderTarget.DepthStencilSurface);
	gc->ContextParams = *GCParams;
	gc->ZFormat = zfmt;
	gc->AASamples = AASamples;
	gc->ColorFormat = FinalColorFormat;
	gc->MaxTextureStages = adapters[AdapterID].Capabilities.MaxSimultaneousTextures;
	gc->texman = new TextureManager(gc);

	gc->SetDefaultStates();
	
	GraphicsContexts.push_back(gc);

	return gc;	
}

//////////////////////////////////////////
// ----==( CreateGraphicsContext )==----
// (Public Member Function)
// Creates a graphics context with the specified parameters
// Light version, basic info passed, fills in the rest
//////////////////////////////////////////
GraphicsContext *Engine3D::CreateGraphicsContext(HWND WindowHandle, int x, int y, int bpp, word flags) {

	ContextInitParameters gcp;
	gcp.x = x;
	gcp.y = y;
	gcp.bpp = bpp;
	gcp.DepthBits = 24;
	gcp.DevType = DeviceHardware;
	gcp.FullScreen = (flags & GCCREATE_FULLSCREEN) ? true : false;
	gcp.HardwareTnL = true;
	gcp.RefreshRate = 75;
	gcp.Antialiasing = false;
	gcp.Buffers = DoubleBuffering;
	gcp.VSync = false;
	gcp.DontCareFlags = GCPDONTCARE_DEPTH | GCPDONTCARE_REFRESH | GCPDONTCARE_ALPHA | GCPDONTCARE_VSYNC;

	return CreateGraphicsContext(WindowHandle, D3DADAPTER_DEFAULT, &gcp);
}

using std::string;
using std::getline;

string GetValue(string line) {
	int nexttoken = (int)line.find("=") + 1;
	while(line[++nexttoken] == ' ');
	int tokenend = (int)line.find_first_of(" \n\r\t");
	return line.substr(nexttoken, tokenend - nexttoken);
}

ContextInitParameters Engine3D::LoadContextParamsConfigFile(const char *cfgfilename) {
	ContextInitParameters cip;
	memset(&cip, 0, sizeof(ContextInitParameters));

	std::ifstream file(cfgfilename);
	if(!file.is_open()) throw EngineInitException("Could not open configuration file");

	string line;
	getline(file, line);
	while(!file.eof()) {
		if(line[0] != ';' && line[0] != '\n' && line[0] != '\r' && line[0] != ' ') {
			int tokenend = (int)line.find(" ");
			string token = line.substr(0, tokenend);

			if(token == "fullscreen") {
				string value = GetValue(line);
				cip.FullScreen = (value == "true") ? true : false;
			} else if(token == "resolution") {
                string value = GetValue(line);
				int x = (int)value.find("x");
				cip.x = atoi(value.substr(0, x).c_str());
				cip.y = atoi(value.substr(x+1, value.size()).c_str());
			} else if(token == "bpp") {
				cip.bpp = atoi(GetValue(line).c_str());
			} else if(token == "zbufferdepth") {
				cip.DepthBits = atoi(GetValue(line).c_str());
			} else if(token == "device") {
				cip.DevType = (GetValue(line) == "ref") ? DeviceReference : DeviceHardware;
			} else if(token == "tnl") {
				cip.HardwareTnL = (GetValue(line) == "false") ? false : true;
			} else if(token == "refresh") {
				cip.RefreshRate = atoi(GetValue(line).c_str());
			} else if(token == "antialiasing") {
				string value = GetValue(line);
				cip.Antialiasing = (value == "none") ? false : true;
				if(cip.Antialiasing) {
					cip.BestAA = (value == "speed" || value == "low") ? false : true;
				}
			} else if(token == "flipchain") {
				cip.Buffers = (GetValue(line) == "triplebuffering") ? TripleBuffering : DoubleBuffering;
			} else if(token == "vsync") {
				cip.VSync = (GetValue(line) == "true") ? true : false;
			} else if(token == "dontcareabout") {
				cip.DontCareFlags = 0;
				string flags = GetValue(line);
                string part;
				while(1) {
					int begin = (int)flags.find_first_not_of(", ");
					int end = (int)flags.find(",", begin) - begin;
					part = flags.substr(begin, end);
					//part = flags.substr(0, flags.find(","));// \n\r\t"));
					if(part.empty()) break;
					if(part == "bpp") cip.DontCareFlags |= GCPDONTCARE_BPP;
					if(part == "refresh") cip.DontCareFlags |= GCPDONTCARE_REFRESH;
					if(part == "alpha") cip.DontCareFlags |= GCPDONTCARE_ALPHA;
					if(part == "zbufferdepth") cip.DontCareFlags |= GCPDONTCARE_DEPTH;
					if(part == "tnl") cip.DontCareFlags |= GCPDONTCARE_TNL;
					if(part == "flipchain") cip.DontCareFlags |= GCPDONTCARE_BUFFERS;
					if(part == "aamode") cip.DontCareFlags |= GCPDONTCARE_AA;
					if(part == "vsync") cip.DontCareFlags |= GCPDONTCARE_VSYNC;
					int temp = (int)flags.find_first_of(",\n\r");
					if(temp == string::npos) break;
					flags.erase(0, temp+1);
				}
			}

		}
		        
		getline(file, line);
	}

	return cip;
}

void Engine3D::DestroyGraphicsContext(GraphicsContext *gc) {
	gc->D3DDevice->Release();
}

int Engine3D::GetAdapterCount() const {
	return AdapterCount;
}

const Adapter *Engine3D::GetAdapterInfo(int adapter) const {
	return &adapters[adapter];
}


//////////////////////////////////////////////////////////////////////////////
// helper functions

bool Lock(VertexBuffer *vb, Vertex **data) {
	D3DVERTEXBUFFER_DESC desc;
	vb->GetDesc(&desc);
	dword flags = (desc.Usage & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE : 0;
	
	return (vb->Lock(0, 0, (byte**)data, flags) == D3D_OK);
}

bool Lock(IndexBuffer *ib, Index **data) {
	D3DINDEXBUFFER_DESC desc;
	ib->GetDesc(&desc);
	dword flags = (desc.Usage & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE : 0;

	return (ib->Lock(0, 0, (byte**)data, flags) == D3D_OK);
}

void Unlock(VertexBuffer *vb) {
	vb->Unlock();
}

void Unlock(IndexBuffer *ib) {
	ib->Unlock();
}


void CreateProjectionMatrix(Matrix4x4 *mat, float yFOV, float Aspect, float NearClip, float FarClip) {

    float h, w, Q;
	float xFOV = yFOV * Aspect;
 
    w = (float)(1.0f/tan(xFOV*0.5f));		// 1/tan(x) == cot(x)
    h = (float)(1.0f/tan(yFOV*0.5f));		// 1/tan(x) == cot(x)
    Q = FarClip / (FarClip - NearClip);
	
	/*	Projection Matrix
	[ w     0.0f      0.0f      0.0f]
	|0.0f    h        0.0f      0.0f|
	|0.0f   0.0f       Q        1.0f|
	[0.0f   0.0f  -Q*NearClip   0.0f]
	*/

	mat->ResetIdentity();
    mat->m[0][0] = w;
	mat->m[1][1] = h;
	mat->m[2][2] = Q;
	mat->m[3][2] = -Q*NearClip;
	mat->m[2][3] = 1.0f;
}

dword PackVector(const Vector3 &vec, float Height) {
	dword r = (dword)(127.0f * vec.x + 128.0f);
	dword g = (dword)(127.0f * vec.y + 128.0f);
	dword b = (dword)(127.0f * vec.z + 128.0f);
	dword a = (dword)(255.0f * Height);
    
	return( (a<<24L) + (r<<16L) + (g<<8L) + (b<<0L) );
}

void NormalMapFromHeightField(Texture *tex) {
	
	// Lock the texture
	D3DLOCKED_RECT d3dlr;
	D3DSURFACE_DESC d3dsd;

	int LevelCount = tex->GetLevelCount();
	for(int i=0; i<LevelCount; i++) {
		
		tex->GetLevelDesc(i, &d3dsd);
		tex->LockRect(i, &d3dlr, 0, 0);
		DWORD* pPixel = (DWORD*)d3dlr.pBits;

		// For each pixel, generate a vector normal that represents the change
		// in thea height field at that pixel
		for( DWORD j=0; j<d3dsd.Height; j++  ) {
			for( DWORD i=0; i<d3dsd.Width; i++ ) {
				DWORD color00 = pPixel[0];
				DWORD color10 = pPixel[1];
				DWORD color01 = pPixel[d3dlr.Pitch/sizeof(DWORD)];

				float fHeight00 = (float)((color00 & 0x00ff0000) >> 16) / 255.0f;
				float fHeight10 = (float)((color10 & 0x00ff0000) >> 16) / 255.0f;
				float fHeight01 = (float)((color01 & 0x00ff0000) >> 16) / 255.0f;

				Vector3 vPoint00(i+0.0f, j+0.0f, fHeight00);
				Vector3 vPoint10(i+1.0f, j+0.0f, fHeight10);
				Vector3 vPoint01(i+0.0f, j+1.0f, fHeight01);
				Vector3 v10 = vPoint10 - vPoint00;
				Vector3 v01 = vPoint01 - vPoint00;

				Vector3 Normal = v10.CrossProduct(v01);
				Normal.Normalize();

				// Store the normal as an RGBA value in the normal map
				*pPixel++ = PackVector(Normal, fHeight00);
			}
		}
		tex->UnlockRect(i);
	}
}

void UpdateMipmapChain(Texture *tex) {
	D3DXFilterTexture(tex, 0, 0, D3DX_FILTER_BOX);
}

dword AddEdge(Edge *edges, dword EdgeCount, const Edge &newedge) {
	// remove internal edges
	for(dword i=0; i<EdgeCount; i++) {
		if((edges[i].vertices[0] == newedge.vertices[0] && edges[i].vertices[1] == newedge.vertices[1]) || (edges[i].vertices[0] == newedge.vertices[1] && edges[i].vertices[1] == newedge.vertices[0])) {
			edges[i] = edges[--EdgeCount];
			return EdgeCount;
		}
	}

    edges[EdgeCount++] = newedge;
	return EdgeCount;
}

//#define LIGHTDIR(l, p)  (l->GetType() == LTDir ? l->GetDirection() : p - l->GetPosition())
inline Vector3 GetLightDir(const Light *light, const Vector3 &pos, const Matrix4x4 &mat) {
	if(light->GetType() == LTDir) {
		return light->GetDirection();
	} else {
		Vector3 lpos = light->GetPosition();
		lpos.Transform(mat);
		return pos - lpos;
	}
	return Vector3();
}
		

//////////////////////////////////////////
// ----==( CreateShadowVolume )==----
// (Helper Function)
// Creates a graphics context with the specified parameters
// Light version, basic info passed, fills in the rest
//////////////////////////////////////////

TriMesh *CreateShadowVolume(const TriMesh &mesh, const Light *light, const Matrix4x4 &MeshXForm, bool WorldCoords) {

	// transform light into object's local coordinate system
	//const_cast<Light*>(light)->Transform(MeshXForm.Inverse());
	Matrix4x4 InvXForm = MeshXForm.Inverse();

	const Vertex *varray = mesh.GetVertexArray();
	const Triangle *triarray = mesh.GetTriangleArray();

	dword VertexCount = mesh.GetVertexCount();
	dword TriangleCount = mesh.GetTriangleCount();

	Edge *edges = new Edge[TriangleCount];
	dword EdgeCount = 0;

	// first find the contour edges

	for(dword i=0; i<TriangleCount; i++) {

		// find the light vector incident at this triangle
		Vertex TriVerts[3];
		TriVerts[0] = varray[triarray[i].vertices[0]];
		TriVerts[1] = varray[triarray[i].vertices[1]];
		TriVerts[2] = varray[triarray[i].vertices[2]];

		Vector3 pos = (TriVerts[0].pos + TriVerts[1].pos + TriVerts[2].pos) / 3.0f;
		//Vector3 LightDir = LIGHTDIR(light, pos);
		Vector3 LightDir = GetLightDir(light, pos, InvXForm);

		// if it looks away from the light...
		if(DotProduct(triarray[i].normal, LightDir) >= 0.0f) {
			EdgeCount = AddEdge(edges, EdgeCount, Edge(triarray[i].vertices[0], triarray[i].vertices[1]));
			EdgeCount = AddEdge(edges, EdgeCount, Edge(triarray[i].vertices[1], triarray[i].vertices[2]));
			EdgeCount = AddEdge(edges, EdgeCount, Edge(triarray[i].vertices[2], triarray[i].vertices[0]));
		}
	}

	// now extract the contour edges to build the shadow volume boundrary
	const float ExtrudeMagnitude = 100000.0f;
	Vertex *ShadowVertices = new Vertex[EdgeCount * 6];

	for(dword i=0; i<EdgeCount; i++) {
		Vertex QuadVert[4];
		QuadVert[0] = varray[edges[i].vertices[0]];
		QuadVert[1] = varray[edges[i].vertices[1]];
		//QuadVert[2] = QuadVert[1].pos + (Vector3)LIGHTDIR(light, QuadVert[1].pos) * ExtrudeMagnitude;
		//QuadVert[3] = QuadVert[0].pos + (Vector3)LIGHTDIR(light, QuadVert[0].pos) * ExtrudeMagnitude;
		QuadVert[2] = QuadVert[1].pos + GetLightDir(light, QuadVert[1].pos, InvXForm) * ExtrudeMagnitude;
		QuadVert[3] = QuadVert[0].pos + GetLightDir(light, QuadVert[0].pos, InvXForm) * ExtrudeMagnitude;

        ShadowVertices[i*6] = QuadVert[0];
		ShadowVertices[i*6+1] = QuadVert[1];
		ShadowVertices[i*6+2] = QuadVert[2];

		ShadowVertices[i*6+3] = QuadVert[0];
		ShadowVertices[i*6+4] = QuadVert[2];
		ShadowVertices[i*6+5] = QuadVert[3];

		if(WorldCoords) {
			for(int j=0; j<6; j++) {
				ShadowVertices[i*6+j].pos.Transform(MeshXForm);
			}
		}
	}

	TriMesh *ShadowMesh = new TriMesh(1);
	ShadowMesh->SetData(ShadowVertices, 0, EdgeCount * 6, 0);

	delete [] ShadowVertices;
	delete [] edges;

	//const_cast<Light*>(light)->Transform(MeshXForm);	// return light to world coordinate system

	return ShadowMesh;	
}


///////////// local //////////////

ColorDepth GetColorDepthFromPixelFormat(D3DFORMAT fmt) {
	switch(fmt) {
	case D3DFMT_R8G8B8: return ColorDepth(24, 24, 0);
	case D3DFMT_A8R8G8B8: return ColorDepth(32, 24, 8);
	case D3DFMT_X8R8G8B8: return ColorDepth(32, 24, 0);
	case D3DFMT_R5G6B5: return ColorDepth(16, 16, 0);
	case D3DFMT_X1R5G5B5: return ColorDepth(16, 15, 0);
	case D3DFMT_A1R5G5B5: return ColorDepth(16, 15, 1);
	case D3DFMT_P8: return ColorDepth(8, 8, 0);
	default: return ColorDepth(0, 0, 0);
	}
}

bool operator ==(const ColorDepth &lhs, const ColorDepth &rhs) {
	return (lhs.bpp == rhs.bpp && lhs.colorbits == rhs.colorbits && lhs.alpha == rhs.alpha);
}

D3DFORMAT GetPixelFormatFromColorDepth(ColorDepth cd, bool noalpha) {
	if(cd == ColorDepth(24, 24, 0)) return D3DFMT_R8G8B8;
	if(cd == ColorDepth(32, 24, 8)) return noalpha ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8;
	if(cd == ColorDepth(32, 24, 0)) return D3DFMT_X8R8G8B8;
	if(cd == ColorDepth(16, 16, 0)) return D3DFMT_R5G6B5;
	if(cd == ColorDepth(16, 15, 0)) return D3DFMT_X1R5G5B5;
	if(cd == ColorDepth(16, 15, 1)) return noalpha ? D3DFMT_X1R5G5B5 : D3DFMT_A1R5G5B5;
	if(cd == ColorDepth(8, 8, 0)) return D3DFMT_P8;
	return (D3DFORMAT)0;
}


D3DFORMAT GetDepthStencilFormat(int depthbits, bool stencil) {
	switch(depthbits) {
	case 32: 
		return (stencil) ? D3DFMT_D24S8 : D3DFMT_D32;
		break;

	case 24:
		return (stencil) ? D3DFMT_D24S8 : D3DFMT_D24X8;
		break;

	case 16:
		return (stencil) ? D3DFMT_D15S1 : D3DFMT_D16;
		break;

	default:
		break;
	}

	return (D3DFORMAT)0;
}