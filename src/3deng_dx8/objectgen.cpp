#include "objectgen.h"
#include "n3dmath.h"

struct Quad {
	Index verts[4];
	Vector3 norm;
};

///////////////////////////////////////////////
//  --==( Generate a tesselated plane )==--  //
///////////////////////////////////////////////

void ObjGen::CreatePlane(GraphicsContext *gc, float size, dword subdivisions, TriMesh **mesh, byte LODLevels) {

	dword VertexNum = (subdivisions+2) * (subdivisions+2);
	dword TriNum = ((subdivisions+1) * (subdivisions+1)) << 1;
	int QuadNum = TriNum >> 1;
	int QuadsRow = subdivisions+1;		// quads per row
	int VertsRow = subdivisions+2;		// vertices per row
	int VertsCol = VertsRow;

    Vertex *varray = new Vertex[VertexNum];
	Triangle *tarray = new Triangle[TriNum];

	for(int j=0; j<VertsCol; j++) {
		for(int i=0; i<VertsRow; i++) {

			float u = (float)i/(float)QuadsRow;
			float v = (float)j/(float)QuadsRow;
			varray[j*VertsRow+i] = Vertex(Vector3(u - 0.5f, 1.0f - v - 0.5f, 0), u, v, 0x00ffffff);
			varray[j*VertsRow+i].pos *= size;
		}
	}

	// first seperate the quads and then triangulate
	Quad *quads = new Quad[QuadNum];

	for(int i=0; i<QuadNum; i++) {
		quads[i].verts[0] = i + i/QuadsRow;
		quads[i].verts[1] = quads[i].verts[0] + 1;
		quads[i].verts[2] = quads[i].verts[0] + VertsRow;
		quads[i].verts[3] = quads[i].verts[1] + VertsRow;
	}

	for(int i=0; i<QuadNum; i++) {
		tarray[i<<1] = Triangle(quads[i].verts[0], quads[i].verts[1], quads[i].verts[3]);
		tarray[(i<<1)+1] = Triangle(quads[i].verts[0], quads[i].verts[3], quads[i].verts[2]);
	}

	for(dword i=0; i<VertexNum; i++) {
		varray[i].normal = Vector3(0, 0, -1);
	}

	*mesh = new TriMesh(LODLevels, gc);
	(*mesh)->SetData(varray, tarray, VertexNum, TriNum);

	delete [] quads;
	delete [] varray;
	delete [] tarray;
}

	


/////////////////////////////////////////////////////////
//  --==( Generate Cube without shared vertices )==--  //
/////////////////////////////////////////////////////////

void ObjGen::CreateCube(GraphicsContext *gc, float size, TriMesh **mesh, byte LODLevels) {

	Vertex verts[36];
	verts[0] = Vertex(Vector3(-1, +1, -1),	0, 0);
	verts[1] = Vertex(Vector3(+1, +1, -1),	1, 0);
	verts[2] = Vertex(Vector3(+1, -1, -1),	1, 1);
	verts[3] = Vertex(Vector3(-1, +1, -1),	0, 0);
	verts[4] = Vertex(Vector3(+1, -1, -1),	1, 1);
	verts[5] = Vertex(Vector3(-1, -1, -1),	0, 1);
	verts[6] = Vertex(Vector3(-1, +1, +1),	0, 0);
	verts[7] = Vertex(Vector3(-1, +1, -1),	1, 0);
	verts[8] = Vertex(Vector3(-1, -1, -1),	1, 1);
	verts[9] = Vertex(Vector3(-1, +1, +1),	0, 0);
	verts[10] = Vertex(Vector3(-1, -1, -1),	1, 1);
	verts[11] = Vertex(Vector3(-1, -1, +1),	0, 1);
	verts[12] = Vertex(Vector3(-1, +1, +1),	1, 0);
	verts[13] = Vertex(Vector3(-1, -1, +1),	1, 1);
	verts[14] = Vertex(Vector3(+1, +1, +1),	0, 0);
	verts[15] = Vertex(Vector3(+1, +1, +1),	0, 0);
	verts[16] = Vertex(Vector3(-1, -1, +1),	1, 1);
	verts[17] = Vertex(Vector3(+1, -1, +1),	0, 1);
	verts[18] = Vertex(Vector3(+1, +1, -1),	0, 0);
	verts[19] = Vertex(Vector3(+1, +1, +1),	1, 0);
	verts[20] = Vertex(Vector3(+1, -1, +1),	1, 1);
	verts[21] = Vertex(Vector3(+1, +1, -1),	0, 0);
	verts[22] = Vertex(Vector3(+1, -1, +1),	1, 1);
	verts[23] = Vertex(Vector3(+1, -1, -1),	0, 1);
	verts[24] = Vertex(Vector3(-1, +1, -1),	0, 1);
	verts[25] = Vertex(Vector3(+1, +1, +1),	1, 0);
	verts[26] = Vertex(Vector3(+1, +1, -1),	1, 1);
	verts[27] = Vertex(Vector3(-1, +1, +1),	0, 0);
	verts[28] = Vertex(Vector3(+1, +1, +1),	1, 0);
	verts[29] = Vertex(Vector3(-1, +1, -1),	0, 1);
	verts[30] = Vertex(Vector3(-1, -1, +1),	0, 1);
	verts[31] = Vertex(Vector3(-1, -1, -1),	0, 0);
	verts[32] = Vertex(Vector3(+1, -1, +1),	1, 1);
	verts[33] = Vertex(Vector3(-1, -1, -1),	0, 0);
	verts[34] = Vertex(Vector3(+1, -1, -1),	1, 0);
	verts[35] = Vertex(Vector3(+1, -1, +1),	1, 1);

	Triangle tri[12];
	for(int i=0; i<12; i++) {
		tri[i].vertices[0] = i*3;
		tri[i].vertices[1] = i*3+1;
		tri[i].vertices[2] = i*3+2;
	}	

	for(int i=0; i<36; i++) {
		verts[i].pos *= size / 2.0f;
		verts[i].color = 0x00ffffff;
	}

	TriMesh *tmpmesh = new TriMesh(LODLevels, gc);
	tmpmesh->SetData(verts, tri, 36, 12);

	tmpmesh->CalculateNormals();

	*mesh = tmpmesh;
}


/////////////////////////////////////
//  --==( Generate Cylinder )==--  //
/////////////////////////////////////
/*
void ObjGen::CreateCylinder(float radius, float height, int sides, int hsegs, bool caps, TriMesh **mesh, byte LODLevels) {

	int slices = hsegs + 1;
	unsigned long vertex_count = sides * slices + (caps ? (sides + 1)<<1 : 0);
	unsigned long tri_count = (sides << 1) * (slices - 1) + (caps ? sides : 0);

	Vertex *varray = new Vertex[vertex_count];
	Triangle *tarray = new Triangle[tri_count];

	Vertex *vptr = varray;
	Triangle *tptr = tarray;
	
	// create a circle by rotating a vector around the y axis
	Vector3 x_axis(radius, 0, 0);
		
	float rot_rads = TwoPi / (float)sides;
	for(int i=0; i<sides; i++) {
		Vector3 pos = x_axis;
		pos.Rotate(0, rot_rads * (float)i, 0);

		*vptr++ = pos;
*/