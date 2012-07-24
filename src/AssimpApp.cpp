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

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Light.h"
#include "cinder/TriMesh.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"

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

		void resize( ResizeEvent event );

		void update();
		void draw();

	private:
		CameraPersp mCamera;
		assimp::AssimpLoader mAssimpLoader;
};


void AssimpApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
}

void AssimpApp::setup()
{
	//mAssimpLoader = assimp::AssimpLoader( getAssetPath( "seymour.dae" ) );
	mAssimpLoader = assimp::AssimpLoader( getAssetPath( "player_249_1833.dae" ) );
}

void AssimpApp::resize( ResizeEvent event )
{
	mCamera.setPerspective( 60., getWindowAspectRatio(), 1, 1000 );
}

void AssimpApp::update()
{
}

void AssimpApp::draw()
{
	gl::clear( Color::black() );

	gl::setMatrices( mCamera );

	mCamera.lookAt( Vec3f( 0, 1, -2 ), Vec3f( 0, 1, 0 ) );
	gl::enableDepthWrite();
	gl::enableDepthRead();

	gl::rotate( Vec3f( -90, 0., getElapsedSeconds() * 20 ) );
	gl::color( Color::white() );

	mAssimpLoader.draw();
	//gl::drawStrokedCube( mAssimpLoader.getBoundingBox() );
}

CINDER_APP_BASIC( AssimpApp, RendererGl(0) )

