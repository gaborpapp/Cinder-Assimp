#pragma once

#include "cinder/Vector.h"
#include "cinder/Quaternion.h"
#include "cinder/Matrix44.h"
#include "cinder/CinderMath.h"

namespace mndl { namespace math {

// matrix decomposition converted from Ogre

template< typename T >
void QDUDecomposition( ci::Matrix33<T> m, ci::Matrix33<T> *kQ,
	ci::Vec3<T> *kD, ci::Vec3<T> *kU)
{
	// Factor M = QR = QDU where Q is orthogonal, D is diagonal,
	// and U is upper triangular with ones on its diagonal.  Algorithm uses
	// Gram-Schmidt orthogonalization (the QR algorithm).
	//
	// If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
	//
	//   q0 = m0/|m0|
	//   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
	//   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
	//
	// where |V| indicates length of vector V and A*B indicates dot
	// product of vectors A and B.  The matrix R has entries
	//
	//   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
	//   r10 = 0      r11 = q1*m1  r12 = q1*m2
	//   r20 = 0      r21 = 0      r22 = q2*m2
	//
	// so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
	// u02 = r02/r00, and u12 = r12/r11.

	// Q = rotation
	// D = scaling
	// U = shear

	// D stores the three diagonal entries r00, r11, r22
	// U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

	// build orthogonal matrix Q
	T fInvLength = m.m00*m.m00 + m.m10*m.m10 + m.m20*m.m20;
	if ( fInvLength != T( 0 ) )
		fInvLength = T( 1 ) / ci::math< T >::sqrt( fInvLength );

	kQ->m00 = m.m00*fInvLength;
	kQ->m10 = m.m10*fInvLength;
	kQ->m20 = m.m20*fInvLength;

	T fDot = kQ->m00*m.m01 + kQ->m10*m.m11 +
		kQ->m20*m.m21;
	kQ->m01 = m.m01-fDot*kQ->m00;
	kQ->m11 = m.m11-fDot*kQ->m10;
	kQ->m21 = m.m21-fDot*kQ->m20;
	fInvLength = kQ->m01*kQ->m01 + kQ->m11*kQ->m11 + kQ->m21*kQ->m21;
	if ( fInvLength != T( 0 ) )
		fInvLength = T( 1 ) / ci::math< T >::sqrt( fInvLength );

	kQ->m01 *= fInvLength;
	kQ->m11 *= fInvLength;
	kQ->m21 *= fInvLength;

	fDot = kQ->m00*m.m02 + kQ->m10*m.m12 +
		kQ->m20*m.m22;
	kQ->m02 = m.m02-fDot*kQ->m00;
	kQ->m12 = m.m12-fDot*kQ->m10;
	kQ->m22 = m.m22-fDot*kQ->m20;
	fDot = kQ->m01*m.m02 + kQ->m11*m.m12 +
		kQ->m21*m.m22;
	kQ->m02 -= fDot*kQ->m01;
	kQ->m12 -= fDot*kQ->m11;
	kQ->m22 -= fDot*kQ->m21;
	fInvLength = kQ->m02*kQ->m02 + kQ->m12*kQ->m12 + kQ->m22*kQ->m22;
	if ( fInvLength != T( 0 ) )
		fInvLength = T( 1 ) / ci::math< T >::sqrt( fInvLength );

	kQ->m02 *= fInvLength;
	kQ->m12 *= fInvLength;
	kQ->m22 *= fInvLength;

	// guarantee that orthogonal matrix has determinant 1 (no reflections)
	T fDet = kQ->determinant();

	if ( fDet < 0.0 )
	{
		*kQ = *kQ * (T)-1;
	}

	// build "right" matrix R
	ci::Matrix33<T> kR;
	kR.m00 = kQ->m00*m.m00 + kQ->m10*m.m10 +
		kQ->m20*m.m20;
	kR.m01 = kQ->m00*m.m01 + kQ->m10*m.m11 +
		kQ->m20*m.m21;
	kR.m11 = kQ->m01*m.m01 + kQ->m11*m.m11 +
		kQ->m21*m.m21;
	kR.m02 = kQ->m00*m.m02 + kQ->m10*m.m12 +
		kQ->m20*m.m22;
	kR.m12 = kQ->m01*m.m02 + kQ->m11*m.m12 +
		kQ->m21*m.m22;
	kR.m22 = kQ->m02*m.m02 + kQ->m12*m.m12 +
		kQ->m22*m.m22;

	// the scaling component
	kD->x = kR.m00;
	kD->y = kR.m11;
	kD->z = kR.m22;

	// the shear component
	T fInvD0 = T(1) / kD->x;
	kU->x = kR.m01*fInvD0;
	kU->y = kR.m02*fInvD0;
	kU->z = kR.m12/kD->y;
}

/** Decompose a Matrix44 to orientation / scale / position.
 **/
template< typename T >
void decompose( ci::Matrix44<T> m, ci::Vec3<T> *position, ci::Vec3<T> *scale, ci::Quaternion<T> *orientation)
{
	//assert( isAffine( m ) );

	ci::Matrix33<T> m3x3 = m.subMatrix33( 0, 0 );

	ci::Matrix33<T> matQ;
	ci::Vec3<T> vecU;
	QDUDecomposition( m3x3, &matQ, scale, &vecU );

	*orientation = ci::Quaternion<T>( matQ );
	*position = ci::Vec3<T>( m.m03, m.m13, m.m23 );
}

/** Check whether or not the matrix is affine matrix.
 *  @remarks
 *  An affine matrix is a 4x4 matrix with row 3 equal to (0, 0, 0, 1),
 *  e.g. no projective coefficients.
 **/
template< typename T >
inline bool isAffine( const ci::Matrix44<T> &m )
{
	return m.m30 == 0 && m.m31 == 0 && m.m32 == 0 && m.m33 == 1;
}

} } // namespace mndl::math
