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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "cinder/TriMesh.h"
#include "cinder/Stream.h"
#include "cinder/Exception.h"

namespace mndl { namespace assimp {

class AssimpLoader
{
	public:
		/** Constructs and does the parsing of the file **/
		AssimpLoader( ci::DataSourceRef dataSource );
		AssimpLoader( ci::fs::path filename );
		~AssimpLoader();

		/** Loads all the groups present in the file into a single TriMesh
		 *  \param destTriMesh the destination TriMesh, whose contents are cleared first
		 **/
		void load( ci::TriMesh *destTriMesh );

		class AssimpLoaderExc : public ci::Exception {};

	private:
		ci::Buffer mBuffer;

		void recursiveLoad( ci::TriMesh *destTriMesh, const aiScene *sc, const aiNode* nd );

		Assimp::Importer importer;
		const aiScene *scene;
};

} } // namespace mndl::assimp

