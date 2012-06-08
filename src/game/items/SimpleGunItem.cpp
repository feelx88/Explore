#include "SimpleGunItem.h"
#include "../Player.h"
#include <engine/PathTools.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;
using namespace gui;

SimpleGunItem::SimpleGunItem( ExplorePtr explore, PlayerPtr owner )
    : Item( explore, owner, "" ),
      mCurBullet( 0 )
{
    std::string fileName = PathTools::getAbsoluteFileNameFromFolder( "SimpleGun", "xml" );
    mBasePath = PathTools::getBasePathFromFile( fileName );

    mProperties.reset( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                fileName, *mProperties );

    for( int x = 0; x < 10; ++x )
    {
        mBullets[x].reset( new Entity( mDevice, mBulletWorld, fileName, "Bullet" ) );
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

IGUIElementPtr SimpleGunItem::getGUIElement() const
{
    return mGUI;
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
