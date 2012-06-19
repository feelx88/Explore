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


#include "Player.h"
#include <engine/IrrlichtTools.h>
#include <engine/EntityTools.h>
#include <engine/EventReceiver.h>
#include <engine/PathTools.h>
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

#include "ItemFactory.h"
#include "items/SimpleGunItem.h"
#include "items/SimpleForceGunItem.h"

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
      mActiveItem( -1 ),
      mJumped( false )
{
    mProperties.reset( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                PathTools::getAbsoluteFileNameFromFolder( "Player", "xml" ),
                *mProperties );
    mEntity.reset( new Entity( mDevice, mBulletWorld, mProperties ) );
    mCamera = static_cast<ICameraSceneNodePtr>( mEntity->getSceneNode() );
    mEntity->getRigidBody()->setSleepingThresholds( 0.f, 0.f );
    mEntity->getRigidBody()->setAngularFactor( btVector3( 0.f, 0.f, 0.f ) );

    addItems();
    createGUI();
    setKeyMappings();
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

void Player::switchItem( int index )
{
    mInventory.at( mActiveItem )->setGUIVisible( false );
    mItemIcons[mActiveItem]->setDrawBorder( false );

    mActiveItem = index;

    if( mActiveItem >= mInventory.size() )
        mActiveItem = 0;
    if( mActiveItem < 0 )
        mActiveItem = mInventory.size() - 1;

    mInventory.at( mActiveItem )->setGUIVisible( true );
    mItemIcons[mActiveItem]->setDrawBorder( true );
}

void Player::addItems()
{
    mInventory.push_back( ItemPtr( ItemFactory::create( mExplore, this, "SimpleGun.item" ) ) );
    mInventory.push_back( ItemPtr( ItemFactory::create( mExplore, this, "SimpleForceGun.item" ) ) );

    mNumItems = mInventory.size();
    mActiveItem = mInventory.empty() ? -1 : 0;

    if( mActiveItem >= 0 )
    {
        IGUIElementPtr gui = mInventory.at( mActiveItem )->getGUIElement();
        if( gui )
            gui->setVisible( true );
    }
}

void Player::createGUI()
{
    IGUIWindow *win = mDevice->getGUIEnvironment()->addWindow(
                recti( 0, 0, 10 * 32 + 2, 32 + 2 ) );
    win->setDrawTitlebar( false );
    win->getCloseButton()->remove();
    win->move( vector2di( 10 * 32, 0 ) );

    for( int x = 0; x < mInventory.size(); ++x )
    {
        IGUIButton *img = mDevice->getGUIEnvironment()->addButton(
                    recti( x * 32 + 1, 1, ( x + 1 ) * 32 + 1, 33 ), win );
        img->setImage( mInventory.at( x )->getIcon() );
        img->setUseAlphaChannel( true );

        img->setDrawBorder( false );

        mItemIcons[x] = img;
    }

    mItemIcons[mActiveItem]->setDrawBorder( true );

    mItemWin = win;

    mCrossX = mDevice->getVideoDriver()->getScreenSize().Width / 2;
    mCrossY = mDevice->getVideoDriver()->getScreenSize().Height / 2;
    mCrossColor = mProperties->get( "Crosshair.Color", SColor( 255, 0, 255, 0 ) );
}

void Player::setKeyMappings()
{
    mKeyMapping[EPKM_FORWARD] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.Forward", "KEY_KEY_W" ) );
    mKeyMapping[EPKM_BACKWARD] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.Backward", "KEY_KEY_S" ) );
    mKeyMapping[EPKM_LEFT] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.Left", "KEY_KEY_A" ) );
    mKeyMapping[EPKM_RIGHT] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.Right", "KEY_KEY_D" ) );
    mKeyMapping[EPKM_JUMP] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.Jump", "KEY_SPACE" ) );
    mKeyMapping[EPKM_MOUSECONTROL] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.MouseControl", "KEY_TAB" ) );
    mKeyMapping[EPKM_NEXTSLOT] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.NextSlot", "KEY_KEY_E" ) );
    mKeyMapping[EPKM_PREVOISSLOT] =
            Explore::getKeyCode( mExplore->readConfigValue<std::string>(
                                     "Controls.PreviousSlot", "KEY_KEY_Q" ) );
}

void Player::processControls()
{
    if( mEventReceiver->isMouseLocked() )
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

        if( mEventReceiver->mouseClicked( 0 ) )
            mInventory.at( mActiveItem )->startAction( EIA_FIRST_ACTION );
        if( mEventReceiver->mouseClicked( 1 ) )
            mInventory.at( mActiveItem )->startAction( EIA_SECOND_ACTION );

        if( mEventReceiver->keyClicked( mKeyMapping[EPKM_NEXTSLOT] ) )
            switchItem( mActiveItem + 1 );
        if( mEventReceiver->keyClicked( mKeyMapping[EPKM_PREVOISSLOT] ) )
            switchItem( mActiveItem - 1 );
    }

    for( int x = 0; x < mNumItems; ++x )
    {
        if( mItemIcons[x]->isPressed() )
            switchItem( x );
    }

    vector3df vel( 0.f, 0.f, 0.f );

    if( mEventReceiver->keyPressed( mKeyMapping[EPKM_FORWARD] ) )
        vel.Z += 10;
    if( mEventReceiver->keyPressed( mKeyMapping[EPKM_BACKWARD] ) )
        vel.Z -= 10;
    if( mEventReceiver->keyPressed( mKeyMapping[EPKM_LEFT] ) )
        vel.X -= 10;
    if( mEventReceiver->keyPressed( mKeyMapping[EPKM_RIGHT] ) )
        vel.X += 10;

    vel = rotateToDirection( vel );
    vel.Y = mEntity->getRigidBody()->getLinearVelocity().getY();

    if( mEventReceiver->keyClicked( mKeyMapping[EPKM_MOUSECONTROL] ) )
        mEventReceiver->lockMouse( !mEventReceiver->isMouseLocked() );

    vector3df pos( *mEntity->getPosition() );

    line3df rayDown( pos, pos - vector3df( 0.f, 1.2f, 0.f ) );
    if( EntityTools::getFirstEntityInRay( mBulletWorld, rayDown ) )
    {
        if( mEventReceiver->keyPressed( mKeyMapping[EPKM_JUMP] ) )
        {
            vel.Y = 6.f;
            mJumped = true;
        }
        else
            mJumped = false;
    }
    else
    {
        if( !mJumped && vel.Y > 0.f )
            vel.Y = 0.f;
    }

    mEntity->getRigidBody()->setLinearVelocity( VectorConverter::bt( vel ) );
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
