#ifndef _3DGEOM_H_
#define _3DGEOM_H_

#include <vector>
#include "n3dmath.h"
#include "3dengtypes.h"
#include "switches.h"

struct TexCoord {
	float u, v;

	TexCoord(float u = 0.0f, float v = 0.0f);
};

class Triangle;	// fwd declaration

class Vertex {
public:
	Vector3 pos;
	float BlendFactor;
	dword BlendIndex;
	Vector3 normal;
	dword color;
	TexCoord tex[4];

	Vertex();
	Vertex(const Vector3 &position, float tu = 0.0f, float tv = 0.0f, dword color = 0x00ffffff);

	void CalculateNormal(const Vertex *vbuffer, const Triangle *triangles, long trinum);
};


class Edge {
public:
	Index vertices[2];
	Index adjfaces[2];

	Edge();
	Edge(Index v1, Index v2, Index af1 = 0, Index af2 = 0);
};


class Triangle {
public:
	Index vertices[3];
	Vector3 normal;
	dword SmoothingGroup;

	Triangle(Index v1 = 0, Index v2 = 0, Index v3 = 0);

	void CalculateNormal(Vertex *vbuffer, bool normalize=0);
};


enum TriMeshMode {TriMeshDynamic, TriMeshStatic};

class GraphicsContext;

class TriMesh {
private:
	GraphicsContext *gc;

	// LOD arrays of vertex and triangle arrays for the mesh
	Vertex **varray;
	Triangle **triarray;
	// system managed copy of the data (probably on the video ram or something)
	VertexBuffer **vbuffer;
	IndexBuffer **ibuffer;

	std::vector<dword> **AdjTriangles;
	bool *AdjValid;
	
	dword *VertexCount, *TriCount;
	byte Levels;

	bool *BuffersValid;
	bool dynamic;

	// synchronizes the system managed copy of vertices/indices with the local data
	bool UpdateSystemBuffers(byte level);
	void UpdateLODChain();

public:
	TriMesh(byte LODLevels, GraphicsContext *gc = 0);
	TriMesh(const TriMesh &mesh);
	~TriMesh();

	const TriMesh &operator =(const TriMesh &mesh);

	const Vertex *GetVertexArray(byte level = 0) const;
	const Triangle *GetTriangleArray(byte level = 0) const;
	
	Vertex *GetModVertexArray();
	Triangle *GetModTriangleArray();

	const VertexBuffer *GetVertexBuffer(byte level = 0) const;
	const IndexBuffer *GetIndexBuffer(byte level = 0) const;

	dword GetVertexCount(byte level = 0) const;
	dword GetTriangleCount(byte level = 0) const;
	byte GetLevelCount() const;

	void ChangeMode(TriMeshMode mode);
	void SetGraphicsContext(GraphicsContext *gc);
	void SetData(const Vertex *vdata, const Triangle *tridata, dword vcount, dword tricount);

	void CalculateNormals();
	void CalculateNormalsFast();
	//void CalculateEdges();
};

#endif	// _3DGEOM_H_