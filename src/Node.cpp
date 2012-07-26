#include "Node.h"

using namespace ci;
using namespace std;

namespace mndl {

Node::Node() :
	mScale( Vec3f::one() ),
	mInheritOrientation( true ),
	mInheritScale( true )
{
}

Node::Node( const std::string &name ) :
	mName( name ),
	mScale( Vec3f::one() ),
	mInheritOrientation( true ),
	mInheritScale( true )
{
}

void Node::setParent( NodeRef parent )
{
	mParent = parent;
}

NodeRef Node::getParent() const
{
	return mParent;
}

void Node::setOrientation( const ci::Quatf &q )
{
	mOrientation = q;
	mOrientation.normalize();
}

const Quatf &Node::getOrientation() const
{
	return mOrientation;
}

void Node::setPosition( const ci::Vec3f &pos )
{
	mPosition = pos;
}

const Vec3f& Node::getPosition() const
{
	return mPosition;
}

void Node::setScale( const Vec3f &scale )
{
	mScale = scale;
}

const Vec3f &Node::getScale() const
{
	return mScale;
}

void Node::setInheritOrientation( bool inherit )
{
	mInheritOrientation = inherit;
}

bool Node::getInheritOrientation() const
{
	return mInheritOrientation;
}

void Node::setInheritScale( bool inherit )
{
	mInheritScale = inherit;
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
    // TODO: cache transforms, only update when necessary
	update();
	return mDerivedOrientation;
}

const Vec3f &Node::getDerivedPosition() const
{
    // TODO: cache transforms, only update when necessary
	update();
	return mDerivedPosition;
}

const Vec3f &Node::getDerivedScale() const
{
    // TODO: cache transforms, only update when necessary
	update();
	return mDerivedScale;
}

const Matrix44f &Node::getDerivedTransform() const
{
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
}

} // namespace mndl
