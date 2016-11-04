#include <cassert>
#include "3dgeom.h"
#include "3dengine.h"

using std::vector;

TexCoord::TexCoord(float u, float v) {
	this->u = u;
	this->v = v;
}

// Vertex class implementation

Vertex::Vertex() {
	color = 0x00ffffff;
	memset(tex, 0, 4*sizeof(TexCoord));
}

Vertex::Vertex(const Vector3 &position, float tu, float tv, dword color) {
	pos = position;
	tex[0].u = tex[1].u = tex[2].u = tex[3].u = tu;
	tex[0].v = tex[1].v = tex[2].v = tex[3].v = tv;
	this->color = color;
}

void Vertex::CalculateNormal(const Vertex *vbuffer, const Triangle *triangles, long trinum) {

	// find the position of the curent vertex in the vertex buffer
	dword index = (dword)(this - vbuffer);
	
	normal = Vector3(0, 0, 0);
	const Triangle *tri = triangles;
	for(int i=0; i<trinum; i++, tri++) {
		if(tri->vertices[0] == index || tri->vertices[1] == index || tri->vertices[2] == index) {
			normal += tri->normal;
		}
	}

	normal.Normalize();
}

/////////// Edge class implementation ///////////

Edge::Edge() {
	vertices[0] = vertices[1] = adjfaces[0] = adjfaces[1] = 0;
}

Edge::Edge(Index v1, Index v2, Index af1, Index af2) {
	vertices[0] = v1;
	vertices[1] = v2;
	adjfaces[0] = af1;
	adjfaces[1] = af2;
}

/////////// Triangle class implementation /////////////
Triangle::Triangle(Index v1, Index v2, Index v3) {
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
}

void Triangle::CalculateNormal(Vertex *vbuffer, bool normalize) {
	Vector3 v1 = vbuffer[vertices[1]].pos - vbuffer[vertices[0]].pos;
	Vector3 v2 = vbuffer[vertices[2]].pos - vbuffer[vertices[0]].pos;
	normal = v1.CrossProduct(v2);
	if(normalize) normal.Normalize();
}


/////////////// Triangular Mesh implementation /////////////

TriMesh::TriMesh(byte LODLevels, GraphicsContext *gc) {
	memset(this, 0, sizeof(TriMesh));
	this->gc = gc;
	Levels = LODLevels;

	varray = new Vertex*[Levels];
	memset(varray, 0, Levels * sizeof(Vertex*));
	
	triarray = new Triangle*[Levels];
	memset(triarray, 0, Levels * sizeof(Triangle*));
	
	vbuffer = new VertexBuffer*[Levels];
	memset(vbuffer, 0, Levels * sizeof(VertexBuffer*));

	ibuffer = new IndexBuffer*[Levels];
	memset(ibuffer, 0, Levels * sizeof(IndexBuffer*));

	AdjTriangles = new std::vector<dword>*[Levels];
	memset(AdjTriangles, 0, Levels * sizeof(std::vector<dword>*));

	VertexCount = new dword[Levels];
	TriCount = new dword[Levels];

	BuffersValid = new bool[Levels];
	memset(BuffersValid, 0, Levels * sizeof(bool));

	AdjValid = new bool[Levels];
	memset(AdjValid, 0, Levels * sizeof(bool));
}

TriMesh::TriMesh(const TriMesh &mesh) {

	memcpy(this, &mesh, sizeof(TriMesh));

	BuffersValid = new bool[Levels];
	memset(BuffersValid, 0, Levels * sizeof(bool));
    
	varray = new Vertex*[Levels];
	triarray = new Triangle*[Levels];
	vbuffer = new VertexBuffer*[Levels];
	ibuffer = new IndexBuffer*[Levels];

	VertexCount = new dword[Levels];
	TriCount = new dword[Levels];

	for(int i=0; i<Levels; i++) {
		
		VertexCount[i] = mesh.VertexCount[i];
		TriCount[i] = mesh.TriCount[i];
		
        varray[i] = new Vertex[VertexCount[i]];
		triarray[i] = new Triangle[TriCount[i]];
		
		memcpy(varray[i], mesh.varray[i], VertexCount[i] * sizeof(Vertex));
		memcpy(triarray[i], mesh.triarray[i], TriCount[i] * sizeof(Triangle));

		vbuffer[i] = 0;
		ibuffer[i] = 0;

		UpdateSystemBuffers(i);
	}
}

TriMesh::~TriMesh() {
	if(varray) {
		for(int i=0; i<Levels; i++) {
            delete [] varray[i];
		}
		delete [] varray;
	}

	if(triarray) {
		for(int i=0; i<Levels; i++) {
			delete [] triarray[i];
		}
		delete triarray;
	}
	
	if(vbuffer) {
		for(int i=0; i<Levels; i++) {
			if(vbuffer[i]) vbuffer[i]->Release();
		}
	}
	if(ibuffer) {
		for(int i=0; i<Levels; i++) {
			if(ibuffer[i]) ibuffer[i]->Release();
		}
	}

	if(AdjTriangles) {
		for(int i=0; i<Levels; i++) {
			delete [] AdjTriangles[i];
		}
		delete AdjTriangles;
	}
}

const TriMesh &TriMesh::operator =(const TriMesh &mesh) {
	memcpy(this, &mesh, sizeof(TriMesh));

	BuffersValid = new bool[Levels];
	memset(BuffersValid, 0, Levels * sizeof(bool));
    
	varray = new Vertex*[Levels];
	triarray = new Triangle*[Levels];
	vbuffer = new VertexBuffer*[Levels];
	ibuffer = new IndexBuffer*[Levels];

	VertexCount = new dword[Levels];
	TriCount = new dword[Levels];

	for(int i=0; i<Levels; i++) {
		
		VertexCount[i] = mesh.VertexCount[i];
		TriCount[i] = mesh.TriCount[i];
		
        varray[i] = new Vertex[VertexCount[i]];
		triarray[i] = new Triangle[TriCount[i]];
		
		memcpy(varray[i], mesh.varray[i], VertexCount[i] * sizeof(Vertex));
		memcpy(triarray[i], mesh.triarray[i], TriCount[i] * sizeof(Triangle));

		vbuffer[i] = 0;
		ibuffer[i] = 0;

		UpdateSystemBuffers(i);
	}
	return mesh;
}


const Vertex *TriMesh::GetVertexArray(byte level) const {
	if(level >= Levels) return 0;
	return varray[level];
}

const Triangle *TriMesh::GetTriangleArray(byte level) const {
	if(level >= Levels) return 0;
	return triarray[level];
}
	

Vertex *TriMesh::GetModVertexArray() {
	memset(BuffersValid, 0, Levels * sizeof(bool));
	return varray[0];
}

Triangle *TriMesh::GetModTriangleArray() {
	memset(BuffersValid, 0, Levels * sizeof(bool));
	memset(AdjValid, 0, Levels * sizeof(bool));
	return triarray[0];
}

const VertexBuffer *TriMesh::GetVertexBuffer(byte level) const {
	if(level >= Levels) return 0;

	if(!BuffersValid[level]) {
		const_cast<TriMesh*>(this)->UpdateSystemBuffers(level);
	}
	return vbuffer[level];
}

const IndexBuffer *TriMesh::GetIndexBuffer(byte level) const {
	if(level >= Levels) return 0;

	if(!BuffersValid[level]) {
		const_cast<TriMesh*>(this)->UpdateSystemBuffers(level);
	}

	return ibuffer[level];
}

		
dword TriMesh::GetVertexCount(byte level) const {
	if(level >= Levels) return 0xdeadbeef;
	return VertexCount[level];
}

dword TriMesh::GetTriangleCount(byte level) const {
	if(level >= Levels) return 0xdeadbeef;
	return TriCount[level];
}

byte TriMesh::GetLevelCount() const {
	return Levels;
}

void TriMesh::SetGraphicsContext(GraphicsContext *gc) {
	this->gc = gc;
	memset(BuffersValid, 0, Levels * sizeof(bool));	// invalidate all system buffers in all levels
}

void TriMesh::SetData(const Vertex *vdata, const Triangle *tridata, dword vcount, dword tricount) {

	memset(BuffersValid, 0, Levels * sizeof(bool));
	memset(AdjValid, 0, Levels * sizeof(bool));
	
	if(varray[0]) delete [] varray[0];
	if(triarray[0]) delete [] triarray[0];
	varray[0] = new Vertex[vcount];
	triarray[0] = new Triangle[tricount];

	if(vdata) memcpy(varray[0], vdata, vcount * sizeof(Vertex));
	if(tridata) memcpy(triarray[0], tridata, tricount * sizeof(Triangle));
	VertexCount[0] = vcount;
	TriCount[0] = tricount;

	UpdateLODChain();
}

bool TriMesh::UpdateSystemBuffers(byte level) {

	if(!gc || level >= Levels) return false;

	if(vbuffer[level]) {
		D3DVERTEXBUFFER_DESC vbdesc;
		vbuffer[level]->GetDesc(&vbdesc);
		if(vbdesc.Size / sizeof(Vertex) != VertexCount[level]) {
			vbuffer[level]->Release();

			if(gc->D3DDevice->CreateVertexBuffer(VertexCount[level] * sizeof(Vertex), dynamic ? D3DUSAGE_DYNAMIC : 0, VertexFormat, D3DPOOL_DEFAULT, &vbuffer[level]) != D3D_OK) {
				return false;
			}
		}
	} else {
		if(gc->D3DDevice->CreateVertexBuffer(VertexCount[level] * sizeof(Vertex), dynamic ? D3DUSAGE_DYNAMIC : 0, VertexFormat, D3DPOOL_DEFAULT, &vbuffer[level]) != D3D_OK) {
			return false;
		}
	}
	
	Vertex *vbdata;
	Lock(vbuffer[level], &vbdata);
	memcpy(vbdata, varray[level], VertexCount[level] * sizeof(Vertex));
	Unlock(vbuffer[level]);

	if(ibuffer[level]) {
		D3DINDEXBUFFER_DESC ibdesc;
		ibuffer[level]->GetDesc(&ibdesc);
		if(ibdesc.Size / IndexSize != TriCount[level] * 3) {
			ibuffer[level]->Release();

			if(gc->D3DDevice->CreateIndexBuffer(TriCount[level] * 3 * IndexSize, dynamic ? D3DUSAGE_DYNAMIC : 0, IndexFormat, D3DPOOL_DEFAULT, &ibuffer[level]) != D3D_OK) {
				return false;
			}
		}
	} else {
		if(gc->D3DDevice->CreateIndexBuffer(TriCount[level] * 3 * IndexSize, dynamic ? D3DUSAGE_DYNAMIC : 0, IndexFormat, D3DPOOL_DEFAULT, &ibuffer[level]) != D3D_OK) {
			return false;
		}
	}


	Index *ibdata;
	Lock(ibuffer[level], &ibdata);
	for(dword i=0; i<TriCount[level]; i++) {
		*ibdata++ = triarray[level][i].vertices[0];
		*ibdata++ = triarray[level][i].vertices[1];
		*ibdata++ = triarray[level][i].vertices[2];
	}
	Unlock(ibuffer[level]);

	BuffersValid[level] = true;
	return true;
}

void TriMesh::UpdateLODChain() {
	for(byte i=1; i<Levels; i++) {
		// TODO: apply mesh optimization, for now, just copy as it is
		VertexCount[i] = VertexCount[0];
		TriCount[i] = TriCount[0];
		
		if(!varray[i]) varray[i] = new Vertex[VertexCount[i]];
		memcpy(varray[i], varray[0], VertexCount[i] * sizeof(Vertex));

		if(!triarray[i]) triarray[i] = new Triangle[TriCount[i]];
		memcpy(triarray[i], triarray[0], TriCount[i] * sizeof(Triangle));

		UpdateSystemBuffers(i);
	}

	if(!BuffersValid[0]) UpdateSystemBuffers(0);
}

// TODO: this will brake currently with multiple LOD levels, revise LOD normal calculation
// and revise UpdateLODChain() too
void TriMesh::CalculateNormals() {
	memset(BuffersValid, 0, Levels * sizeof(bool));

	Triangle *tri = triarray[0];
	Triangle *tend = tri + TriCount[0];

	while(tri != tend) {
		(*tri++).CalculateNormal(varray[0], false);
	}

	/*
	Vertex *vert = varray[0];
	Vertex *vend = vert + VertexCount[0];

	while(vert != vend) {
		(*vert++).CalculateNormal(varray[0], triarray[0], TriCount[0]);
	}
	*/

	if(!AdjValid[0]) {
        if(AdjTriangles[0]) delete [] AdjTriangles[0];
		AdjTriangles[0] = new std::vector<dword>[VertexCount[0]];
	}

	for(dword i=0; i<VertexCount[0]; i++) {
		if(AdjValid[0]) {
			assert(AdjTriangles[0]);
			Vector3 normal(0.0f, 0.0f, 0.0f);
			for(dword j=0; j<AdjTriangles[0][i].size(); j++) {
				normal += triarray[0][AdjTriangles[0][i][j]].normal;
			}
			normal.Normalize();
			varray[0][i].normal = normal;
		} else {

			AdjTriangles[0][i].erase(AdjTriangles[0][i].begin(), AdjTriangles[0][i].end());
			Vector3 normal(0.0f, 0.0f, 0.0f);
			for(dword j=0; j<TriCount[0]; j++) {
				if(triarray[0][j].vertices[0] == i || triarray[0][j].vertices[1] == i || triarray[0][j].vertices[2] == i) {
					AdjTriangles[0][i].push_back(j);
					normal += triarray[0][j].normal;
				}
			}
			normal.Normalize();
			varray[0][i].normal = normal;
		}
	}

	AdjValid[0] = true;
	
	UpdateLODChain();
}

void TriMesh::CalculateNormalsFast() {
	memset(BuffersValid, 0, Levels * sizeof(bool));

	Triangle *tri = triarray[0];
	Triangle *tend = tri + TriCount[0];

	while(tri != tend) {
		tri->CalculateNormal(varray[0], true);
		varray[0][tri->vertices[0]].normal = tri->normal;
		varray[0][tri->vertices[1]].normal = tri->normal;
		varray[0][tri->vertices[2]].normal = tri->normal;
		tri++;
	}

	UpdateLODChain();
}

void TriMesh::ChangeMode(TriMeshMode mode) {
	dynamic = mode == TriMeshDynamic;
}

/*

void TriMesh::CalculateEdges() {

	//build a list of triangles that share each vertex
	std::list<Triangle*> *ShareTris = new std::list<Triangle*>[VertexCount[0]];

	for(dword i=0; i<VertexCount[0]; i++) {
        for(dword j=0; j<TriCount[0]; j++) {
			Index VertexIndex = (Index)((Vertex*)&varray[0][i] - (Vertex*)&varray[0][0]);
			if(triarray[0][j].vertices[0] == VertexIndex || triarray[0][j].vertices[1] == VertexIndex || triarray[0][j].vertices[2] == VertexIndex) {
				ShareTris[i].push_back(&triarray[0][j]);	// if it references this vertex add it
			}
		}
	}

	// find the adjacent triangles of each edge
	for(dword i=0; i<TriCount[0]; i++) {
		Triangle *tri = &triarray[0][i];

		for(int j=0; j<3; j++) {
			tri->edges[j].vertices[0] = tri->vertices[j];
			tri->edges[j].vertices[1] = tri->vertices[(j+1) % 3];
			tri->edges[j].adjfaces[0] = (Index)i;
		}

		for(int j=0; j<3; j++) {
			Index v0 = (Index)((Vertex*)&varray[0][tri->edges[j].vertices[0]] - (Vertex*)&varray[0][0]);
			Index v1 = (Index)((Vertex*)&varray[0][tri->edges[j].vertices[1]] - (Vertex*)&varray[0][0]);

			std::list<Triangle*>::iterator iter = ShareTris[v0].begin();
			while(iter != ShareTris[v0].end()) {
				Index TriIndex = (Index)(*iter - &triarray[0][0]);
				
				if((TriIndex != i) && ((*iter)->vertices[0] == v1 || (*iter)->vertices[1] == v1 || (*iter)->vertices[2] == v1)) {
					tri->edges[j].adjfaces[1] = TriIndex;
					break;
				}
				iter++;
			}
			if(iter == ShareTris[v0].end()) tri->edges[j].adjfaces[1] = (Index)0xffffffff;
		}
	}

	// TODO: wield duplicate edges
}

*/