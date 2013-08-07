/*
 Copyright (C) 2011-2013 Gabor Papp

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

 Based on the Node system of OGRE (Object-oriented Graphics Rendering Engine)
 <http://www.ogre3d.org/>
 Copyright (c) 2000-2013 Torus Knot Software Ltd
*/

#pragma once

#include <string>
#include <vector>

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Quaternion.h"
#include "cinder/Matrix.h"

namespace mndl {

class Node;

typedef std::shared_ptr< Node > NodeRef;

class Node
{
	public:
		Node();
		Node( const std::string &name );
		virtual ~Node() {};

		void setParent( std::weak_ptr< Node > parent );
		std::weak_ptr< Node > getParent() const;

		const std::vector< NodeRef > &getChildren() const { return mChildren; }
		std::vector< NodeRef > &getChildren() { return mChildren; }

		void addChild( NodeRef child );

		void setOrientation( const ci::Quatf &q );
		const ci::Quatf &getOrientation() const;

		void setPosition( const ci::Vec3f &pos );
		const ci::Vec3f& getPosition() const;

		void setScale( const ci::Vec3f &scale );
		const ci::Vec3f &getScale() const;

		void setInheritOrientation( bool inherit );
		bool getInheritOrientation() const;

		void setInheritScale( bool inherit );
		bool getInheritScale() const;

		void setName( const std::string &name );
		const std::string &getName() const;

		void setInitialState();
		void resetToInitialState();
		const ci::Vec3f &getInitialPosition() const;
		const ci::Quatf &getInitialOrientation() const;
		const ci::Vec3f &getInitialScale() const;

		const ci::Quatf &getDerivedOrientation() const;
		const ci::Vec3f &getDerivedPosition() const;
		const ci::Vec3f &getDerivedScale() const;

		const ci::Matrix44f &getDerivedTransform() const;

		//! Returns the local orientation of the given world-space orientation \a worldOrientation relative to this node
		ci::Quatf convertWorldToLocalOrientation( const ci::Quatf &worldOrientation ) const;
		//! Returns the world orientation of a point in the local space of the node
		ci::Quatf convertLocalToWorldOrientation( const ci::Quatf &localOrientation ) const;
		//! Returns the local position of the given world-space position \a worldPos relative to this node
		ci::Vec3f convertWorldToLocalPosition( const ci::Vec3f &worldPos ) const;
		//! Returns the world position of a point in the local space of the node
		ci::Vec3f convertLocalToWorldPosition( const ci::Vec3f &localPos ) const;

		void requestUpdate();

		void show( bool visible = true );
		void hide();
		bool isVisible() const;

	protected:
		/// Pointer to parent node.
		std::weak_ptr< Node > mParentWeak;

		/// Shared pointer vector holding the children.
		std::vector< NodeRef > mChildren;

		/// Name of this node.
		std::string mName;

		/// The orientation of the node relative to its parent.
		ci::Quatf mOrientation;

		/// The position of the node relative to its parent.
		ci::Vec3f mPosition;

		/// Scaling factor applied to this node.
		ci::Vec3f mScale;

		/// Stores whether this node inherits orientation from its parent.
		bool mInheritOrientation;

		mutable bool mNeedsUpdate;

		/// Stores whether this node inherits scale from its parent
		bool mInheritScale;

		/** Cached combined orientation.
		  This member is the orientation derived by combining the
		  local transformations and those of it's parents.
		  */
		mutable ci::Quatf mDerivedOrientation;

		/** Cached combined position.
		  This member is the position derived by combining the
		  local transformations and those of it's parents.
		  */
		mutable ci::Vec3f mDerivedPosition;

		/** Cached combined scale.
		  This member is the position derived by combining the
		  local transformations and those of it's parents.
		  */
		mutable ci::Vec3f mDerivedScale;

		/// The position to use as a base for keyframe animation.
		ci::Vec3f mInitialPosition;
		/// The orientation to use as a base for keyframe animation.
		ci::Quatf mInitialOrientation;
		/// The scale to use as a base for keyframe animation.
		ci::Vec3f mInitialScale;

		/// Cached derived transform as a 4x4 matrix
		mutable ci::Matrix44f mDerivedTransform;

		/// Stores whether the node is rendered.
		bool mVisible;

		void update() const;
};

} // namespace mndl

