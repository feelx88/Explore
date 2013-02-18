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

#include "VisualPlayer.h"

using namespace irr;
using namespace core;

VisualPlayer::VisualPlayer( ExplorePtr explore, IPlayerPtr parent )
    : IPlayer( explore, parent ),
      mActiveItem( -1 )
{
    //FIXME:search for another way to create the (right) player model
    mEntity.reset( new Entity( mExplore->getIrrlichtDevice(),
                               mExplore->getBulletWorld(),
                               "TestCube" ) );
}

VisualPlayer::~VisualPlayer()
{
}

EntityPtr VisualPlayer::getEntity() const
{
    return mEntity;
}

vector3df VisualPlayer::rotateToDirection(vector3df dir) const
{
    matrix4 m = mEntity->getSceneNode()->getAbsoluteTransformation();
    m.rotateVect( dir );

    return dir;
}

vector3df VisualPlayer::getPosition() const
{
    return *mEntity->getPosition();
}

quaternion VisualPlayer::getRotation() const
{
    return *mEntity->getRotation();
}

ItemPtr VisualPlayer::getActiveItem() const
{
    if( mActiveItem < 0 )
        return ItemPtr();

    return mInventory.at( mActiveItem );
}

void VisualPlayer::setClientID( uint32_t clientID )
{
    mClientID = clientID;
}

uint32_t VisualPlayer::clientID() const
{
    return mClientID;
}

void VisualPlayer::update()
{
}

void VisualPlayer::serializeInternal( NetworkSyncablePacket &packet,
                                      uint8_t actionID )
{
    if( actionID == EAID_CREATE || actionID == EAID_UPDATE )
    {
        packet.writeUInt32( mClientID );

        vector3df position = *mEntity->getPosition();
        vector3df rotation = *mEntity->getRotation();

        packet.writeFloat( position.X );
        packet.writeFloat( position.Y );
        packet.writeFloat( position.Z );

        packet.writeFloat( rotation.X );
        packet.writeFloat( rotation.Y );
        packet.writeFloat( rotation.Z );
    }
}

boost::optional<NetworkSyncablePacket> VisualPlayer::deserializeInternal(
        NetworkSyncablePacket &packet )
{
    if( packet.getActionID() == EAID_CREATE || packet.getActionID() == EAID_UPDATE )
    {
        //uint32_t [clientID] already read on EAID_CREATE
        if( packet.getActionID() == EAID_UPDATE )
        {
            packet.readUInt32();
        }

        float positionX = packet.readFloat();
        float positionY = packet.readFloat();
        float positionZ = packet.readFloat();

        float rotationX = packet.readFloat();
        float rotationY = packet.readFloat();
        float rotationZ = packet.readFloat();

        mEntity->setPosition( vector3df( positionX, positionY, positionZ ) );
        mEntity->setRotation( vector3df( rotationX, rotationY, rotationZ ) );
    }

    return boost::none;
}
