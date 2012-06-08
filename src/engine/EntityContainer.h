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
    EntityContainer(IrrlichtDevicePtr device, BulletWorldPtr world,
                    const std::string &propFileName = "" );
    EntityContainer(IrrlichtDevicePtr device, BulletWorldPtr world,
                    PropTreePtr properties, const std::string &basePath = "" );

    void addEntity( EntityPtr entity, const int &id, const std::string &name = "" );

    EntityPtr getEntity( const int &id ) const;
    EntityPtr getEntity( const std::string &name ) const;

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
