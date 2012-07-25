#pragma once

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

		void setParent( NodeRef parent );
		NodeRef getParent() const;

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

	protected:
		/// Shared pointer to parent node.
		NodeRef mParent;

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

		void update() const;
};

} // namespace mndl

