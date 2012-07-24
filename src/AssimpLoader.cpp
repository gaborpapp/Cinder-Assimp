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

#include "cinder/app/App.h"
#include "cinder/CinderMath.h"
#include "cinder/Utilities.h"

#include "AssimpLoader.h"

using namespace std;
using namespace ci;

namespace mndl { namespace assimp {

static void fromAssimp( const aiMesh *aim, TriMesh *cim, AssimpMeshHelper *helper = NULL )
{
	// copy vertices
	for ( unsigned i = 0; i < aim->mNumVertices; ++i )
	{
		cim->appendVertex( fromAssimp( aim->mVertices[i] ) );
	}

	if( aim->HasNormals() )
	{
		for ( unsigned i = 0; i < aim->mNumVertices; ++i )
		{
			cim->appendNormal( fromAssimp( aim->mNormals[i] ) );
		}
	}

	// aiVector3D *	mTextureCoords [AI_MAX_NUMBER_OF_TEXTURECOORDS]
	// just one for now
#if 0
	if(aim->GetNumUVChannels()>0){
		for (int i=0; i < (int)aim->mNumVertices;i++){
			if( helper != NULL && helper->texture.getWidth() > 0.0 ){
				ofVec2f texCoord = helper->texture.getCoordFromPercent(aim->mTextureCoords[0][i].x ,aim->mTextureCoords[0][i].y);
				ofm.addTexCoord(texCoord);
			}else{
				ofm.addTexCoord(ofVec2f(aim->mTextureCoords[0][i].x ,aim->mTextureCoords[0][i].y));	
			}
		}
	}
#endif

	//aiColor4D *mColors [AI_MAX_NUMBER_OF_COLOR_SETS]
	if ( aim->GetNumColorChannels() > 0 )
	{
		for ( unsigned i = 0; i < aim->mNumVertices; ++i )
		{
			cim->appendColorRGBA( fromAssimp( aim->mColors[0][i] ) );
		}
	}

	for ( unsigned i = 0; i < aim->mNumFaces; ++i )
	{
		if ( aim->mFaces[i].mNumIndices > 3 )
		{
			throw AssimpLoaderExc( "non-triangular face found: model " +
					string( aim->mName.data ) + ", face #" +
					toString< unsigned >( i ) );
		}

		cim->appendTriangle( aim->mFaces[ i ].mIndices[ 0 ],
							 aim->mFaces[ i ].mIndices[ 1 ],
							 aim->mFaces[ i ].mIndices[ 2 ] );
	}
}

AssimpLoader::AssimpLoader( fs::path filename )
{
	unsigned flags = aiProcessPreset_TargetRealtime_MaxQuality |
					 aiProcess_Triangulate;

	mImporterRef = shared_ptr< Assimp::Importer >( new Assimp::Importer() );
	mScene = mImporterRef->ReadFile( filename.string(), flags );
	if ( !mScene )
		throw AssimpLoaderExc( mImporterRef->GetErrorString() );

	calculateDimensions();
	loadGlResources();
}

AssimpLoader::~AssimpLoader()
{
}

void AssimpLoader::calculateDimensions()
{
    getBoundingBox( &mSceneMin, &mSceneMax );
	//app::console() << mSceneMin << ", " << mSceneMax << endl;
	mSceneCenter = mSceneMin + mSceneMax / 2.f;
}

void AssimpLoader::getBoundingBox( ci::Vec3f *min, ci::Vec3f *max )
{
    aiMatrix4x4 trafo;

	aiVector3D aiMin, aiMax;
    aiMin.x = aiMin.y = aiMin.z =  1e10f;
    aiMax.x = aiMax.y = aiMax.z = -1e10f;

    getBoundingBoxForNode( mScene->mRootNode, &aiMin, &aiMax, &trafo );
	*min = fromAssimp( aiMin );
	*max = fromAssimp( aiMax );
}

void AssimpLoader::getBoundingBoxForNode( const aiNode *nd, aiVector3D *min, aiVector3D *max, aiMatrix4x4 *trafo )
{
    aiMatrix4x4 prev;

    prev = *trafo;
    //aiMultiplyMatrix4( trafo, &nd->mTransformation );
	*trafo = *trafo * nd->mTransformation;

    for ( unsigned n = 0; n < nd->mNumMeshes; ++n )
	{
        const struct aiMesh *mesh = mScene->mMeshes[ nd->mMeshes[ n ] ];
        for ( unsigned t = 0; t < mesh->mNumVertices; ++t )
		{
            aiVector3D tmp = mesh->mVertices[ t ];
            //aiTransformVecByMatrix4( &tmp, trafo );
			tmp *= (*trafo);

            min->x = math<float>::min( min->x, tmp.x );
            min->y = math<float>::min( min->y, tmp.y );
            min->z = math<float>::min( min->z, tmp.z );
            max->x = math<float>::max( max->x, tmp.x );
            max->y = math<float>::max( max->y, tmp.y );
            max->z = math<float>::max( max->z, tmp.z );
        }
    }

    for ( unsigned n = 0; n < nd->mNumChildren; ++n )
	{
        getBoundingBoxForNode( nd->mChildren[n], min, max, trafo );
    }

    *trafo = prev;
}

void AssimpLoader::loadGlResources()
{
	// create new mesh helpers for each mesh, will populate their data later.
	// modelMeshes.resize(scene->mNumMeshes,ofxAssimpMeshHelper());

	// create OpenGL buffers and populate them based on each meshes pertinant info.
	for ( unsigned i = 0; i < mScene->mNumMeshes; ++i )
	{
		app::console() << "loading mesh " << i << endl;

		// current mesh we are introspecting
		aiMesh *mesh = mScene->mMeshes[ i ];

		// the current meshHelper we will be populating data into.
		//ofxAssimpMeshHelper & meshHelper = modelMeshes[i];
		AssimpMeshHelper meshHelper;

		//meshHelper.texture = NULL;

#if 0
		// Handle material info
		aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D dcolor, scolor, acolor, ecolor;

		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &dcolor)){
			meshHelper.material.setDiffuseColor(aiColorToOfColor(dcolor));
		}

		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &scolor)){
			meshHelper.material.setSpecularColor(aiColorToOfColor(scolor));
		}

		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &acolor)){
			meshHelper.material.setAmbientColor(aiColorToOfColor(acolor));
		}

		if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &ecolor)){
			meshHelper.material.setEmissiveColor(aiColorToOfColor(ecolor));
		}

		float shininess;
		if(AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shininess)){
			meshHelper.material.setShininess(shininess);
		}

		int blendMode;
		if(AI_SUCCESS == aiGetMaterialInteger(mtl, AI_MATKEY_BLEND_FUNC, &blendMode)){
			if(blendMode==aiBlendMode_Default){
				meshHelper.blendMode=OF_BLENDMODE_ALPHA;
			}else{
				meshHelper.blendMode=OF_BLENDMODE_ADD;
			}
		}

		// Culling
		unsigned int max = 1;
		int two_sided;
		if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
			meshHelper.twoSided = true;
		else
			meshHelper.twoSided = false;

		// Load Textures
		int texIndex = 0;
		aiString texPath;

		// TODO: handle other aiTextureTypes
		if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath)){
			ofLog(OF_LOG_VERBOSE, "loading image from %s", texPath.data);
			string modelFolder = ofFilePath::getEnclosingDirectory(filepath,false);
			string relTexPath = ofFilePath::getEnclosingDirectory(texPath.data,false);
			string texFile = ofFilePath::getFileName(texPath.data);
			string realPath = modelFolder + relTexPath  + texFile;
			if(!ofFile::doesFileExist(realPath) || !ofLoadImage(meshHelper.texture,realPath)) {
				ofLog(OF_LOG_ERROR,string("error loading image ") + filepath + " " +realPath);
			}else{
				ofLog(OF_LOG_VERBOSE, "texture width: %f height %f", meshHelper.texture.getWidth(), meshHelper.texture.getHeight());
			}
		}
#endif

		meshHelper.mAiMesh = mesh;
		fromAssimp( mesh, &meshHelper.mCachedTriMesh, &meshHelper );
		meshHelper.mValidCache = true;
		meshHelper.mHasChanged = false;

		meshHelper.mAnimatedPos.resize( mesh->mNumVertices );
		if ( mesh->HasNormals() )
		{
			meshHelper.mAnimatedNorm.resize( mesh->mNumVertices );
		}

#if 0

		int usage;
		if(getAnimationCount()){
#ifndef TARGET_OPENGLES
			usage = GL_STREAM_DRAW;
#else
			usage = GL_DYNAMIC_DRAW;
#endif
		}else{
			usage = GL_STATIC_DRAW;

		}

		meshHelper.vbo.setVertexData(&mesh->mVertices[0].x,3,mesh->mNumVertices,usage,sizeof(aiVector3D));
		if(mesh->HasVertexColors(0)){
			meshHelper.vbo.setColorData(&mesh->mColors[0][0].r,mesh->mNumVertices,GL_STATIC_DRAW,sizeof(aiColor4D));
		}
		if(mesh->HasNormals()){
			meshHelper.vbo.setNormalData(&mesh->mNormals[0].x,mesh->mNumVertices,usage,sizeof(aiVector3D));
		}
		if (meshHelper.cachedMesh.hasTexCoords()){
			meshHelper.vbo.setTexCoordData(meshHelper.cachedMesh.getTexCoordsPointer()[0].getPtr(),mesh->mNumVertices,GL_STATIC_DRAW,sizeof(ofVec2f));
		}
#endif

		meshHelper.mIndices.resize( mesh->mNumFaces * 3 );
		unsigned j = 0;
		for ( unsigned x = 0; x < mesh->mNumFaces; ++x )
		{
			for ( unsigned a = 0; a < mesh->mFaces[x].mNumIndices; ++a)
			{
				meshHelper.mIndices[ j++ ] = mesh->mFaces[ x ].mIndices[ a ];
			}
		}

#if 0
		meshHelper.vbo.setIndexData(&meshHelper.indices[0],meshHelper.indices.size(),GL_STATIC_DRAW);
#endif

		mModelMeshes.push_back( meshHelper );
	}

#if 0
	animationTime = -1;
	setNormalizedTime(0);
#endif

	app::console() << "finished loading gl resources" << endl;
}

void AssimpLoader::draw()
{
	//ofPushStyle();

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
	//glPolygonMode(GL_FRONT_AND_BACK, ofGetGLPolyMode(renderType));
	gl::enable( GL_NORMALIZE );

	/*
	ofPushMatrix();

	ofTranslate(pos);

	ofRotate(180, 0, 0, 1);
	ofTranslate(-scene_center.x, -scene_center.y, scene_center.z);

	if(normalizeScale)
	{
		ofScale(normalizedScale , normalizedScale, normalizedScale);
	}

	for(int i = 0; i < (int)rotAngle.size(); i++){
		ofRotate(rotAngle[i], rotAxis[i].x, rotAxis[i].y, rotAxis[i].z);
	}

	ofScale(scale.x, scale.y, scale.z);


	if(getAnimationCount())
	{
		updateGLResources();
	}
	*/
	vector< AssimpMeshHelper >::const_iterator it = mModelMeshes.begin();
	gl::enableWireframe();
	for ( ; it != mModelMeshes.end(); ++it )
	{
		gl::draw( it->mCachedTriMesh );

		/*
		ofxAssimpMeshHelper & meshHelper = modelMeshes.at(i);

		// Texture Binding
		if(bUsingTextures && meshHelper.texture.isAllocated()){
			meshHelper.texture.bind();
		}

		if(bUsingMaterials){
			meshHelper.material.begin();
		}


		// Culling
		if(meshHelper.twoSided)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		ofEnableBlendMode(meshHelper.blendMode);
#ifndef TARGET_OPENGLES
		meshHelper.vbo.drawElements(GL_TRIANGLES,meshHelper.indices.size());
#else
		switch(renderType){
			case OF_MESH_FILL:
				meshHelper.vbo.drawElements(GL_TRIANGLES,meshHelper.indices.size());
				break;
			case OF_MESH_WIREFRAME:
				meshHelper.vbo.drawElements(GL_LINES,meshHelper.indices.size());
				break;
			case OF_MESH_POINTS:
				meshHelper.vbo.drawElements(GL_POINTS,meshHelper.indices.size());
				break;
		}
#endif

		// Texture Binding
		if(bUsingTextures && meshHelper.texture.bAllocated()){
			meshHelper.texture.unbind();
		}

		if(bUsingMaterials){
			meshHelper.material.end();
		}
		*/
	}
	gl::disableWireframe();

	//ofPopMatrix();

	glPopClientAttrib();
	glPopAttrib();
	//ofPopStyle();
}

} } // namespace mndl::assimp

