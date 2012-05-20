#ifndef BULLETSCENENODEANIMATOR_H
#define BULLETSCENENODEANIMATOR_H

#include <irrlicht/irrlicht.h>

class BulletSceneNodeAnimator : public irr::scene::ISceneNodeAnimator
{
public:
    BulletSceneNodeAnimator();

    void animateNode(irr::scene::ISceneNode *node, irr::u32 );
    irr::scene::ISceneNodeAnimator *createClone(irr::scene::ISceneNode*,
                                                irr::scene::ISceneManager* );
};

#endif // BULLETSCENENODEANIMATOR_H
