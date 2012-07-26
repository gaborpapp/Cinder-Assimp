/*
 Copyright (C) 2011 Gabor Papp

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <vector>

/* 2.0
#include "assimp/assimp.hpp"
#include "assimp/aiScene.h"
#include "assimp/aiPostProcess.h"
*/
//* 3.0
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
//*/

#include "cinder/Cinder.h"
#include "cinder/Color.h"
#include "cinder/TriMesh.h"
#include "cinder/Stream.h"
#include "cinder/AxisAlignedBox.h"

#include "Node.h"
#include "AssimpMeshHelper.h"

namespace mndl { namespace assimp {

inline ci::Vec3f fromAssimp( const aiVector3D &v )
{
    return ci::Vec3f( v.x, v.y, v.z );
}

inline aiVector3D toAssimp( const ci::Vec3f &v )
{
    return aiVector3D( v.x, v.y, v.z );
}

inline ci::Quatf fromAssimp( const aiQuaternion &q )
{
    return ci::Quatf( q.w, q.x, q.y, q.z );
}

inline ci::Matrix44f fromAssimp( const aiMatrix4x4 &m )
{
	return ci::Matrix44f( &m.a1, true );
}

inline aiQuaternion toAssimp( const ci::Quatf &q )
{
    return aiQuaternion( q.w, q.v.x, q.v.y, q.v.z );
}

inline ci::ColorAf fromAssimp( const aiColor4D &c )
{
    return ci::ColorAf( c.r, c.g, c.b, c.a );
}

inline aiColor4D toAssimp( const ci::ColorAf &c )
{
    return aiColor4D( c.r, c.g, c.b, c.a );
}

/* 3.0
inline std::string fromAssimp( const aiString &s )
{
	return std::string( s.C_Str() );
}
*/
inline std::string fromAssimp( const aiString &s )
{
	return std::string( s.data );
}

class AssimpLoaderExc : public std::exception
{
	public:
		AssimpLoaderExc( const std::string &log ) throw()
		{
			strncpy( mMessage, log.c_str(), 512 );
		}

		virtual const char* what() const throw()
		{
			return mMessage;
		}

	private:
		char mMessage[ 513 ];
};

class AssimpNode : public mndl::Node
{
	public:
		std::vector< AssimpMeshHelperRef > mMeshes;

		// FIXME: test
		ci::Matrix44f mTransform;
		aiMatrix4x4 mAiTransform;
		const aiNode *mAiNode;
};

typedef std::shared_ptr< AssimpNode > AssimpNodeRef;

class AssimpLoader
{
	public:
		AssimpLoader() {}

		/** Constructs and does the parsing of the file **/
		AssimpLoader( ci::fs::path filename );
		~AssimpLoader();

		void update();
		void draw();

		ci::AxisAlignedBox3f getBoundingBox() const { return mBoundingBox; }

		void enableTextures( bool enable = true ) { mUsingTextures = enable; }
		void disableTextures() { mUsingTextures = false; }

		void enableSkinning( bool enable = true );
		void disableSkinning() { enableSkinning( false ); }

		void enableAnimation( bool enable = true ) { mEnableAnimation = enable; }
		void disableAnimation() { mEnableAnimation = false; }

	private:
		void loadAllMeshes();
		AssimpNodeRef loadNodes( const aiNode* nd, AssimpNodeRef parentRef = AssimpNodeRef() );
		AssimpMeshHelperRef convertAiMesh( const aiMesh *mesh );

		void calculateDimensions();
		void calculateBoundingBox( ci::Vec3f *min, ci::Vec3f *max );
		void calculateBoundingBoxForNode( const aiNode *nd, aiVector3D *min, aiVector3D *max, aiMatrix4x4 *trafo );

		void updateAnimation();
		void updateSkinning();
		void updateMeshes();

		std::shared_ptr< Assimp::Importer > mImporterRef; // mScene will be destroyed along with the Importer object
		ci::fs::path mFilePath; /// model path
		const aiScene *mScene;

		ci::AxisAlignedBox3f mBoundingBox;

		AssimpNodeRef mRootNode; /// root node of scene

		std::vector< AssimpNodeRef > mMeshNodes; /// nodes with meshes
		std::vector< AssimpMeshHelperRef > mModelMeshes; /// all meshes

		bool mUsingMaterials;
		bool mUsingNormals;
		bool mUsingTextures;
		bool mUsingColors;
		bool mEnableSkinning;
		bool mEnableAnimation;
};

} } // namespace mndl::assimp

