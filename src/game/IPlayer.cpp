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

#include "IPlayer.h"

#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>

#include "Item.h"
#include "ItemFactory.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

IPlayer::IPlayer( ExplorePtr explore, IPlayer *parent )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() ),
      mParent( parent )
{
    mProperties.reset( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                PathTools::getAbsoluteFileNameFromFolder( "Player", "xml" ),
                *mProperties );
}

IPlayer::~IPlayer()
{
}

void IPlayer::addOwnedItem( Item *item )
{
    mOwnedItems.push_back( ItemPtr( item ) );
}


EntityPtr IPlayer::getEntity() const
{
    return mEntity;
}

IPlayer *IPlayer::getParent() const
{
    return mParent;
}

void IPlayer::setParent(IPlayer *parent)
{
    mParent = parent;
}

vector3df IPlayer::rotateToDirection( vector3df dir ) const
{
    matrix4 m = mEntity->getSceneNode()->getAbsoluteTransformation();
    m.rotateVect( dir );

    return dir;
}
