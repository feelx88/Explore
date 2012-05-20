#include "BulletSceneNodeAnimator.h"

using namespace irr;
using namespace core;
using namespace scene;

BulletSceneNodeAnimator::BulletSceneNodeAnimator()
{
}

void BulletSceneNodeAnimator::animateNode( ISceneNode *node, u32 )
{
}


ISceneNodeAnimator *BulletSceneNodeAnimator::createClone( ISceneNode*,
                                                          ISceneManager* )
{
    return new BulletSceneNodeAnimator();
}
