#ifndef _3DENGINE_H_
#define _3DENGINE_H_

// standard includes
#include <vector>
// system includes
#include "d3d8.h"
// includes from my codebase
#include "typedefs.h"
#include "linkedlist.h"
#include "color.h"
// 3d engine includes
#include "n3dmath.h"
#include "switches.h"
#include "3dengtypes.h"
#include "material.h"
#include "textureman.h"
#include "3dgeom.h"

enum DeviceType {DeviceHardware = D3DDEVTYPE_HAL, DeviceReference = D3DDEVTYPE_REF};
enum TnLMode {HardwareTnL = D3DCREATE_HARDWARE_VERTEXPROCESSING, SoftwareTnL = D3DCREATE_SOFTWARE_VERTEXPROCESSING};
enum BufferChainMode {DoubleBuffering = 1, TripleBuffering = 2};
enum UsageFlags {UsageStatic = 0, UsageDynamic = D3DUSAGE_DYNAMIC};
enum ShadeMode {FlatShading = D3DSHADE_FLAT, GouraudShading = D3DSHADE_GOURAUD};
enum FaceOrder {Clockwise = D3DCULL_CW, CounterClockwise = D3DCULL_CCW};

enum PhongComponent {Ambient, Diffuse, Specular};

enum PrimitiveType {
	TriangleList	= D3DPT_TRIANGLELIST,
	TriangleStrip	= D3DPT_TRIANGLESTRIP,
	TriangleFan		= D3DPT_TRIANGLEFAN,
	LineList		= D3DPT_LINELIST,
	LineStrip		= D3DPT_LINESTRIP,
	PointList		= D3DPT_POINTLIST
};

enum BlendingFactor {
	BLEND_ZERO			= D3DBLEND_ZERO,
	BLEND_ONE			= D3DBLEND_ONE,
	BLEND_SRCCOLOR		= D3DBLEND_SRCCOLOR,
	BLEND_INVSRCCOLOR	= D3DBLEND_INVSRCCOLOR,
	BLEND_SRCALPHA		= D3DBLEND_SRCALPHA,
	BLEND_INVSRCALPHA	= D3DBLEND_INVSRCALPHA,
	BLEND_DESTCOLOR		= D3DBLEND_DESTCOLOR,
	BLEND_INVDESTCOLOR	= D3DBLEND_INVDESTCOLOR,
	BLEND_DESTALPHA		= D3DBLEND_DESTALPHA,
	BLEND_INVDESTALPHA	= D3DBLEND_INVDESTALPHA,
	BLEND_SRCALPHASAT	= D3DBLEND_SRCALPHASAT
};

enum CmpFunc {
	CMP_NEVER		= D3DCMP_NEVER,
    CMP_LESS		= D3DCMP_LESS,
    CMP_EQUAL		= D3DCMP_EQUAL,
    CMP_LEQUAL		= D3DCMP_LESSEQUAL,
    CMP_GREATER		= D3DCMP_GREATER,
    CMP_NOTEQUAL	= D3DCMP_NOTEQUAL,
    CMP_GEQUAL		= D3DCMP_GREATEREQUAL,
    CMP_ALWAYS		= D3DCMP_ALWAYS
};

enum StencilOp {
	SOP_KEEP		= D3DSTENCILOP_KEEP,
    SOP_ZERO		= D3DSTENCILOP_ZERO,
    SOP_REPLACE		= D3DSTENCILOP_REPLACE,
    SOP_INCSAT		= D3DSTENCILOP_INCRSAT,
    SOP_DECSAT		= D3DSTENCILOP_DECRSAT,
    SOP_INVERT		= D3DSTENCILOP_INVERT,
    SOP_INC			= D3DSTENCILOP_INCR,
    SOP_DEC			= D3DSTENCILOP_DECR
};

enum TextureBlendFunction {
	TexBlendSelectArg1			= D3DTOP_SELECTARG1,			// S[rgba] = Arg1
	TexBlendSelectArg2			= D3DTOP_SELECTARG2,			// S[rgba] = Arg2
	TexBlendAdd					= D3DTOP_ADD,					// S[rgba] = Arg1 + Arg2
	TexBlendAddSigned			= D3DTOP_ADDSIGNED,				// S[rgba] = Arg1 + Arg2 - 0.5
	TexBlendAddSigned2x			= D3DTOP_ADDSIGNED2X,			// S[rgba] = (Arg1 + Arg2 - 0.5) << 1
	TexBlendSubtract			= D3DTOP_SUBTRACT,				// S[rgba] = Arg1 - Arg2
	TexBlendAddSmooth			= D3DTOP_ADDSMOOTH,				// S[rgba] = (Arg1 + Arg2) - (Arg1 * Arg2)
	TexBlendModulate			= D3DTOP_MODULATE,				// S[rgba] = Arg1 * Arg2
	TexBlendModulate2x			= D3DTOP_MODULATE2X,			// S[rgba] = (Arg1 * Arg2) << 1
	TexBlendModulate4x			= D3DTOP_MODULATE4X,			// S[rgba] = (Arg1 * Arg2) << 2
	TexBlendVertexAlpha			= D3DTOP_BLENDDIFFUSEALPHA,		// S[rgba] = Arg1*Alpha + Arg2*(1-Alpha)
	TexBlendTextureAlpha		= D3DTOP_BLENDTEXTUREALPHA,		// S[rgba] = Arg1*Alpha + Arg2*(1-Alpha)
	TexBlendFactorAlpha			= D3DTOP_BLENDFACTORALPHA,		// S[rgba] = Arg1*Alpha + Arg2*(1-Alpha)
	TexBlendPrevAlpha			= D3DTOP_BLENDFACTORALPHA,		// S[rgba] = Arg1*Alpha + Arg2*(1-Alpha)
	TexBlendPreMulAlpha			= D3DTOP_BLENDTEXTUREALPHAPM,	// S[rgba] = Arg1 + Arg2*(1-Alpha)
	TexBlendPreModulate			= D3DTOP_PREMODULATE,
	TexBlendModAlphaAddColor	= D3DTOP_MODULATEALPHA_ADDCOLOR,// S[rgba] = Arg1[rgb] + Arg1[a] * Arg2[rgb]
	TexBlendModulateAddAlpha	= D3DTOP_MODULATECOLOR_ADDALPHA,// S[rgba] = Arg1[rgb] * Arg2[rgb] + Arg1[a]
	TexBlendModInvAlphaAddColor	= D3DTOP_MODULATEINVALPHA_ADDCOLOR,	// S[rgba] = Arg1[rgb] + (1-Arg1[a]) * Arg2[rgb]
	TexBlendModulateInvAddAlpha	= D3DTOP_MODULATEINVCOLOR_ADDALPHA,	// S[rgba] = (1-Arg1[rgb]) * Arg2[rgb] + Arg1[a]
	TexBlendBumpEnv				= D3DTOP_BUMPENVMAP,			// bump mapping with next stage's env map
	TexBlendBumpEnvLuminance	= D3DTOP_BUMPENVMAPLUMINANCE,	// bump mapping with next stage's env map with luminance
	TexBlendDotProduct			= D3DTOP_DOTPRODUCT3,			// S[rgba] = Arg1[rgb] (dot) Arg2[rgb]
	TexBlendMultiplyAdd			= D3DTOP_MULTIPLYADD,			// S[rgba] = Arg1 + Arg2 * Arg3
	TexBlendLerp				= D3DTOP_LERP					// S[rgba] = Arg2 + (Arg3 - Arg2) * Arg1
};

enum TextureBlendArgument {
	TexArgNone			= 0,	// valid only for arg3
	TexArgCurrent		= D3DTA_CURRENT,	// the color from the previous stage output (diffuse for 1st)
	TexArgDiffuseColor	= D3DTA_DIFFUSE,	// the diffuse interpolated color
	TexArgSpecularColor	= D3DTA_SPECULAR,	// the specular interpolated color
	TexArgTexture		= D3DTA_TEXTURE,	// the texture bound to this stage
	TexArgFactor		= D3DTA_TFACTOR,	// a user defined factor
	TexArgTemp			= D3DTA_TEMP		// temp register
};

enum TextureFilteringType {
	PointSampling,
	BilinearFiltering, 
	TrilinearFiltering, 
	AnisotropicFiltering
};

enum TextureAddressing {
    TexAddrWrap			= D3DTADDRESS_WRAP,
    TexAddrMirror		= D3DTADDRESS_MIRROR,
    TexAddrClamp		= D3DTADDRESS_CLAMP,
    TexAddrBorder		= D3DTADDRESS_BORDER,
    TexAddrMirrorOnce	= D3DTADDRESS_MIRRORONCE
};

enum TexTransformState {
	TexTransformDisable		= D3DTTFF_DISABLE,
	TexTransform1D			= D3DTTFF_COUNT1,
	TexTransform2D			= D3DTTFF_COUNT2,
	TexTransform3D			= D3DTTFF_COUNT3,
	TexTransform4D			= D3DTTFF_COUNT4,
	TexTransformProjected	= D3DTTFF_PROJECTED
};

// don't care flags for context creation
const unsigned short GCPDONTCARE_NONE		= 0;	// 0000000000000000
const unsigned short GCPDONTCARE_BPP		= 1;	// 0000000000000001
const unsigned short GCPDONTCARE_REFRESH	= 2;	// 0000000000000010
const unsigned short GCPDONTCARE_ALPHA		= 4;	// 0000000000000100
const unsigned short GCPDONTCARE_DEPTH		= 8;	// 0000000000001000
const unsigned short GCPDONTCARE_TNL		= 16;	// 0000000000010000
const unsigned short GCPDONTCARE_BUFFERS	= 32;	// 0000000000100000
const unsigned short GCPDONTCARE_AA			= 64;	// 0000000001000000
const unsigned short GCPDONTCARE_VSYNC		= 128;	// 0000000010000000

// fullscreen / windowed flags
const unsigned short GCCREATE_WINDOWED		= 0;
const unsigned short GCCREATE_FULLSCREEN	= 1;



class Vertex;

struct ColorDepth {
	int bpp, colorbits, alpha;

	ColorDepth(int bits=0, int c=0, int a=0) {colorbits = c; alpha = a; bpp = bits; }
};

struct DisplayMode {
	unsigned int XRes, YRes, RefreshRate;
	ColorDepth ColorFormat;
};

enum DisplayModeItem {ModeItemSize, ModeItemBpp, ModeItemAlpha, ModeItemRefresh};

struct Adapter {
	char *Driver, *Description;	
	int64 DriverVersion;
	dword VentorID, DeviceID, SubSysID, Revision;
	GUID DeviceGUID;

	unsigned int ModeCount;
	DisplayMode *Modes;
	D3DCAPS8 Capabilities;
};

struct RenderTarget {
	Surface *ColorSurface, *DepthStencilSurface;
};

struct RenderParams {
	ShadeMode Shading;
	bool Billboarded;
	dword VertexProgram;
	dword PixelProgram;
	bool ZWrite;
	BlendingFactor SourceBlendFactor, DestBlendFactor;
};

struct ContextInitParameters {
	int x, y;
	int bpp;
	int RefreshRate;
	bool AlphaChannel;
	int DepthBits;
	DeviceType DevType;
	bool HardwareTnL;
	bool FullScreen;
	bool Antialiasing;
	bool BestAA;
	bool VSync;
	BufferChainMode Buffers;
	unsigned short DontCareFlags;
};


class GraphicsContext {
private:
	PrimitiveType ptype;
	FaceOrder CullOrder;
	bool BackfaceCulling;
	bool MipMapEnabled;
	bool BillBoardingEnabled;
	dword MipFilter;
	
	// cache of current transformation matrices
	Matrix4x4 WorldMat[256], ViewMat, ProjMat, TexMat[8];

	// disable copying contexts by making copy constructor and assignment private
	GraphicsContext(const GraphicsContext &gc);
	const GraphicsContext &operator =(const GraphicsContext &gc);

public:
	HWND WindowHandle;
	RenderTarget MainRenderTarget;
	IDirect3DDevice8 *D3DDevice;
	ContextInitParameters ContextParams;
	D3DFORMAT ColorFormat, ZFormat;
	int AASamples;
	int MaxTextureStages;

	TextureManager *texman;		// texture manager
	
	GraphicsContext();

	void SetDefaultStates();

	bool CreateVertexBuffer(uint32 VertexCount, UsageFlags usage, VertexBuffer **vb) const;
	bool CreateIndexBuffer(uint32 IndexCount, UsageFlags usage, IndexBuffer **ib) const;

	bool CreateSurface(uint32 Width, uint32 Height, Surface **surf) const;
	bool CreateDepthStencil(uint32 Width, uint32 Height, Surface **zsurf) const;

	void Clear(dword color) const;
	void ClearZBuffer(float zval) const;
	void ClearStencil(byte sval) const;
	void ClearZBufferStencil(float zval, byte sval) const;

	void Flip() const;

	bool Draw(VertexBuffer *vb);
	bool Draw(Vertex *varray, unsigned int VertexCount);
	bool Draw(VertexBuffer *vb, IndexBuffer *ib);
	bool Draw(Vertex *varray, Index *iarray, unsigned int VertexCount, unsigned int IndexCount);
	bool Draw(Vertex *varray, Triangle *triarray, unsigned int VertexCount, unsigned int TriCount);

	IDirect3DDevice8 *GetDevice() const;
	int GetTextureStageNumber() const {return MaxTextureStages;}

	////// render states //////
	void SetPrimitiveType(PrimitiveType pt);
	void SetBackfaceCulling(bool enable);
	void SetFrontFace(FaceOrder order);
	void SetAutoNormalize(bool enable);
	void SetBillboarding(bool enable);
	void SetColorWrite(bool red, bool green, bool blue, bool alpha);

	// blending states
	void SetAlphaBlending(bool enable);
	void SetBlendFunc(BlendingFactor src, BlendingFactor dest);

	// zbuffer states
	void SetZBuffering(bool enable);
	void SetZWrite(bool enable);
	void SetZFunc(CmpFunc func);

	// set stencil buffer states
	void SetStencilBuffering(bool enable);
	void SetStencilPassOp(StencilOp sop);
	void SetStencilFailOp(StencilOp sop);
	void SetStencilPassZFailOp(StencilOp sop);
	void SetStencilOp(StencilOp Fail, StencilOp StencilPassZFail, StencilOp Pass);
	void SetStencilFunc(CmpFunc func);
	void SetStencilReference(dword value);

	// texture & material states
	void SetTextureFiltering(TextureFilteringType texfilter, int TextureStage = 0xa11);
	void SetTextureAddressing(TextureAddressing uaddr, TextureAddressing vaddr, int TextureStage = 0xa11);
	void SetTextureBorderColor(dword color, int TextureStage = 0xa11);
	void SetTexture(int index, Texture *tex);
	void SetTextureFactor(dword factor);
	void SetMipMapping(bool enable, int TextureStage = 0xa11);
	void SetMaterial(const Material &mat);

	void BlitTexture(const Texture *texture, RECT *rect, const Color &col = Color(1.0f));
	
	// multitexturing interface
	void EnableTextureStage(int stage);
	void DisableTextureStage(int stage);
	void SetTextureStageColor(int stage, TextureBlendFunction op, TextureBlendArgument arg1, TextureBlendArgument arg2, TextureBlendArgument arg3 = TexArgNone);
	void SetTextureStageAlpha(int stage, TextureBlendFunction op, TextureBlendArgument arg1, TextureBlendArgument arg2, TextureBlendArgument arg3 = TexArgNone);
	void SetTextureCoordIndex(int stage, int index);
	void SetTextureTransformState(int stage, TexTransformState TexXForm);
	//void SetTextureCoordGenerator(int stage, TexGen tgen);

	// programmable interface
	void SetVertexProgram(dword vs);
	void SetPixelProgram(dword ps);

	dword CreateVertexProgram(const char *fname);
	void DestroyVertexProgram(dword vprog);
	void SetVertexProgramConstant(dword creg, float val);
	void SetVertexProgramConstant(dword creg, const Vector3 &val);
	void SetVertexProgramConstant(dword creg, const Vector4 &val);
	void SetVertexProgramConstant(dword creg, const Color &val);
	void SetVertexProgramConstant(dword creg, const Matrix4x4 &val);
	void SetVertexProgramConstant(dword creg, const void *data, dword size);


	// lighting states
	void SetLighting(bool enable);
	void SetColorVertex(bool enable);
	void SetAmbientLight(Color AmbientColor);
	void SetShadingMode(ShadeMode mode);
	void SetSpecular(bool enable);

	// Transformation Matrices
	void SetWorldMatrix(const Matrix4x4 &WorldMat, unsigned int BlendIndex = 0);
	void SetViewMatrix(const Matrix4x4 &ViewMat);
	void SetProjectionMatrix(const Matrix4x4 &ProjMat);
	void SetTextureMatrix(const Matrix4x4 &TexMat, unsigned int TextureStage = 0);
	void SetViewport(unsigned int x, unsigned int y, unsigned int xsize, unsigned int ysize, float MinZ = 0.0f, float MaxZ = 1.0f);

	const Matrix4x4 &GetWorldMatrix(unsigned int BlendIndex = 0);
	const Matrix4x4 &GetViewMatrix();
	const Matrix4x4 &GetProjectionMatrix();
	const Matrix4x4 &GetTextureMatrix(unsigned int TextureStage = 0);

	// render target
	void ResetRenderTarget();
	void SetRenderTarget(RenderTarget &rtarg);
	void SetRenderTarget(Texture *rtarg, Texture *ztarg);
	void SetRenderTarget(Texture *rtarg, Surface *ztarg);
	//RenderTarget GetRenderTarget() const;
};



class Engine3D {
private:
	IDirect3D8 *d3d;
	unsigned int AdapterCount;
	Adapter *adapters;	// array of adapters (filled in at the constructor)
	std::vector<GraphicsContext*> GraphicsContexts;	// a list of active graphics contexts
	
	void RetrieveAdapterInfo();
	LinkedList<DisplayMode> *CreateModesList(unsigned int AdapterID) const;
	void NarrowModesList(LinkedList<DisplayMode> *list, DisplayModeItem item, long value, long value2=0) const;
	DisplayMode ChooseBestMode(LinkedList<DisplayMode> *modes) const;
	int MaxAntialiasingSamples() const;
	
public:

	Engine3D();
	~Engine3D();

	int GetAdapterCount() const;
	const Adapter *GetAdapterInfo(int adapter) const;

	GraphicsContext *CreateGraphicsContext(HWND WindowHandle, int x, int y, int bpp, word flags);
	GraphicsContext *CreateGraphicsContext(HWND WindowHandle, unsigned int AdapterID, ContextInitParameters *GCParams);
	ContextInitParameters LoadContextParamsConfigFile(const char *cfgfilename);

	void DestroyGraphicsContext(GraphicsContext *gc);
};

class Light;

// helper functions
bool Lock(VertexBuffer *vb, Vertex **data);
bool Lock(IndexBuffer *ib, Index **data);
void Unlock(VertexBuffer *vb);
void Unlock(IndexBuffer *ib);
void CreateProjectionMatrix(Matrix4x4 *mat, float yFOV, float Aspect, float NearClip, float FarClip);
void NormalMapFromHeightField(Texture *tex);
void UpdateMipmapChain(Texture *tex);
TriMesh *CreateShadowVolume(const TriMesh &mesh, const Light *light, const Matrix4x4 &MeshXForm, bool WorldCoords = false);

#endif	// _3DENGINE_H_