/*
    Copyright 2012 Felix Müller.

    This file is part of Explore.

    Explore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Explore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Explore.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "BulletSceneNodeAnimator.h"
#include "VectorConverter.h"
#include "QuaternionConverter.h"

using namespace irr;
using namespace core;
using namespace scene;

BulletSceneNodeAnimator::BulletSceneNodeAnimator( BulletWorldPtr world, RigidBodyPtr body )
    : mBulletWorld( world ),
      mRigidBody( body ),
      mApplyToCamera( false )
{
    if( mRigidBody && mBulletWorld )
        mBulletWorld->addRigidBody( mRigidBody.get() );
}

BulletSceneNodeAnimator::~BulletSceneNodeAnimator()
{
    if( mRigidBody && mBulletWorld )
        mBulletWorld->removeRigidBody( mRigidBody.get() );
}

void BulletSceneNodeAnimator::animateNode( ISceneNode *node, u32 )
{
    if( !node )
        return;

    btTransform trans = mRigidBody->getWorldTransform();

    vector3df pos = VectorConverter::irr( trans.getOrigin() );
    matrix4 mat = QuaternionConverter::irr( trans.getRotation() ).getMatrix();

    node->setPosition( pos );
    node->setRotation( mat.getRotationDegrees() );

    if( !mApplyToCamera || node->getType() != ESNT_CAMERA )
        return;

    vector3df up( 0.f, 1.f, 0.f );
    vector3df target( 0.f, 0.f, 1.f );
    mat.rotateVect( up );
    mat.rotateVect( target );

    static_cast<ICameraSceneNode*>( node )->setUpVector( up );
    static_cast<ICameraSceneNode*>( node )->setTarget( target + pos );
}


ISceneNodeAnimator *BulletSceneNodeAnimator::createClone( ISceneNode*,
                                                          ISceneManager* )
{
    return new BulletSceneNodeAnimator(
                mBulletWorld, RigidBodyPtr( new btRigidBody( *mRigidBody.get() ) ) );
}

void BulletSceneNodeAnimator::applyToCamera( bool enabled )
{
    mApplyToCamera = enabled;
}

bool BulletSceneNodeAnimator::isAppliedToCamera() const
{
    return mApplyToCamera;
}
