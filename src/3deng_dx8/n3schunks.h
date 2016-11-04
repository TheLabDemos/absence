#ifndef _N3SCHUNKS_H_
#define _N3SCHUNKS_H_

enum ChunkID {
	Chunk_Environment			= 0x3100,		// + Environment
	Chunk_Env_Ambient			= 0x3101,		//   - Ambient Light
	Chunk_Env_BackColor			= 0x3102,		//   - Background Fill Color
	Chunk_Camera				= 0x3200,		// + Camera
	Chunk_Cam_FieldOfView		= 0x3201,		//   - FOV angle
	Chunk_Cam_ClipPlanes		= 0x3202,		//   - Clipping Planes (Near / Far)
	Chunk_Cam_Position			= 0x3204,		//   - Position
	Chunk_Cam_Base				= 0x3203,		//   - Base Vectors i, j, k specifying orientation
	Chunk_Cam_Target			= 0x3205,		//   - Target Position
	Chunk_Light					= 0x3300,		// + Light
	Chunk_Lit_Type				= 0x3301,		//   - Light Type
	Chunk_Lit_Color				= 0x3302,		//   - Light Color
	Chunk_Lit_Intensity			= 0x3303,		//   - Light Intensity
	Chunk_Lit_Position			= 0x3304,		//   - Light Position
	Chunk_Lit_Target			= 0x3305,		//   - Target Position
	Chunk_Lit_Direction			= 0x3306,		//   - Light Direction
	Chunk_Lit_CastShadows		= 0x3307,		//   - Does the light cast shadows
	Chunk_Lit_InnerCone			= 0x3308,		//   - SpotLight Inner Cone
	Chunk_Lit_OuterCone			= 0x3309,		//   - SpotLight Outer Cone
    Chunk_Geometry				= 0x3d00,		// + Geometry
	Chunk_Geo_Object			= 0x3d01,		//   + Object
	Chunk_Material				= 0x3b00,		//     + Material
	Chunk_Mat_AmbientColor		= 0x3b01,		//       - Ambient Color
	Chunk_Mat_DiffuseColor		= 0x3b02,		//       - Diffuse Color
	Chunk_Mat_SpecularColor		= 0x3b03,		//       - Specular Color
	Chunk_Mat_Alpha				= 0x3b10,		//       - Alpha Transparency
	Chunk_Mat_SpecularPower		= 0x3b11,		//       - Specular Power
	Chunk_Mat_SpecularIntensity = 0x3b12,		//       - Specular Intensity (multiplier)
	Chunk_Mat_Emissive			= 0x3b13,		//       - Self Illumination (Light Emission)
	Chunk_Mat_TextureMaps		= 0x3ba0,		//       + Texture Maps
	Chunk_Map_DiffuseMap		= 0x3ba1,		//         - Diffuse Map
	Chunk_Map_ReflectMap		= 0x3ba2,		//         - Reflection Map (env)
	Chunk_Map_BumpMap			= 0x3ba3,		//         - Bump Map
	Chunk_Map_AlphaMap			= 0x3ba4,		//         - Alpha Map (opacity)
	Chunk_Map_LightMap			= 0x3ba5,		//         - Light Map	
	Chunk_Obj_CastShadows		= 0x3d02,		//     - Does the object cast shadows
	Chunk_Obj_Vertices			= 0x3da0,		//     + Vertices
	Chunk_Ver_Vertex			= 0x3da1,		//       - Vertex (for each)
	Chunk_Obj_Triangles			= 0x3db0,		//     + Triangles
	Chunk_Tri_Triangle			= 0x3db1,		//       - Triangle (for each)
	Chunk_Geo_Curve				= 0x3c00,		//   + Curve
	Chunk_Cur_CurveType			= 0x3c01,		//     - Curve Type
	Chunk_Cur_ControlPoint		= 0x3c02		//     - Control Point
};

#endif	// _N3SCHUNKS_H_