#ifndef _N3MCHUNKS_H_
#define _N3MCHUNKS_H_

enum ChunkID {
	Chunk_ModelName			= 0x3a00,		// - Model Name
	Chunk_Material			= 0x3b00,		// + Material
	Chunk_Mat_AmbientColor	= 0x3b01,		//   - Ambient Color
	Chunk_Mat_DiffuseColor	= 0x3b02,		//   - Diffuse Color
	Chunk_Mat_SpecularColor	= 0x3b03,		//   - Specular Color
	Chunk_Mat_Alpha			= 0x3b10,		//   - Alpha Transparency
	Chunk_Mat_SpecularPower	= 0x3b11,		//   - Specular Power
	Chunk_Mat_Emissive		= 0x3b12,		//   - Self Illumination (Light Emission)
	Chunk_Mat_TextureMaps	= 0x3ba0,		//   + Texture Maps
	Chunk_Map_DiffuseMap	= 0x3ba1,		//     - Diffuse Map
	Chunk_Map_ReflectMap	= 0x3ba2,		//     - Reflection Map (env)
	Chunk_Map_BumpMap		= 0x3ba3,		//     - Bump Map
	Chunk_Map_AlphaMap		= 0x3ba4,		//     - Alpha Map (opacity)
	Chunk_Map_LightMap		= 0x3ba5,		//     - Light Map
	Chunk_Geometry			= 0x3d00,		// + Geometry
	Chunk_Geo_VertexNum		= 0x3d01,		//   - Vertex Number
	Chunk_Geo_TriangleNum	= 0x3d02,		//   - Triangle Number
	Chunk_Geo_Vertices		= 0x3da0,		//   + Vertices
	Chunk_Ver_Vertex		= 0x3da1,		//     - Vertex (for each)
	Chunk_Geo_Triangles		= 0x3db0,		//   + Triangles
	Chunk_Tri_Triangle		= 0x3db1		//     - Triangle (for each)
};

#endif	// _N3MCHUNKS_H_