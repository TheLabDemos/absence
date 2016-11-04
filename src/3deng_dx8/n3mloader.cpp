#include <fstream>
#include <string>
#include "n3mloader.h"
#include "typedefs.h"
#include "textureman.h"	// material class

using namespace std;
using n3mfile::ChunkHeader;

const int HeaderSize = 6;

// helper functions
byte ReadByte(ifstream &instr);
word ReadWord(ifstream &instr);
dword ReadDword(ifstream &instr);
float ReadFloat(ifstream &instr);
string ReadString(ifstream &instr);
ChunkHeader ReadChunkHeader(ifstream &instr);
void Skip(ifstream &instr, dword bytestoskip);
void SkipChunk(ifstream &instr, const ChunkHeader &ch);

dword MakeStrDword(char *str);
dword ReverseByteOrder(dword val);


// loads a n3m model file to a Nuc3D Object3D
bool n3mfile::LoadModel(const char *fname, Object **obj, GraphicsContext *gc) {

	ifstream file(fname, ios::binary);
	if(!file.is_open()) return false;

	dword idtag = ReadDword(file);
	if(idtag != ReverseByteOrder(MakeStrDword("N3DM"))) return false;
	Skip(file, 4);	// file size

	Material mat;
	int VertexCount = 0;
	int TriCount = 0;
	Vertex *verts = 0;
	Triangle *tris = 0;
	int index = 0;
	string map;
	
	while(!file.eof()) {
        
		ChunkHeader header;
		header = ReadChunkHeader(file);

		switch(header.id) {
        case Chunk_ModelName:
			SkipChunk(file, header);
			break;

		case Chunk_Mat_AmbientColor:
			mat.Ambient.r = ReadFloat(file);
			mat.Ambient.g = ReadFloat(file);
			mat.Ambient.b = ReadFloat(file);
			break;

		case Chunk_Mat_DiffuseColor:
			mat.Diffuse.r = ReadFloat(file);
			mat.Diffuse.g = ReadFloat(file);
			mat.Diffuse.b = ReadFloat(file);
			break;

		case Chunk_Mat_SpecularColor:
			mat.Specular.r = ReadFloat(file);
			mat.Specular.g = ReadFloat(file);
			mat.Specular.b = ReadFloat(file);
			break;

		case Chunk_Mat_Alpha:
			mat.Alpha = ReadFloat(file);
			break;

		case Chunk_Mat_SpecularPower:
			mat.Power = ReadFloat(file);
			break;

		case Chunk_Mat_Emissive:
			mat.Emissive.r = ReadFloat(file);
			mat.Emissive.g = mat.Emissive.r;
			mat.Emissive.b = mat.Emissive.r;
			break;

		case Chunk_Map_DiffuseMap:
			Skip(file, 4);
			map = ReadString(file);
			if(map.size() > 0) {
                mat.SetTexture(gc->texman->LoadTexture(map.c_str()), TextureMap);
			}
			break;

		case Chunk_Map_ReflectMap:
			mat.EnvBlend = ReadFloat(file);
			map = ReadString(file);
			if(map.size() > 0) {
				mat.SetTexture(gc->texman->LoadTexture(map.c_str()), EnvironmentMap);
			}
			break;

		case Chunk_Map_BumpMap:
			Skip(file, 4);	// bump str
			map = ReadString(file);
			if(map.size() > 0) {
				mat.SetTexture(gc->texman->LoadTexture(map.c_str()), BumpMap);
			}
			break;

		case Chunk_Map_AlphaMap:
			SkipChunk(file, header);
			break;

		case Chunk_Map_LightMap:
			SkipChunk(file, header);
			break;

		case Chunk_Geo_VertexNum:
			VertexCount = ReadDword(file);
			verts = new Vertex[VertexCount];
			break;

		case Chunk_Geo_TriangleNum:
			TriCount = ReadDword(file);
			tris = new Triangle[TriCount];
			break;

		case Chunk_Ver_Vertex:
			index = ReadDword(file);
			verts[index].pos.x = ReadFloat(file);
			verts[index].pos.y = ReadFloat(file);
			verts[index].pos.z = ReadFloat(file);
			verts[index].normal.x = ReadFloat(file);
			verts[index].normal.y = ReadFloat(file);
			verts[index].normal.z = ReadFloat(file);
			verts[index].tex[0].u = ReadFloat(file);
			verts[index].tex[0].v = ReadFloat(file);
			break;

		case Chunk_Tri_Triangle:
			index = ReadDword(file);
			tris[index].vertices[0] = (Index)ReadDword(file);
			tris[index].vertices[1] = (Index)ReadDword(file);
			tris[index].vertices[2] = (Index)ReadDword(file);
			tris[index].normal.x = ReadFloat(file);
			tris[index].normal.y = ReadFloat(file);
			tris[index].normal.z = ReadFloat(file);
			break;

		default: break;
		}
	}

	file.close();

	Object *model = new Object(gc);
	TriMesh *mesh = model->GetTriMesh();
	
	mesh->SetData(verts, tris, VertexCount, TriCount);

	*obj = model;   
	
	return true;
}


byte ReadByte(ifstream &instr) {
	return (byte)instr.get();
}

word ReadWord(ifstream &instr) {
	word val;
	instr.read((char*)&val, sizeof(word));
	return val;
}

dword ReadDword(ifstream &instr) {
	dword val;
	instr.read((char*)&val, sizeof(dword));
	return val;
}

float ReadFloat(ifstream &instr) {
	float val;
	instr.read((char*)&val, sizeof(float));
	return val;
}

string ReadString(ifstream &instr) {
	string str;
	char ch;
	while(!instr.eof() && (ch = instr.get())) str.push_back(ch);
	return str;
}

void Skip(ifstream &instr, dword bytestoskip) {
	instr.ignore(bytestoskip);
}

void SkipChunk(ifstream &instr, const ChunkHeader &ch) {
	instr.ignore(ch.size - HeaderSize);
}

ChunkHeader ReadChunkHeader(ifstream &instr) {
	ChunkHeader ch;
	ch.id = (ChunkID)ReadWord(instr);
	ch.size = ReadDword(instr);
	return ch;
}

dword MakeStrDword(char *str) {
	return (dword)(((byte)str[0] << 24) | ((byte)str[1] << 16) | ((byte)str[2] << 8) | (byte)str[3]);
}

dword ReverseByteOrder(dword val) {
	return (dword)((byte)val << 24 | ((byte)(val >> 8) << 16) | ((byte)(val >> 16) << 8) | (byte)(val >> 24));
}


