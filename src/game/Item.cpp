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

LUABINDER_REGISTER_MODULE_START( ItemBinder )
    class_<Item>( "Item" )
LUABINDER_REGISTER_MODULE_END( ItemBinder )

Item::Item( ExplorePtr explore, IPlayerPtr owner, PropTreePtr properties,
            const std::string &basePath )
    : NetworkSyncable(),
      mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mBulletWorld( explore->getBulletWorld() ),
      mLua( explore->getLuaVM() ),
      mProperties( PropTreePtr( new boost::property_tree::ptree( *properties ) ) ),
      mBasePath( basePath ),
      mOwner( owner ),
      mGUI( 0 )
{
    setTypeID( ENTI_ITEM );
    loadIcon();

    mEntities.reset( new EntityContainer( mDevice, mBulletWorld, mProperties, mBasePath ) );

    registerScripts();

    if( mProperties->get( "Item.AutoAddEntities", false ) )
        registerEntities();

    if( mProperties->get( "Item.AutoActivation", false ) )
        setActivationState( true );

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

void Item::serializeInternal( NetworkSyncablePacket &packet, uint8_t actionID )
{
    if( actionID == ENGA_CREATE )
    {
        packet.writeString( mFileName );
        packet.writeUInt32( mOwner.lock()->getUID() );

        const EntityMap &entities = mEntities->getEntities();

        packet.writeUInt16( entities.size() );

        _LOG( "**************************************************" );
        _LOG( "UID", getUID() );
        _LOG( "#entities", entities.size() );

        foreach_( const EntityMap::value_type &x, entities )
        {
            RigidBodyPtr body = x.second->getRigidBody();
            btTransform trans = body->getWorldTransform();
            btVector3 origin = trans.getOrigin();
            btQuaternion rotation = trans.getRotation();

            float xx = origin.getX();
            float xy = origin.getY();
            float xz = origin.getZ();

            float yx = rotation.getX();
            float yy = rotation.getY();
            float yz = rotation.getZ();
            float yw = rotation.getW();

            _LOG( "xx", xx );
            _LOG( "xy", xy );
            _LOG( "xz", xz );
            _LOG( "yx", yx );
            _LOG( "yy", yy );
            _LOG( "yz", yz );
            _LOG( "yw", yw );

            packet.writeFloat( xx );//trans.getOrigin().getX() );
            packet.writeFloat( xy );//trans.getOrigin().getY() );
            packet.writeFloat( xz );//trans.getOrigin().getZ() );

            packet.writeFloat( yx );//trans.getRotation().getX() );
            packet.writeFloat( yy );//trans.getRotation().getY() );
            packet.writeFloat( yz );//trans.getRotation().getZ() );
            packet.writeFloat( yw );//trans.getRotation().getW() );
        }
        _LOG( "**************************************************" );

        return;
    }
}

boost::optional<NetworkSyncablePacket> Item::deserializeInternal(
        NetworkSyncablePacket &packet )
{
    if( packet.getActionID() == ENGA_CREATE )
    {
        //String(name) and UInt32(ownerID) already read

        uint16_t size = packet.readUInt16();//TODO: range check?

        const EntityMap &entities = mEntities->getEntities();

        _LOG( "**************************************************" );
        _LOG( "UID", getUID() );
        _LOG( "#entities", size );

        foreach_( const EntityMap::value_type &x, entities )
        {
            float xx = packet.readFloat();
            float xy = packet.readFloat();
            float xz = packet.readFloat();

            float yx = packet.readFloat();
            float yy = packet.readFloat();
            float yz = packet.readFloat();
            float yw = packet.readFloat();

            _LOG( "xx", xx );
            _LOG( "xy", xy );
            _LOG( "xz", xz );
            _LOG( "yx", yx );
            _LOG( "yy", yy );
            _LOG( "yz", yz );
            _LOG( "yw", yw );

            btVector3 origin( xx,//packet.readFloat(),
                              xy,//packet.readFloat(),
                              xz );//packet.readFloat() );

            btQuaternion rotation( yx,//packet.readFloat(),
                                   yy,//packet.readFloat(),
                                   yz,//packet.readFloat(),
                                   yw );//packet.readFloat() );

            RigidBodyPtr body = x.second->getRigidBody();
            body->setWorldTransform( btTransform( rotation, origin ) );
        }
        _LOG( "**************************************************" );
    }
    return boost::none;
}

void Item::startAction( E_ITEM_ACTION actionID )
{
    ScriptMap::iterator x = mScripts.find( actionID );
    if( x == mScripts.end() )
        return;
    else
        mScripts.at( actionID )->exec();
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
    return mFileName;
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
            E_ITEM_ACTION actionID =
                    x->second.get( "<xmlattr>.Action", EIA_FIRST_ACTION );
            LuaScriptPtr script( new LuaScript(
                                     mLua, x->second.data(),
                                     x->second.get( "<xmlattr>.File", false ) ) );
            mScripts.insert( std::make_pair( actionID, script ) );

            _LOG( "Added script for action", actionID );
        }
    }
}
