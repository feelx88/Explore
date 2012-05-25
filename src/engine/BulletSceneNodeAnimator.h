#ifndef BULLETSCENENODEANIMATOR_H
#define BULLETSCENENODEANIMATOR_H

#include <irrlicht/irrlicht.h>
#include <bullet/btBulletDynamicsCommon.h>
#include "EngineTypedefs.h"

class BulletSceneNodeAnimator : public irr::scene::ISceneNodeAnimator
{
public:
    BulletSceneNodeAnimator( BulletWorldPtr world, RigidBodyPtr body );
    virtual ~BulletSceneNodeAnimator();

    void animateNode(irr::scene::ISceneNode *node, irr::u32 );
    irr::scene::ISceneNodeAnimator *createClone(irr::scene::ISceneNode*,
                                                irr::scene::ISceneManager* );

    void applyToCamera( bool enabled );
    bool isAppliedToCamera() const;

protected:
    BulletWorldPtr mBulletWorld;
    RigidBodyPtr mRigidBody;

    bool mApplyToCamera;
};

typedef boost::shared_ptr<BulletSceneNodeAnimator> BulletSceneNodeAnimatorPtr;

#endif // BULLETSCENENODEANIMATOR_H
