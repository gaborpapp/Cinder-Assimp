#pragma once

#include <vector>

#include "assimp/mesh.h"

#include "cinder/TriMesh.h"

namespace mndl { namespace assimp {

class AssimpMeshHelper
{
	public:
		// pointer to the aiMesh we represent.
		aiMesh *mAiMesh;

/*
		// VBOs
		ofVbo vbo;

		// texture
		ofTexture texture;
		*/
		std::vector< uint32_t > mIndices;

/*
		// Material 
		ofMaterial material;

		ofBlendMode blendMode;

		bool twoSided;

*/
		bool mHasChanged;

		std::vector< aiVector3D > mAnimatedPos;
		std::vector< aiVector3D > mAnimatedNorm;

		ci::TriMesh mCachedTriMesh;
		bool mValidCache;
};

} } // namespace mndl::assimp

