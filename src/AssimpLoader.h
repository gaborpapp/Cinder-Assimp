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

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "cinder/Cinder.h"
#include "cinder/Color.h"
#include "cinder/TriMesh.h"
#include "cinder/Stream.h"

#include "AssimpMeshHelper.h"

namespace mndl { namespace assimp {

inline ci::Vec3f fromAssimp( const aiVector3D &v )
{
    return ci::Vec3f( v.x, v.y, v.z );
}

inline ci::ColorAf fromAssimp( const aiColor4D &c )
{
    return ci::ColorAf( c.r, c.g, c.b, c.a );
}

inline aiVector3D toAssimp( const ci::Vec3f &v )
{
    return aiVector3D( v.x, v.y, v.z );
}

inline aiColor4D toAssimp( const ci::ColorAf &c )
{
    return aiColor4D( c.r, c.g, c.b, c.a );
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

class AssimpLoader
{
	public:
		AssimpLoader() {}

		/** Constructs and does the parsing of the file **/
		AssimpLoader( ci::fs::path filename );
		~AssimpLoader();

		void draw();

	private:
		void loadGlResources();

		void calculateDimensions();
		void getBoundingBox( ci::Vec3f *min, ci::Vec3f *max );
		void getBoundingBoxForNode( const aiNode *nd, aiVector3D *min, aiVector3D *max, aiMatrix4x4 *trafo );

		std::shared_ptr< Assimp::Importer > mImporterRef; // mScene will be destroyed along with the Importer object
		ci::fs::path mFilePath; /// model path
		const aiScene *mScene;

		ci::Vec3f mSceneMin, mSceneMax; /// scene bounding box
		ci::Vec3f mSceneCenter;

		std::vector< AssimpMeshHelper > mModelMeshes;

		bool mUsingMaterials;
		bool mUsingNormals;
		bool mUsingTextures;
		bool mUsingColors;
};

} } // namespace mndl::assimp

