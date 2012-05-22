#include "BulletSceneNodeAnimator.h"

using namespace irr;
using namespace core;
using namespace scene;

BulletSceneNodeAnimator::BulletSceneNodeAnimator( BulletWorldPtr world, RigidBodyPtr body )
    : mBulletWorld( world ),
      mRigidBody( body )
{
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
    btQuaternion btRot = trans.getRotation();

    vector3df irrPos = vector3df( btPos.getX(), btPos.getY(), btPos.getZ() );
    quaternion irrQuat( btRot.getX(), btRot.getY(), btRot.getZ(), btRot.getW() );
    vector3df irrRot;

    irrQuat.toEuler( irrRot );

    node->setPosition( irrPos );
    node->setRotation( irrRot );
}


ISceneNodeAnimator *BulletSceneNodeAnimator::createClone( ISceneNode*,
                                                          ISceneManager* )
{
    return new BulletSceneNodeAnimator(
                mBulletWorld, RigidBodyPtr( new btRigidBody( *mRigidBody.get() ) ) );
}
