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


#ifndef ENTITYCONTAINER_H
#define ENTITYCONTAINER_H

#include "EngineTypedefs.h"
#include "Entity.h"

#include <boost/unordered_map.hpp>

typedef boost::unordered::unordered_map<int,EntityPtr> EntityMap;
typedef boost::unordered::unordered_map<std::string,int> IDMap;

class EntityContainer;
typedef boost::shared_ptr<EntityContainer> EntityContainerPtr;

class EntityContainer
{
public:
    EntityContainer( IrrlichtDevicePtr device, BulletWorldPtr world,
                     const std::string &propFileName = "" );
    EntityContainer( IrrlichtDevicePtr device, BulletWorldPtr world,
                     PropTreePtr properties, const std::string &basePath = "" );

    virtual ~EntityContainer();

    void addEntity(EntityPtr entity, int id, const std::string &name = "" );

    EntityPtr getEntity( const int &id ) const;
    EntityPtr getEntity( const std::string &name ) const;

    const EntityMap &getEntities() const;

    PropTreePtr getProperties() const;
private:
    void create();

    IrrlichtDevicePtr mDevice;
    BulletWorldPtr mBulletWorld;

    PropTreePtr mProperties;
    EntityMap mEntities;
    IDMap mIDs;

    std::string mBasePath;
};

#endif // ENTITYCONTAINER_H
