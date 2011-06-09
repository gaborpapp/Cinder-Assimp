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
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AssimpApp : public AppBasic
{
	public:
		void prepareSettings(Settings *settings);
		void setup();

		void resize(ResizeEvent event);

		void update();
		void draw();

	private:
		TriMesh mesh;
		gl::Texture txt;

		CameraPersp camera;
};


void AssimpApp::prepareSettings(Settings *settings)
{
	settings->setWindowSize(640, 480);
}

void AssimpApp::setup()
{
	AssimpLoader loader((DataSourceRef)loadResource(RES_OBJ));
	loader.load(&mesh);

	txt = loadImage(loadResource(RES_TXT));
}

void AssimpApp::resize(ResizeEvent event)
{
	camera.setPerspective(60., getWindowAspectRatio(), 1, 1000);
}

void AssimpApp::update()
{
}

void AssimpApp::draw()
{
	gl::clear(Color(0, 0, 0));

	gl::setMatrices(camera);

	gl::enableDepthWrite();
	gl::enableDepthRead();

	gl::enable(GL_TEXTURE_2D);
	txt.bind();

	gl::rotate(Vec3f(0, getElapsedSeconds() * 20., 0));
	gl::scale(Vec3f(4., 4., 4.));
	gl::translate(Vec3f(0, -5, 0));
	gl::color(Color::white());
	gl::draw(mesh);

	txt.unbind();
	gl::disable(GL_TEXTURE_2D);
}

CINDER_APP_BASIC(AssimpApp, RendererGl)

