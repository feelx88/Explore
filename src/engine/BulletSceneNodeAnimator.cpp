#include "BulletSceneNodeAnimator.h"

using namespace irr;
using namespace core;
using namespace scene;

BulletSceneNodeAnimator::BulletSceneNodeAnimator( BulletWorldPtr world, RigidBodyPtr body )
    : mBulletWorld( world ),
      mRigidBody( body )
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
    btTransform trans;
    mRigidBody->getMotionState()->getWorldTransform( trans );

    btVector3 btPos = trans.getOrigin();
    btQuaternion btQuat = trans.getRotation();

    vector3df irrPos = vector3df( btPos.getX(), btPos.getY(), btPos.getZ() );
    quaternion irrQuat( -btQuat.getX(), -btQuat.getY(), -btQuat.getZ(), btQuat.getW() );

    node->setPosition( irrPos );
    node->setRotation( irrQuat.getMatrix().getRotationDegrees() );
}


ISceneNodeAnimator *BulletSceneNodeAnimator::createClone( ISceneNode*,
                                                          ISceneManager* )
{
    return new BulletSceneNodeAnimator(
                mBulletWorld, RigidBodyPtr( new btRigidBody( *mRigidBody.get() ) ) );
}
