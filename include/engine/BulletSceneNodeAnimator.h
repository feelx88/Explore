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


#ifndef BULLETSCENENODEANIMATOR_H
#define BULLETSCENENODEANIMATOR_H

#include <EngineTypedefs.h>

#include <irrlicht/irrlicht.h>
#include <bullet/btBulletDynamicsCommon.h>

class APIEXPORT BulletSceneNodeAnimator : public irr::scene::ISceneNodeAnimator
{
public:
    BulletSceneNodeAnimator( RigidBodyPtr body );
    virtual ~BulletSceneNodeAnimator();

    void animateNode(irr::scene::ISceneNode *node, irr::u32 );
    irr::scene::ISceneNodeAnimator *createClone(irr::scene::ISceneNode*,
                                                irr::scene::ISceneManager* );

    void applyToCamera( bool enabled );
    bool isAppliedToCamera() const;

protected:
    boost::weak_ptr<btRigidBody> mRigidBody;

    bool mApplyToCamera;
};

typedef boost::shared_ptr<BulletSceneNodeAnimator> BulletSceneNodeAnimatorPtr;

#endif // BULLETSCENENODEANIMATOR_H
