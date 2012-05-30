#include "Player.h"
#include <engine/LoggerSingleton.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

Player::Player( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() )
{
    PropTreePtr prop( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                "data/Entities/Player/Player.xml", *prop );
    mEntity.reset( new Entity( mDevice, mBulletWorld, prop ) );
}

Player::~Player()
{
}

EntityPtr Player::getEntity() const
{
    return mEntity;
}

void Player::update()
{
    if( mEventReceiver->keyPressed( KEY_KEY_W ) )
        mEntity->getRigidBody()->applyCentralImpulse( btVector3( 0, 0, 10 ) );
    if( mEventReceiver->keyPressed( KEY_KEY_S ) )
        mEntity->getRigidBody()->applyCentralImpulse( btVector3( 0, 0, -10 ) );
    if( mEventReceiver->keyPressed( KEY_KEY_A ) )
        mEntity->getRigidBody()->applyCentralImpulse( btVector3( -10, 0, 0 ) );
    if( mEventReceiver->keyPressed( KEY_KEY_D ) )
        mEntity->getRigidBody()->applyCentralImpulse( btVector3( 10, 0, 0 ) );
}
