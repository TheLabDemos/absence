#ifndef _N3SLOADER_H_
#define _N3SLOADER_H_

#include "typedefs.h"

namespace n3sfile {

	enum LightType {
		LTYPE_POINT,
		LTYPE_TARGETSPOT,
		LTYPE_DIR,
		LTYPE_SPOT		
	};

	struct ChunkHeader {
		ChunkID id;
		dword size;
	};
}

#endif	// _N3SLOADER_H_
