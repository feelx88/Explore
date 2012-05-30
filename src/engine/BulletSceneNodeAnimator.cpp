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
