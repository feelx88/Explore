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


#include "SimpleGunItem.h"
#include "../Player.h"
#include "../ItemFactory.h"
#include <engine/PathTools.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;
using namespace gui;

int SimpleGunItem::sRegisterDummy( ItemFactory::registerItem<SimpleGunItem>( "SimpleGun" ) );

SimpleGunItem::SimpleGunItem( ExplorePtr explore, PlayerPtr owner,
                              PropTreePtr properties, const std::string &basePath )
    : Item( explore, owner, properties, basePath ),
      mCurBullet( 0 )
{
    //TODO:Load gun values from property tree
    std::string fileName =
            PathTools::getAbsolutePath( mProperties->get<std::string>( "Item.EntityFile" ), mBasePath );

    for( int x = 0; x < 10; ++x )
    {
        mBullets[x].reset( new Entity( mDevice, mBulletWorld, fileName ) );
    }

    dimension2du winSize =
            mDevice->getGUIEnvironment()->getSkin()->getFont()->getDimension( L"||||||||||" );

    mGUI = mDevice->getGUIEnvironment()->addStaticText( L"||||||||||",
                                                        recti( 0, 0, winSize.Width, winSize.Height ),
                                                        false, false, 0, -1, true );
    mGUI->setVisible( false );

    loadIcon();
}

SimpleGunItem::~SimpleGunItem()
{
    mGUI->remove();
}

void SimpleGunItem::startAction( int actionID )
{
    if( actionID == 0 )
        shoot();
    else if( actionID == 1 )
        reload();
}

void SimpleGunItem::shoot()
{
    if( mCurBullet >= 10 )
        return;

    EntityPtr e = mOwner->getEntity();

    vector3df target = mOwner->rotateToDirection();

    float accel = 50.f;

    mBullets[mCurBullet]->setPosition( e->getSceneNode()->getAbsolutePosition() + target );
    mBullets[mCurBullet]->getRigidBody()->setLinearVelocity( btVector3( 0, 0, 0 ) );
    mBullets[mCurBullet]->getRigidBody()->applyCentralImpulse( VectorConverter::bt( target * accel ) );
    ++mCurBullet;

    stringw ammo;
    for( int x = 0; x < 10 - mCurBullet; ++x )
        ammo += L"|";

    mGUI->setText( ammo.c_str() );
}

void SimpleGunItem::reload()
{
    mCurBullet = 0;
    mGUI->setText( L"||||||||||" );

    for( int x = 0; x < 10; ++x )
    {
        mBullets[x]->setPosition( vector3df( 0.f, -10000.f, 0.f ) );
    }
}
