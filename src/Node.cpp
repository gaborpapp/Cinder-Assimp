/*
 Copyright (C) 2011-2012 Gabor Papp

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Node.h"

using namespace ci;
using namespace std;

namespace mndl {

Node::Node() :
	mScale( Vec3f::one() ),
	mInheritOrientation( true ),
	mInheritScale( true ),
	mNeedsUpdate( true )
{
}

Node::Node( const std::string &name ) :
	mName( name ),
	mScale( Vec3f::one() ),
	mInheritOrientation( true ),
	mInheritScale( true ),
	mNeedsUpdate( true )
{
}

void Node::setParent( NodeRef parent )
{
	mParent = parent;
	requestUpdate();
}

NodeRef Node::getParent() const
{
	return mParent;
}

void Node::addChild( NodeRef child )
{
	mChildren.push_back( child );
}

void Node::setOrientation( const ci::Quatf &q )
{
	mOrientation = q;
	mOrientation.normalize();
	requestUpdate();
}

const Quatf &Node::getOrientation() const
{
	return mOrientation;
}

void Node::setPosition( const ci::Vec3f &pos )
{
	mPosition = pos;
	requestUpdate();
}

const Vec3f& Node::getPosition() const
{
	return mPosition;
}

void Node::setScale( const Vec3f &scale )
{
	mScale = scale;
	requestUpdate();
}

const Vec3f &Node::getScale() const
{
	return mScale;
}

void Node::setInheritOrientation( bool inherit )
{
	mInheritOrientation = inherit;
	requestUpdate();
}

bool Node::getInheritOrientation() const
{
	return mInheritOrientation;
}

void Node::setInheritScale( bool inherit )
{
	mInheritScale = inherit;
	requestUpdate();
}

bool Node::getInheritScale() const
{
	return mInheritScale;
}

void Node::setName( const string &name )
{
	mName = name;
}

const string &Node::getName() const
{
	return mName;
}

void Node::setInitialState()
{
	mInitialPosition = mPosition;
	mInitialOrientation = mOrientation;
	mInitialScale = mScale;
}

void Node::resetToInitialState()
{
	mPosition = mInitialPosition;
	mOrientation = mInitialOrientation;
	mScale = mInitialScale;
	requestUpdate();
}

const Vec3f &Node::getInitialPosition() const
{
	return mInitialPosition;
}

const Quatf &Node::getInitialOrientation() const
{
	return mInitialOrientation;
}

const Vec3f &Node::getInitialScale() const
{
	return mInitialScale;
}

const Quatf &Node::getDerivedOrientation() const
{
	if ( mNeedsUpdate )
		update();
	return mDerivedOrientation;
}

const Vec3f &Node::getDerivedPosition() const
{
	if ( mNeedsUpdate )
		update();
	return mDerivedPosition;
}

const Vec3f &Node::getDerivedScale() const
{
	if ( mNeedsUpdate )
		update();
	return mDerivedScale;
}

const Matrix44f &Node::getDerivedTransform() const
{
	if ( mNeedsUpdate )
		update();

    mDerivedTransform = Matrix44f::createScale( mDerivedScale );
    mDerivedTransform *= mDerivedOrientation.toMatrix44();
    mDerivedTransform.setTranslate( mDerivedPosition );

    return mDerivedTransform;
}

void Node::update() const
{
    if ( mParent )
    {
        // update orientation
        const Quatf &parentOrientation = mParent->getDerivedOrientation();
        if ( mInheritOrientation )
        {
            // Combine orientation with that of parent
            mDerivedOrientation = getOrientation() * parentOrientation;
        }
        else
        {
            mDerivedOrientation = getOrientation();
        }

        // update scale
        const Vec3f &parentScale = mParent->getDerivedScale();
        if ( mInheritScale )
        {
            mDerivedScale = parentScale * getScale();
        }
        else
        {
            mDerivedScale = getScale();
        }

		// change position vector based on parent's orientation & scale
        mDerivedPosition = ( parentScale * getPosition() ) * parentOrientation;

        // add altered position vector to parent's
        mDerivedPosition += mParent->getDerivedPosition();
    }
    else
    {
        // root node, no parent
        mDerivedOrientation = getOrientation();
        mDerivedPosition = getPosition();
        mDerivedScale = getScale();
    }

	mNeedsUpdate = false;
}

void Node::requestUpdate()
{
	mNeedsUpdate = true;

	for ( vector< NodeRef >::iterator it = mChildren.begin();
			it != mChildren.end(); ++it )
	{
		(*it)->requestUpdate();
	}
}

} // namespace mndl
