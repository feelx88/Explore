/*
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


#include "Player.h"
#include <engine/IrrlichtTools.h>
#include <engine/EntityTools.h>
#include <engine/EventReceiver.h>
#include <engine/PathTools.h>
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

#include "items/SimpleGunItem.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

Player::Player( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() ),
      mActiveItem( -1 )
{
    mProperties.reset( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                PathTools::getAbsoluteFileNameFromFolder( "Player", "xml" ), *mProperties );
    mEntity.reset( new Entity( mDevice, mBulletWorld, mProperties ) );
    mCamera = static_cast<ICameraSceneNodePtr>( mEntity->getSceneNode() );
    mEntity->getRigidBody()->setSleepingThresholds( 0.f, 0.f );
    mEntity->getRigidBody()->setAngularFactor( btVector3( 0.f, 0.f, 0.f ) );

    mCrossX = mDevice->getVideoDriver()->getScreenSize().Width / 2;
    mCrossY = mDevice->getVideoDriver()->getScreenSize().Height / 2;
    mCrossColor = mProperties->get( "Crosshair.Color", SColor( 255, 0, 255, 0 ) );

    mInventory.push_back( ItemPtr( new SimpleGunItem( mExplore, this ) ) );

    mActiveItem = mInventory.empty() ? -1 : 0;

    if( mActiveItem >= 0 )
    {
        IGUIElementPtr gui = mInventory.at( mActiveItem )->getGUIElement();
        if( gui )
            gui->setVisible( true );
    }

    IGUIWindow *win = mDevice->getGUIEnvironment()->addWindow( recti( 0, 0, 10 * 32, 32 ) );
    win->setDrawTitlebar( false );
    win->getCloseButton()->remove();
    win->move( vector2di( 10 * 32, 0 ) );

    for( int x = 0; x < mInventory.size(); ++x )
    {
        IGUIImage *img = mDevice->getGUIEnvironment()->addImage(
                    recti( x * 32, 0, ( x + 1 ) * 32, 32 ), win );
        img->setImage( mInventory.at( x )->getIcon() );
        img->setUseAlphaChannel( true );
    }

    mItemWin = win;
}

Player::~Player()
{
    mItemWin->remove();
}

EntityPtr Player::getEntity() const
{
    return mEntity;
}

ItemPtr Player::getActiveItem() const
{
    if( mActiveItem < 0 )
        return ItemPtr();

    return mInventory.at( mActiveItem );
}

void Player::update()
{
    processControls();
    drawCrosshair();
}

vector3df Player::rotateToDirection( vector3df dir ) const
{
    matrix4 m = mEntity->getSceneNode()->getAbsoluteTransformation();
    m.rotateVect( dir );

    return dir;
}

void Player::processControls()
{
    vector3df rot( mEntity->getSceneNode()->getRotation() );
    rot.Z = 0.f;
    rot.X += float( mEventReceiver->mouseMoveY() ) / 10.f;
    rot.Y += float( mEventReceiver->mouseMoveX() ) / 10.f;

    if( rot.X < -89.f )
        rot.X = -89.f;
    if( rot.X > +89.f )
        rot.X = +89.f;

    mEntity->getSceneNode()->setRotation( rot );

    vector3df target = rotateToDirection();

    mCamera->setTarget( *( mEntity->getPosition() ) + target * 10000.f );

    vector3df vel( 0.f, 0.f, 0.f );

    if( mEventReceiver->keyPressed( KEY_KEY_W ) )
        vel.Z += 10;
    if( mEventReceiver->keyPressed( KEY_KEY_S ) )
        vel.Z -= 10;
    if( mEventReceiver->keyPressed( KEY_KEY_A ) )
        vel.X -= 10;
    if( mEventReceiver->keyPressed( KEY_KEY_D ) )
        vel.X += 10;

    vel = rotateToDirection( vel );
    vel.Y = mEntity->getRigidBody()->getLinearVelocity().getY();

    if( mEventReceiver->keyPressed( KEY_SPACE ) && iszero( vel.Y ) )
        vel.Y = 5.f;

    mEntity->getRigidBody()->setLinearVelocity( VectorConverter::bt( vel ) );

    if( mActiveItem != -1 )
    {
        if( mEventReceiver->mouseClicked( 0 ) )
            mInventory.at( mActiveItem )->startAction( EIA_FIRST_ACTION );
        if( mEventReceiver->mouseClicked( 1 ) )
            mInventory.at( mActiveItem )->startAction( EIA_SECOND_ACTION );
    }

    if( mEventReceiver->keyClicked( KEY_KEY_Q ) )
    {
        line3df ray;
        ray.start = *( mEntity->getPosition() ) + vector3df( 0, 1, 0 );
        ray.end = ray.start + rotateToDirection( vector3df( 0.f, 0.f, 1000.f ) );
        vector3df out;
        boost::optional<Entity*> e =
                EntityTools::getFirstEntityInRay( mBulletWorld, ray, out );

        if( e )
        {
            ( *e )->getRigidBody()->activate();
            vector3df pos = *( *e )->getPosition();
            ( *e )->getRigidBody()->applyImpulse( VectorConverter::bt( ray.end ),
                                                  VectorConverter::bt( out - pos ) );
        }
    }
}

void Player::drawCrosshair()
{
    mDevice->getVideoDriver()->draw2DLine(
                vector2di( mCrossX - 10, mCrossY ),
                vector2di( mCrossX + 10, mCrossY ),
                mCrossColor );
    mDevice->getVideoDriver()->draw2DLine(
                vector2di( mCrossX, mCrossY - 10 ),
                vector2di( mCrossX, mCrossY + 10 ),
                mCrossColor );
}
