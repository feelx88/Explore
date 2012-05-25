#include "BulletSceneNodeAnimator.h"

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

    btTransform trans;
    trans.setIdentity();
    mRigidBody->getMotionState()->getWorldTransform( trans );

    btVector3 btPos = trans.getOrigin();
    btQuaternion btQuat = trans.getRotation();

    vector3df irrPos = vector3df( btPos.getX(), btPos.getY(), btPos.getZ() );
    matrix4 irrMat = quaternion(
                -btQuat.getX(), -btQuat.getY(),
                -btQuat.getZ(), btQuat.getW() ).getMatrix();

    node->setPosition( irrPos );
    node->setRotation( irrMat.getRotationDegrees() );

    if( !mApplyToCamera || node->getType() != ESNT_CAMERA )
        return;

    vector3df up( 0.f, 1.f, 0.f );
    vector3df target( 0.f, 0.f, 1.f );
    irrMat.rotateVect( up );
    irrMat.rotateVect( target );

    static_cast<ICameraSceneNode*>( node )->setUpVector( up );
    static_cast<ICameraSceneNode*>( node )->setTarget( target + irrPos );
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
