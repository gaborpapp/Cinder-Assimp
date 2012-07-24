#pragma once

#include <vector>
#include <string>

#include "assimp/mesh.h"

#include "cinder/TriMesh.h"
#include "cinder/gl/Material.h"
#include "cinder/gl/Texture.h"

namespace mndl { namespace assimp {

class AssimpMeshHelper
{
	public:
		// pointer to the aiMesh we represent.
		aiMesh *mAiMesh;

/*
		// VBOs
		ofVbo vbo;
*/
		// texture
		ci::gl::Texture mTexture;

		std::vector< uint32_t > mIndices;

		ci::gl::Material mMaterial;
/*
		ofBlendMode blendMode;

*/
		bool mTwoSided;
		bool mHasChanged;

		std::vector< aiVector3D > mAnimatedPos;
		std::vector< aiVector3D > mAnimatedNorm;

		std::string mName;
		ci::TriMesh mCachedTriMesh;
		bool mValidCache;
};

} } // namespace mndl::assimp

