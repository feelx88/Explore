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


#include <items/SimpleGunItem.h>
#include <players/LocalPlayer.h>
#include <ItemFactory.h>
#include <engine/PathTools.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;
using namespace gui;

int SimpleGunItem::sRegisterDummy( ItemFactory::registerItem<SimpleGunItem>( "SimpleGun" ) );

SimpleGunItem::SimpleGunItem( ExplorePtr explore, IPlayerPtr owner,
                              PropTreePtr properties, const std::string &basePath )
    : Item( explore, owner, properties, basePath ),
      mCurBullet( 0 )
{
    //TODO:Load gun values from property tree
    std::string fileName =
            PathTools::getAbsolutePath( mProperties->get<std::string>( "Item.EntityFile" ), mBasePath );

    mBulletCount = mProperties->get( "Item.BulletCount", 10 );
    mAcceleration = mProperties->get( "Item.Acceleration", 50.f );

    for( int x = 0; x < mBulletCount; ++x )
    {
        EntityPtr bullet( new Entity( mDevice, mBulletWorld, fileName ) );
        mBullets.push_back( bullet.get() );
        mEntities->addEntity( bullet, x + 10, "" );
        mFullLabel += L"|";
    }

    dimension2du winSize =
            mDevice->getGUIEnvironment()->getSkin()->getFont()->getDimension( mFullLabel.c_str() );

    mGUI = mDevice->getGUIEnvironment()->addStaticText( mFullLabel.c_str(),
                                                        recti( 0, 0, winSize.Width, winSize.Height ),
                                                        false, false, 0, -1, true );
    mGUI->setVisible( false );

    registerEntities();
}

SimpleGunItem::~SimpleGunItem()
{
}

void SimpleGunItem::startActionInternal( uint8_t actionID )
{
    Item::startActionInternal( actionID );
    if( actionID == EIAID_FIRST_ACTION )
        shoot();
    else if( actionID == EIAID_SECOND_ACTION )
        reload();
}

void SimpleGunItem::shoot()
{
    VisualPlayerPtr owner;

    {
        IPlayerPtr o = getOwner();
        if( o->getType() != IPlayer::EPT_VISUAL
                && o->getType() != IPlayer::EPT_LOCAL )
        {
            return;
        }
        owner = boost::static_pointer_cast<VisualPlayer>( o );
    }

    if( mCurBullet >= mBulletCount )
        return;

    EntityPtr e = owner->getEntity();

    vector3df target = static_cast<LocalPlayer*>( getOwner().get() )->rotateToDirection();

    mBullets.at( mCurBullet )->setPosition(
                e->getSceneNode()->getAbsolutePosition() + target );
    mBullets.at( mCurBullet )->getRigidBody()->setLinearVelocity(
                btVector3( 0, 0, 0 ) );
    mBullets.at( mCurBullet )->getRigidBody()->applyCentralImpulse(
                VectorConverter::bt( target * mAcceleration ) );
    ++mCurBullet;

    stringw ammo;
    for( int x = 0; x < mBulletCount - mCurBullet; ++x )
        ammo += L"|";

    mGUI->setText( ammo.c_str() );
}

void SimpleGunItem::reload()
{
    mCurBullet = 0;
    mGUI->setText( mFullLabel.c_str() );

    for( int x = 0; x < mBulletCount; ++x )
    {
        mBullets.at( x )->setPosition( vector3df( 0.f, -10000.f, 0.f ) );
    }
}
