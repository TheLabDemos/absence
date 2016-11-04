#ifndef	_N3MLOADER_H_
#define _N3MLOADER_H_

#include "objects.h"
#include "n3mchunks.h"

namespace n3mfile {

	struct ChunkHeader {
		ChunkID id;
		dword size;
	};

    bool LoadModel(const char *fname, Object **obj, GraphicsContext *gc);
}

#endif	// _N3MLOADER_H_