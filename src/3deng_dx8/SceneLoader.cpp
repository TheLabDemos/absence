//#include <fstream>
#include <windows.h>
#include <string>
#include <cassert>
#include <cctype>
#include "sceneloader.h"
#include "3dschunks.h"
#include "typedefs.h"

using std::ifstream;
using std::string;

namespace SceneLoader {
    GraphicsContext *gc;
	dword ReadCounter;
	Material *mat;
	dword MatCount;

	bool eof = false;

	string datapath = "";

	string SceneFileName;
	string ObjectName;

	bool SaveNormalFile = false;
}

using namespace SceneLoader;

struct ChunkHeader {
	ChunkID id;
	dword size;
};

struct Percent {
	int IntPercent;
	float FloatPercent;

	Percent(int p = 0) {IntPercent = p; FloatPercent = (float)p / 100.0f; }
	Percent(float p) {FloatPercent = p; IntPercent = (int)(p * 100.0f); }
};

struct TexMap {
	string filename;
	TextureType type;
	float intensity;
	float rotation;
	Vector2 offset;
	Vector2 scale;
};

const dword HeaderSize = 6;

enum {OBJ_MESH, OBJ_PTLIGHT, OBJ_SPLIGHT, OBJ_CAMERA, OBJ_CURVE};

// local function prototypes
byte ReadByte(HANDLE file);
word ReadWord(HANDLE file);
dword ReadDword(HANDLE file);
float ReadFloat(HANDLE file);
Vector3 ReadVector(HANDLE file, bool FlipYZ = true);
string ReadString(HANDLE file);
Color ReadColor(HANDLE file);
Percent ReadPercent(HANDLE file);
ChunkHeader ReadChunkHeader(HANDLE file);
void SkipChunk(HANDLE file, const ChunkHeader &chunk);
void SkipBytes(HANDLE file, dword bytes);

int ReadObject(HANDLE file, const ChunkHeader &ch, void **obj);
int ReadLight(HANDLE file, const ChunkHeader &ch, Light **lt);
Material ReadMaterial(HANDLE file, const ChunkHeader &ch);
TexMap ReadTextureMap(HANDLE file, const ChunkHeader &ch);

Material *FindMaterial(string name);

bool LoadNormalsFromFile(const char *fname, Scene *scene);
void SaveNormalsToFile(const char *fname, Scene *scene);


void SceneLoader::SetGraphicsContext(GraphicsContext *gfx) {
	gc = gfx;
}


void SceneLoader::SetDataPath(const char *path) {
	datapath = path;
}

void SceneLoader::SetNormalFileSaving(bool enable) {
	SaveNormalFile = enable;
}



////////////////////////////////////////
//   --==( function LoadScene )==--   //
// ---------------------------------- //
// Creates a Scene instance and loads //
// the data from specified file       //
////////////////////////////////////////

bool SceneLoader::LoadScene(const char *fname, Scene **scene) {
	if(!gc) return false;

	if(!LoadMaterials(fname, &mat)) return false;

	//ifstream file(fname);
	//if(!file.is_open()) return false;
	HANDLE file = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	assert(file != NULL);
	eof = false;

	SceneFileName = string(fname);

	ChunkHeader chunk;
	Scene *scn = new Scene(gc);		// new scene instance
	
	chunk = ReadChunkHeader(file);
	if(chunk.id != Chunk_3DSMain) {
		CloseHandle(file);
		return false;
	}

	//while(!file.eof()) {
	while(!eof) {

		chunk = ReadChunkHeader(file);

		void *objptr;
		int type;

		switch(chunk.id) {
		case Chunk_Main_3DEditor:
			break;	// dont skip

		case Chunk_Edit_AmbientColor:
			scn->SetAmbientLight(ReadColor(file));
			break;

		case Chunk_Edit_Fog:
			// **TODO** find out chunk structure
			break;

		case Chunk_Edit_Object:
			type = ReadObject(file, chunk, &objptr);
			switch(type) {
			case OBJ_MESH:
				{
					Object *object = (Object*)objptr;
					scn->AddObject(object);
				}
				break;

			case OBJ_CAMERA:
				{
					Camera *cam = (Camera*)objptr;
					scn->AddCamera(cam);
				}
				break;

			case OBJ_PTLIGHT:
				{
					PointLight *lt = (PointLight*)objptr;
					scn->AddLight(lt);
				}
				break;

			case OBJ_SPLIGHT:
				{
					SpotLight *lt = (SpotLight*)objptr;
					scn->AddLight(lt);
				}
				break;

			case OBJ_CURVE:
				{
                    CatmullRomSpline *spline = (CatmullRomSpline*)objptr;
					scn->AddCurve(spline);
				}
				break;
			}

			break;

		default:
			SkipChunk(file, chunk);
		}
	}

	CloseHandle(file);
	

	// check if there is a normals file in the same dir and load them, or else calculate them
	if(!LoadNormalsFromFile((SceneFileName + string(".normals")).c_str(), scn)) {
		std::list<Object*>::iterator objiter = scn->GetObjectsList()->begin();
		while(objiter != scn->GetObjectsList()->end()) {
			(*objiter++)->GetTriMesh()->CalculateNormals();
		}
		if(SaveNormalFile) SaveNormalsToFile((SceneFileName + string(".normals")).c_str(), scn);
	}

	*scene = scn;
    return true;
}



bool SceneLoader::LoadObject(const char *fname, const char *ObjectName, Object **obj) {
	if(!gc) return false;

	if(!LoadMaterials(fname, &mat)) return false;

	//ifstream file(fname);
	//if(!file.is_open()) return false;
	HANDLE file = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	assert(file != NULL);
	eof = false;

	ChunkHeader chunk = ReadChunkHeader(file);
	if(chunk.id != Chunk_3DSMain) {
		CloseHandle(file);
		return false;
	}

	while(!eof) {

		chunk = ReadChunkHeader(file);

		void *objptr;
		int type;

		switch(chunk.id) {
		case Chunk_Main_3DEditor:
			break;	// dont skip

		case Chunk_Edit_Object:
			type = ReadObject(file, chunk, &objptr);
			if(type == OBJ_MESH) {
				Object *object = (Object*)objptr;
				if(!strcmp(object->name.c_str(), ObjectName)) {
					object->GetTriMesh()->CalculateNormals();
					*obj = object;
					CloseHandle(file);
                    return true;
				}
			}
			break;

		default:
			SkipChunk(file, chunk);
		}
	}

	CloseHandle(file);
	return false;
}



bool FindChunk(HANDLE file, word ChunkID) {

	ChunkHeader chunk = ReadChunkHeader(file);

	while(chunk.id != ChunkID) {
		SkipChunk(file, chunk);
		chunk = ReadChunkHeader(file);
	}

	return chunk.id == ChunkID;
}



bool SceneLoader::LoadMaterials(const char *fname, Material **materials) {
	if(!materials) return false;

	//ifstream file(fname);
	//if(!file.is_open()) return false;
	HANDLE file = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	assert(file != NULL);
	eof = false;

	ChunkHeader chunk;

	chunk = ReadChunkHeader(file);
	if(chunk.id != Chunk_3DSMain) {
		CloseHandle(file);
		return false;
	}

	if(!FindChunk(file, Chunk_Main_3DEditor)) {
		CloseHandle(file);
		return false;
	}

	std::vector<Material> mats;
	
	while(!eof) {

		chunk = ReadChunkHeader(file);

		if(chunk.id == Chunk_Edit_Material) {
            Material mat = ReadMaterial(file, chunk);
			mats.push_back(mat);
		} else {
			SkipChunk(file, chunk);
		}
	}

	MatCount = (dword)mats.size();

	if(*materials) delete [] *materials;
	Material *m = new Material[MatCount];

	for(dword i=0; i<MatCount; i++) {
		m[i] = mats[i];
	}

	*materials = m;

	CloseHandle(file);
	return true;
}


TexMap ReadTextureMap(HANDLE file, const ChunkHeader &ch) {
	assert(ch.id == Chunk_Mat_TextureMap || ch.id == Chunk_Mat_TextureMap2 || ch.id == Chunk_Mat_OpacityMap || ch.id == Chunk_Mat_BumpMap || ch.id == Chunk_Mat_ReflectionMap || ch.id == Chunk_Mat_SelfIlluminationMap);

	TexMap map;
	Percent p = ReadPercent(file);
	map.intensity = p.FloatPercent;	

	switch(ch.id) {
	case Chunk_Mat_TextureMap:
		map.type = TextureMap;
		break;

	case Chunk_Mat_TextureMap2:
		map.type = DetailMap;
		break;

	case Chunk_Mat_OpacityMap:
		map.type = OpacityMap;
		break;

	case Chunk_Mat_BumpMap:
		map.type = BumpMap;
		break;

	case Chunk_Mat_ReflectionMap:
		map.type = EnvironmentMap;
		break;

	case Chunk_Mat_SelfIlluminationMap:
		map.type = LightMap;
		break;
	default:
		assert(0);
	}

	ChunkHeader chunk = ReadChunkHeader(file);
	assert(chunk.id == Chunk_Map_FileName);

	map.filename = ReadString(file);
	return map;
}





Material ReadMaterial(HANDLE file, const ChunkHeader &ch) {

	Material mat;

	assert(ch.id == Chunk_Edit_Material);

	ReadCounter = HeaderSize;
	dword ChunkSize = ch.size;

	while(ReadCounter < ChunkSize) {
		ChunkHeader chunk = ReadChunkHeader(file);

		Percent p;
		TexMap map;
		Texture *tex;

		switch(chunk.id) {
		case Chunk_Mat_Name:
			mat.name = ReadString(file);
			break;

		case Chunk_Mat_AmbientColor:
			mat.Ambient = ReadColor(file);
			break;

		case Chunk_Mat_DiffuseColor:
			mat.Diffuse = ReadColor(file);
			break;

		case Chunk_Mat_SpecularColor:
			mat.Specular = ReadColor(file);
			break;

		case Chunk_Mat_Specular:
			p = ReadPercent(file);
			mat.Power = (float)p.IntPercent;
			if(mat.Power > 0.0f) mat.SpecularEnable = true;
			break;

		case Chunk_Mat_SpecularIntensity:
			p = ReadPercent(file);
			mat.Specular.r *= p.FloatPercent;
			mat.Specular.g *= p.FloatPercent;
			mat.Specular.b *= p.FloatPercent;
			break;

		case Chunk_Mat_Transparency:
			p = ReadPercent(file);
			mat.Alpha = 1.0f - p.FloatPercent;
			break;

		case Chunk_Mat_SelfIllumination:
			p = ReadPercent(file);
			mat.Emissive = Color(p.FloatPercent);
			break;

		case Chunk_Mat_TextureMap:
		case Chunk_Mat_TextureMap2:
		case Chunk_Mat_OpacityMap:
		case Chunk_Mat_SelfIlluminationMap:
			map = ReadTextureMap(file, chunk);
			tex = gc->texman->LoadTexture((datapath + map.filename).c_str());
			mat.SetTexture(tex, map.type);
			// RESTORATION: ugh ... (hack)
			if(chunk.id == Chunk_Mat_TextureMap && tex) {
				D3DLOCKED_RECT rect;
				D3DSURFACE_DESC desc;

				tex->GetLevelDesc(0, &desc);
				if(desc.Format != D3DFMT_A8R8G8B8) break;	// no alpha channel

				if(tex->LockRect(0, &rect, 0, D3DLOCK_READONLY) != D3D_OK) {
					break;
				}
				unsigned int *pix = (unsigned int*)rect.pBits;
				for(int i=0; i<(int)(desc.Height * desc.Width); i++) {
					if((*pix++ >> 24) < 0xff) {
						mat.HasTransparentTex = true;
						break;
					}
				}
				tex->UnlockRect(0);
			}
            break;

		case Chunk_Mat_ReflectionMap:
			map = ReadTextureMap(file, chunk);
			mat.SetTexture(gc->texman->LoadTexture((datapath + map.filename).c_str()), map.type);
			mat.EnvBlend = map.intensity;
            break;

		case Chunk_Mat_BumpMap:
			map = ReadTextureMap(file, chunk);
			mat.SetTexture(gc->texman->LoadTexture((datapath + map.filename).c_str()), map.type);
			mat.BumpIntensity = map.intensity;
            break;

		default:
			SkipChunk(file, chunk);
		}
	}

	return mat;
}            





////////////////////////////////////////////////////
byte ReadByte(HANDLE file) {
	byte val;
	dword numread;
	ReadFile(file, &val, sizeof(byte), &numread, NULL);
	if(numread < sizeof(byte)) eof = true;
	ReadCounter++;
	return val;
}

word ReadWord(HANDLE file) {
	word val;
	dword numread;
	ReadFile(file, &val, sizeof(word), &numread, NULL);
	if(numread < sizeof(word)) eof = true;
	ReadCounter += sizeof(word);
	return val;
}

dword ReadDword(HANDLE file) {
	dword val;
	dword numread;
	ReadFile(file, &val, sizeof(dword), &numread, NULL);
	if(numread < sizeof(dword)) eof = true;
	ReadCounter += sizeof(dword);
	return val;
}

float ReadFloat(HANDLE file) {
	float val;
	dword numread;
	ReadFile(file, &val, sizeof(float), &numread, NULL);
	if(numread < sizeof(float)) eof = true;
	ReadCounter += sizeof(float);
	return val;
}
/*
byte ReadByte(HANDLE file) {
	byte val;
	file.read((char*)&val, 1);
	ReadCounter++;
	return val;
}

word ReadWord(HANDLE file) {
	word val;
	file.read((char*)&val, sizeof(word));
	ReadCounter += sizeof(word);
	return val;
}

dword ReadDword(HANDLE file) {
	dword val;
	file.read((char*)&val, sizeof(dword));
	ReadCounter += sizeof(dword);
	return val;
}

float ReadFloat(HANDLE file) {
	float val;
	file.read((char*)&val, sizeof(float));
	ReadCounter += sizeof(float);
	return val;
}
*/
Vector3 ReadVector(HANDLE file, bool FlipYZ) {
	Vector3 vector;
	vector.x = ReadFloat(file);
	if(!FlipYZ) vector.y = ReadFloat(file);
	vector.z = ReadFloat(file);
	if(FlipYZ) vector.y = ReadFloat(file);
	return vector;		
}

string ReadString(HANDLE file) {
	string str;
	char c;
	while(c = (char)ReadByte(file)) {
		str.push_back(c);
	}
	ReadCounter++;

	return str;
}
/*
string ReadString(HANDLE file) {
	string str;
	char c;
	while(c = file.get()) {
		str.push_back(c);
		ReadCounter++;
	}
	str.push_back('\0');
	ReadCounter++;

	return str;
}
*/

Color ReadColor(HANDLE file) {
	ChunkHeader chunk = ReadChunkHeader(file);
	if(chunk.id < 0x0010 || chunk.id > 0x0013) return Color(-1.0f, -1.0f, -1.0f);

	Color color;

	if(chunk.id == Chunk_Color_Byte3 || chunk.id == Chunk_Color_GammaByte3) {
		byte r = ReadByte(file);
		byte g = ReadByte(file);
		byte b = ReadByte(file);
		color = Color(r, g, b);
	} else {
		color.r = ReadFloat(file);
		color.g = ReadFloat(file);
		color.b = ReadFloat(file);
	}

	return color;
}

Percent ReadPercent(HANDLE file) {
	ChunkHeader chunk = ReadChunkHeader(file);
	Percent p;
	if(chunk.id != Chunk_PercentInt && chunk.id != Chunk_PercentFloat) return p;

	if(chunk.id == Chunk_PercentInt) {
		p = Percent(ReadWord(file));
	} else {
		p = Percent(ReadFloat(file));
	}

	return p;
}


ChunkHeader ReadChunkHeader(HANDLE file) {
	ChunkHeader chunk;
	chunk.id = (ChunkID)ReadWord(file);
	chunk.size = ReadDword(file);
	return chunk;
}

void SkipChunk(HANDLE file, const ChunkHeader &chunk) {
	//file.ignore(chunk.size - HeaderSize);
	SetFilePointer(file, chunk.size - HeaderSize, 0, FILE_CURRENT);
	ReadCounter += chunk.size - HeaderSize;
}

void SkipBytes(HANDLE file, dword bytes) {
	SetFilePointer(file, bytes, 0, FILE_CURRENT);
	ReadCounter += bytes;
}

Material *FindMaterial(string name) {
	dword i=0;
	while(i < MatCount) {
		if(mat[i].name == name) return &mat[i];
		i++;
	}

	return 0;
}

///////////////////// Read Object Function //////////////////////
int ReadObject(HANDLE file, const ChunkHeader &ch, void **obj) {
	if(!obj || !gc) return -1;

	ReadCounter = HeaderSize;	// reset the global read counter

	string name = ReadString(file);

	ChunkHeader chunk;
	chunk = ReadChunkHeader(file);
	if(chunk.id == Chunk_Obj_TriMesh) {
		// object is a trimesh... load it
		Vertex *varray;
		Triangle *tarray;
		dword VertexCount=0, TriCount=0;
		Material mat;
		Base base;
		Vector3 translation;

		bool curve = true;

		dword ObjChunkSize = ch.size;

		while(ReadCounter < ObjChunkSize) {	// make sure we only read subchunks of this object chunk
			//assert(!file.eof());
			assert(!eof);
			chunk = ReadChunkHeader(file);

            switch(chunk.id) {
			case Chunk_TriMesh_VertexList:
				VertexCount = (dword)ReadWord(file);
				varray = new Vertex[VertexCount];

				for(dword i=0; i<VertexCount; i++) {
					varray[i].pos = ReadVector(file);
				}

				break;

			case Chunk_TriMesh_FaceDesc:
				curve = false;	// it is a real object not a curve since it has triangles
				TriCount = (dword)ReadWord(file);
				tarray = new Triangle[TriCount];

				for(dword i=0; i<TriCount; i++) {
					tarray[i].vertices[0] = (Index)ReadWord(file);	// 
					tarray[i].vertices[2] = (Index)ReadWord(file);	// flip order to CW
					tarray[i].vertices[1] = (Index)ReadWord(file);	//
					ReadWord(file);	// discard edge visibility flags
				}
				break;

			case Chunk_Face_Material:
				mat = *FindMaterial(ReadString(file));

				SkipBytes(file, ReadWord(file)<<1);
				break;

			case Chunk_TriMesh_TexCoords:
				assert((dword)ReadWord(file) == VertexCount);

				for(dword i=0; i<VertexCount; i++) {
					varray[i].tex[0].u = varray[i].tex[1].u = ReadFloat(file);
					varray[i].tex[0].v = varray[i].tex[1].v = -ReadFloat(file);
				}
				break;

			case Chunk_TriMesh_SmoothingGroup:
				// **TODO** abide by smoothing groups duplicate vertices, weld others etc
				SkipChunk(file, chunk);
				break;

			case Chunk_TriMesh_WorldTransform:
				base.i = ReadVector(file);
				base.k = ReadVector(file);	// flip
				base.j = ReadVector(file);
				translation = ReadVector(file);
				break;

			default:
				SkipChunk(file, chunk);
			}
		}

		if(curve) {
			CatmullRomSpline *spline = new CatmullRomSpline;
			spline->name = name;
			for(dword i=0; i<VertexCount; i++) {
				spline->AddControlPoint(varray[i].pos);
			}

			*obj = spline;
			return OBJ_CURVE;
		} else {

			base.i.Normalize();
			base.j.Normalize();
			base.k.Normalize();
			Matrix3x3 RotXForm = base.CreateRotationMatrix();
			RotXForm.OrthoNormalize();
			
			for(dword i=0; i<VertexCount; i++) {
				varray[i].pos.Translate(-translation.x, -translation.y, -translation.z);
				varray[i].pos.Transform(RotXForm.Transposed());
			}

            Object *object = new Object(gc);
			object->name = name;
			object->GetTriMesh()->SetData(varray, tarray, VertexCount, TriCount);
			object->material = mat;
			object->SetRotation(RotXForm);
			object->SetTranslation(translation.x, translation.y, translation.z);
			*obj = object;

			return OBJ_MESH;
		}
	} else {

		if(chunk.id == Chunk_Obj_Light) {

			dword ObjChunkSize = ch.size;

			Vector3 pos = ReadVector(file);
			Color color = ReadColor(file);

			Vector3 SpotTarget;
			float InnerCone, OuterCone;
			bool spot = false;
			bool att = false;
			bool CastShadows = false;
			float AttEnd = 10000.0f;
			float Intensity = 1.0f;

			while(ReadCounter < ObjChunkSize) {

				chunk = ReadChunkHeader(file);

				switch(chunk.id) {
				case Chunk_Light_SpotLight:
					spot = true;
					SpotTarget = ReadVector(file);
					InnerCone = ReadFloat(file) / 180.0f;
					OuterCone = ReadFloat(file) / 180.0f;
					break;

				case Chunk_Light_Attenuation:
					att = true;
					break;
				
				case Chunk_Light_AttenuationEnd:
					AttEnd = ReadFloat(file);
					break;

				case Chunk_Light_Intensity:
					Intensity = ReadFloat(file);
					break;

				case Chunk_Spot_CastShadows:
					CastShadows = true;
					break;

				default:
					SkipChunk(file, chunk);
				}
			}

			Light *light;
			if(spot) {
				light = new TargetSpotLight(pos, SpotTarget, InnerCone, OuterCone);
			} else {
				light = new PointLight(pos);
			}
			light->SetColor(color);			
			light->SetShadowCasting(CastShadows);
			light->SetIntensity(Intensity);
			light->name = name;

			*obj = light;
			return spot ? OBJ_SPLIGHT : OBJ_PTLIGHT;
		}

		if(chunk.id == Chunk_Obj_Camera) {
			Camera *cam = new Camera;
			Vector3 pos = ReadVector(file);
			Vector3 targ = ReadVector(file);
			float roll = ReadFloat(file);
			float FOV = ReadFloat(file);

			Vector3 up = VECTOR3_J;
			Vector3 view = targ - pos;
			up.Rotate(view.Normalized(), roll);

			cam->SetCamera(pos, targ, up);
			cam->name = name;
			cam->SetFOV(DEGTORAD(FOV) / 1.33333f);

			*obj = cam;
			return OBJ_CAMERA;
		}
	}

	return -1;  // should have already left by now, if not something is wrong
}


////////////////////////////////////////////////////////////////////////////////
// functions to save/load normals from file

void WriteByte(HANDLE file, byte val) {
	dword junk;
	WriteFile(file, &val, sizeof(byte), &junk, 0);
}

void WriteDword(HANDLE file, dword val) {
	dword junk;
	WriteFile(file, &val, sizeof(dword), &junk, 0);
	assert(junk == sizeof(dword));
}

void WriteFloat(HANDLE file, float val) {
	dword junk;
	WriteFile(file, &val, sizeof(float), &junk, 0);
	assert(junk == sizeof(float));
}

void WriteString(HANDLE file, string str) {
	dword junk;
	for(dword i=0; i<(dword)str.size(); i++) {
		WriteFile(file, &str[i], sizeof(char), &junk, 0);
	}
}


void SaveNormalsToFile(const char *fname, Scene *scene) {

	HANDLE file = CreateFile(fname, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	assert(file);

	WriteDword(file, (dword)scene->GetObjectsList()->size());

	std::list<Object*>::iterator objiter = scene->GetObjectsList()->begin();
	while(objiter != scene->GetObjectsList()->end()) {
		WriteString(file, (*objiter)->name);
		dword VertexCount = (*objiter)->GetTriMesh()->GetVertexCount();
		WriteDword(file, VertexCount);

		const Vertex *varray = (*objiter)->GetTriMesh()->GetVertexArray();
		for(dword i=0; i<VertexCount; i++) {
			WriteFloat(file, varray[i].normal.x);
			WriteFloat(file, varray[i].normal.y);
			WriteFloat(file, varray[i].normal.z);
		}

		objiter++;
	}

	CloseHandle(file);
}

bool LoadNormalsFromFile(const char *fname, Scene *scene) {

	HANDLE file = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(!file) return false;

	eof = false;
	ReadCounter = 0;

	dword FileSize = SetFilePointer(file, 0, 0, FILE_END) - SetFilePointer(file, 0, 0, FILE_BEGIN);

	SetFilePointer(file, 0, 0, FILE_BEGIN);

	dword ObjectCount = ReadDword(file);
	if(ObjectCount != scene->GetObjectsList()->size()) {	// detect changes
		CloseHandle(file);
		return false;
	}
    
	while(SceneLoader::ReadCounter < FileSize) {
		string name = ReadString(file);
		dword VertexCount = ReadDword(file);
		
		Object *obj = scene->GetObject(name.c_str());
		if(!obj) {
			CloseHandle(file);
			return false;
		}

		if(VertexCount != obj->GetTriMesh()->GetVertexCount()) {	// again detect changes
			CloseHandle(file);
			return false;
		}
        
		Vertex *varray = obj->GetTriMesh()->GetModVertexArray();
		for(dword i=0; i<VertexCount; i++) {
			varray[i].normal = ReadVector(file, false);
		}
	}

	CloseHandle(file);
	return true;
}

