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

protected:
    BulletWorldPtr mBulletWorld;
    RigidBodyPtr mRigidBody;
};

#endif // BULLETSCENENODEANIMATOR_H
