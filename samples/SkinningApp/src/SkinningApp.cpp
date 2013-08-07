/*
 Copyright (C) 2011-2012 Gabor Papp

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

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/TriMesh.h"
#include "cinder/Camera.h"
#include "cinder/MayaCamUI.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Light.h"
#include "cinder/params/Params.h"
#include "cinder/Quaternion.h"

#include "AssimpLoader.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace mndl;

class SkinningApp : public AppBasic
{
	public:
		void prepareSettings( Settings *settings );
		void setup();

		void resize();
		void mouseDown( MouseEvent event );
		void mouseDrag( MouseEvent event );

		void update();
		void draw();

	private:
		assimp::AssimpLoader mAssimpLoader;

		MayaCamUI mMayaCam;

		params::InterfaceGl mParams;
		void setupParams();

		bool mEnableWireframe;
		float mFps;

		vector< string > mNodeNames;
		int mNodeIndex;
		bool mNoBones;
		vector< Quatf > mNodeOrientations;
};


void SkinningApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void SkinningApp::setup()
{
	mAssimpLoader = assimp::AssimpLoader( getAssetPath( "seymour.dae" ) );
	mAssimpLoader.enableSkinning();
	mNodeNames = mAssimpLoader.getNodeNames();
	if ( mNodeNames.empty () )
	{
		mNodeNames.push_back( "NO BONES!" );
		mNoBones = true;
	}
	else
	{
		mNoBones = false;
	}

	// query original node orientations from model
	mNodeOrientations.assign( mNodeNames.size(), Quatf() );
	if ( !mNoBones )
	{
		for ( size_t i = 0; i < mNodeOrientations.size(); ++i )
		{
			mNodeOrientations[ i ] = mAssimpLoader.getNodeOrientation( mNodeNames[ i ] );
		}
	}

	mNodeIndex = 0;
	mEnableWireframe = false;

	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 300 ) );

	setupParams();

	CameraPersp cam;
	cam.setPerspective( 60, getWindowAspectRatio(), 0.1f, 1000.0f );
	cam.setEyePoint( Vec3f( 0, 5, 15 ) );
	cam.setCenterOfInterestPoint( Vec3f( 0, 5, 0 ) );
	mMayaCam.setCurrentCam( cam );
}

void SkinningApp::setupParams()
{
	mParams.clear();

	mParams.addParam( "Wireframe", &mEnableWireframe );
	mParams.addSeparator();

	mParams.addParam( "Nodes", mNodeNames, &mNodeIndex, "", mNoBones );
	mParams.addSeparator();

	if ( mNoBones )
		return;

	mParams.addParam( "Rotation", &mNodeOrientations[ mNodeIndex ], "opened=true" );
	//mParams.addButton( "Reset current", bind( &SkinningApp::resetCurrentBone, this ) );
	//mParams.addButton( "Reset all", bind( &SkinningApp::resetAllBones, this ) );

	mParams.addSeparator();
	mParams.addParam( "Fps", &mFps, "", true );
}


void SkinningApp::update()
{
	static int lastNodeIndex = -1;

	if ( mNodeIndex != lastNodeIndex )
	{
		setupParams();
		lastNodeIndex = mNodeIndex;
	}

	if ( !mNoBones )
	{
		mAssimpLoader.setNodeOrientation( mNodeNames[ mNodeIndex ],
										  mNodeOrientations[ mNodeIndex ] );
	}

	mAssimpLoader.update();

	mFps = getAverageFps();
}

void SkinningApp::draw()
{
	gl::clear( Color::black() );

	gl::setMatrices( mMayaCam.getCamera() );

	gl::enableDepthWrite();
	gl::enableDepthRead();

	gl::color( Color::white() );

	if ( mEnableWireframe )
		gl::enableWireframe();
	gl::Light light( gl::Light::DIRECTIONAL, 0 );
	light.setAmbient( Color::white() );
	light.setDiffuse( Color::white() );
	light.setSpecular( Color::white() );
	light.lookAt( Vec3f( 0, 5, -20 ), Vec3f( 0, 5, 0 ) );
	light.update( mMayaCam.getCamera() );
	light.enable();

	gl::enable( GL_LIGHTING );
	gl::enable( GL_NORMALIZE );

	mAssimpLoader.draw();
	gl::disable( GL_LIGHTING );

	if ( mEnableWireframe )
		gl::disableWireframe();

	mParams.draw();
}

void SkinningApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void SkinningApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void SkinningApp::resize()
{
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}

CINDER_APP_BASIC( SkinningApp, RendererGl(0) )

