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


#include "Item.h"
#include "ItemFactory.h"
#include "IPlayer.h"
#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;

EntityItemMap Item::sEntityItemMap;
int Item::sRegisterDummy( ItemFactory::registerItem<Item>( "Item" ) );

Item::Item( ExplorePtr explore, IPlayerPtr owner, PropTreePtr properties,
            const std::string &basePath )
    : NetworkSyncable(),
      mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mBulletWorld( explore->getBulletWorld() ),
      mProperties( PropTreePtr( new boost::property_tree::ptree( *properties ) ) ),
      mBasePath( basePath ),
      mOwner( owner ),
      mGUI( 0 )
{
    setTypeID( ENTI_ITEM );
    loadIcon();

    mCacheID = mProperties->get<std::string>( "Item.CacheID" );

    //TODO: Should work because Entities copy their PropTreePtr and mProperties
    //lives until the destructor gets called, but maybe search for a more
    //elegant way
    PropTreePtr child( &mProperties->get_child( "Item" ),
                       specialDeleters::NullDeleter() );
    mEntities.reset( new EntityContainer( mDevice, mBulletWorld,
                                          child,
                                          mBasePath ) );

    registerScripts();

    if( mProperties->get( "Item.AutoAddEntities", false ) )
        registerEntities();

    setActivationState( mProperties->get( "Item.AutoActivation", false ) );

    mHitPoints = mProperties->get( "Item.HitPoints", 1.f );
    mDestructible = mProperties->get( "Item.Destructible", true );
}

Item::~Item()
{
    if( mEntities )
    {
        for( EntityMap::const_iterator x = mEntities->getEntities().begin();
             x != mEntities->getEntities().end(); ++x )
        {
            sEntityItemMap.erase( x->second.get() );
        }
    }

    if( mGUI )
        mGUI->remove();
}

PropTreePtr Item::getProperties() const
{
    return mProperties;
}

EntityContainerPtr Item::getEntities() const
{
    return mEntities;
}

void Item::setOwner( IPlayerPtr owner )
{
    mOwner = owner;
}

IPlayerPtr Item::getOwner() const
{
    return mOwner.lock();
}

void Item::registerEntities()
{
    for( EntityMap::const_iterator x = mEntities->getEntities().begin();
         x != mEntities->getEntities().end(); ++x )
    {
        sEntityItemMap.insert( std::make_pair( x->second.get(), this ) );
    }
}

void Item::loadIcon()
{
    std::string iconFileName = PathTools::getAbsolutePath( "defaultIcon.png" );
    iconFileName = mProperties->get( "Item.Icon", iconFileName );
    iconFileName = PathTools::getAbsolutePath( iconFileName, mBasePath );

    mIcon = mDevice->getVideoDriver()->getTexture( iconFileName.c_str() );
}

void Item::startActionInternal( uint8_t actionID )
{
    ScriptMap::iterator x = mScripts.find( actionID );
    if( x == mScripts.end() )
        return;
    else
        mScripts.at( actionID )->exec();
}

void Item::serializeInternal( NetworkSyncablePacket &packet, uint8_t actionID )
{
    if( actionID == EAID_CREATE )
    {
        packet.writeString( mCacheID );
        packet.writeUInt32( mOwner.lock()->getUID() );

        serializeEntities( packet );

        return;
    }
    else if( actionID == EAID_UPDATE )
    {
        serializeEntities( packet );
    }
}

boost::optional<NetworkSyncablePacket> Item::deserializeInternal(
        NetworkSyncablePacket &packet )
{
    if( packet.getActionID() == EAID_CREATE || packet.getActionID() == EAID_UPDATE )
    {
        //ENGA_CREATE: string[name] and uint32_t[ownerID] already read
        deserializeEntities( packet );
    }
    else
        startActionInternal( packet.getActionID() );
    return boost::none;
}

void Item::serializeEntities( NetworkSyncablePacket &packet )
{
    const EntityMap &entities = mEntities->getEntities();

    packet.writeUInt16( entities.size() );

    foreach_( const EntityMap::value_type &x, entities )
    {
        RigidBodyPtr body = x.second->getRigidBody();
        btTransform trans = body->getWorldTransform();

        packet.writeFloat( trans.getOrigin().getX() );
        packet.writeFloat( trans.getOrigin().getY() );
        packet.writeFloat( trans.getOrigin().getZ() );

        packet.writeFloat( trans.getRotation().getX() );
        packet.writeFloat( trans.getRotation().getY() );
        packet.writeFloat( trans.getRotation().getZ() );
        packet.writeFloat( trans.getRotation().getW() );

        btVector3 linvel = body->getLinearVelocity(),
                angvel = body->getAngularVelocity();
        packet.writeFloat( linvel.getX() );
        packet.writeFloat( linvel.getY() );
        packet.writeFloat( linvel.getZ() );
        packet.writeFloat( angvel.getX() );
        packet.writeFloat( angvel.getY() );
        packet.writeFloat( angvel.getZ() );
    }
}

void Item::deserializeEntities( NetworkSyncablePacket &packet )
{
    const EntityMap &entities = mEntities->getEntities();

    /*uint16_t size = */packet.readUInt16();//TODO: range check?

    foreach_( const EntityMap::value_type &x, entities )
    {
        float originX = packet.readFloat();
        float originY = packet.readFloat();
        float originZ = packet.readFloat();

        float rotationX = packet.readFloat();
        float rotationY = packet.readFloat();
        float rotationZ = packet.readFloat();
        float rotationW = packet.readFloat();

        float linvelX = packet.readFloat();
        float linvelY = packet.readFloat();
        float linvelZ = packet.readFloat();

        float angvelX = packet.readFloat();
        float angvelY = packet.readFloat();
        float angvelZ = packet.readFloat();

        RigidBodyPtr body = x.second->getRigidBody();
        body->setWorldTransform( btTransform( btQuaternion( rotationX,
                                                            rotationY,
                                                            rotationZ,
                                                            rotationW ),
                                              btVector3( originX,
                                                         originY,
                                                         originZ ) ) );
        body->setLinearVelocity( btVector3( linvelX, linvelY, linvelZ ) );
        body->setAngularVelocity( btVector3( angvelX, angvelY, angvelZ ) );
    }
}

void Item::startAction( uint8_t actionID )
{
    startActionInternal( actionID );

    if( actionID == EAID_CREATE || actionID == EAID_DESTROY )
    {
        mExplore->getExploreServer()->checkedSend( serialize( actionID ) );
    }
    else if( actionID != EAID_UPDATE )
        mExplore->getExploreServer()->send( serialize( actionID ) );
}

void Item::setGUIVisible( bool visible )
{
    if( !mGUI )
        return;

    mGUI->setVisible( visible );
}

IGUIElementPtr Item::getGUIElement() const
{
    return mGUI;
}

ITexturePtr Item::getIcon() const
{
    return mIcon;
}

float Item::getHitPoints() const
{
    return mHitPoints;
}

void Item::setHitPoints( float hitPoints )
{
    mHitPoints = hitPoints;
}

void Item::modifyHitPoints( float difference )
{
    if( mDestructible )
        mHitPoints += difference;
}

void Item::setActivationState( bool state )
{
    mActivated = state;
}

bool Item::getActivationState() const
{
    return mActivated;
}

std::string Item::getFileName() const
{
    return mCacheID;
}

Item *Item::getItemFromEntity( Entity *entity )
{
    EntityItemMap::iterator x = sEntityItemMap.find( entity );

    if( x != sEntityItemMap.end() )
        return x->second;
    else
        return 0;
}

void Item::registerScripts()
{
    boost::optional<boost::property_tree::ptree&> itemTree =
        mProperties->get_child_optional( "Item" );

    if( !itemTree )
        return;

    for( boost::property_tree::ptree::iterator x = ( *itemTree ).begin();
         x != ( *itemTree ).end(); ++x )
    {
        if( x->first == "Script" )
        {
            uint8_t actionID =
                    x->second.get<uint8_t>( "<xmlattr>.Action", EIAID_FIRST_ACTION );
            PythonScriptPtr script( new PythonScript( x->second.data(),
                                     x->second.get( "<xmlattr>.File", false ) ) );
            mScripts.insert( std::make_pair( actionID, script ) );

            _LOG( "Added script for action", (int)actionID );
        }
    }
}
