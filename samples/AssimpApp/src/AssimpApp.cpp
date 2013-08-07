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

#include "AssimpLoader.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace mndl;

class AssimpApp : public AppBasic
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
		bool mEnableTextures;
		bool mEnableWireframe;
		bool mEnableSkinning;
		bool mEnableAnimation;
		bool mDrawBBox;
		float mFps;
};


void AssimpApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void AssimpApp::setup()
{
	mAssimpLoader = assimp::AssimpLoader( getAssetPath( "astroboy_walk.dae" ) );
	mAssimpLoader.setAnimation( 0 );


	CameraPersp cam;
	cam.setPerspective( 60, getWindowAspectRatio(), 0.1f, 1000.0f );
	cam.setEyePoint( Vec3f( 0, 7, 20 ) );
	cam.setCenterOfInterestPoint( Vec3f( 0, 7, 0 ) );
	mMayaCam.setCurrentCam( cam );

	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 300 ) );
	mEnableWireframe = false;
	mParams.addParam( "Wireframe", &mEnableWireframe );
	mEnableTextures = true;
	mParams.addParam( "Textures", &mEnableTextures );
	mEnableSkinning = true;
	mParams.addParam( "Skinning", &mEnableSkinning );
	mEnableAnimation = false;
	mParams.addParam( "Animation", &mEnableAnimation );
	mDrawBBox = false;
	mParams.addParam( "Bounding box", &mDrawBBox );
	mParams.addSeparator();
	mParams.addParam( "Fps", &mFps, "", true );
}

void AssimpApp::update()
{
	mAssimpLoader.enableTextures( mEnableTextures );
	mAssimpLoader.enableSkinning( mEnableSkinning );
	mAssimpLoader.enableAnimation( mEnableAnimation );

	double time = fmod( getElapsedSeconds(), mAssimpLoader.getAnimationDuration( 0 ) );
	mAssimpLoader.setTime( time );
	mAssimpLoader.update();

	mFps = getAverageFps();
}

void AssimpApp::draw()
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

	if ( mDrawBBox )
		gl::drawStrokedCube( mAssimpLoader.getBoundingBox() );

	mParams.draw();
}

void AssimpApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void AssimpApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void AssimpApp::resize()
{
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}

CINDER_APP_BASIC( AssimpApp, RendererGl(0) )

