#include "EntityContainer.h"
#include "LoggerSingleton.h"
#include "PathTools.h"
#include <boost/property_tree/xml_parser.hpp>

EntityContainer::EntityContainer( IrrlichtDevicePtr device, BulletWorldPtr world,
                                  const std::string &propFileName )
    : mDevice( device ),
      mBulletWorld( world ),
      mBasePath( "" )
{
    mProperties.reset( new boost::property_tree::ptree() );

    if( propFileName.empty() )
        return;

    std::string fileName = PathTools::getAbsoluteFileNameFromFolder( propFileName, "xml" );
    mBasePath = PathTools::getBasePathFromFile( fileName );

    boost::property_tree::xml_parser::read_xml( fileName, *mProperties );
    create();
}

EntityContainer::EntityContainer( IrrlichtDevicePtr device, BulletWorldPtr world,
                                  PropTreePtr properties, const std::string &basePath )
    : mDevice( device ),
      mBulletWorld( world ),
      mProperties( properties )
{
    mBasePath = PathTools::getAbsolutePath( basePath );
    create();
}

void EntityContainer::addEntity( EntityPtr entity, const int &id, const std::string &name )
{
    mEntities.insert( std::make_pair( id, entity ) );
    mIDs.insert( std::make_pair( name, id ) );
}

EntityPtr EntityContainer::getEntity( const int &id ) const
{
    return mEntities.at( id );
}

EntityPtr EntityContainer::getEntity( const std::string &name ) const
{
    return mEntities.at( mIDs.at( name ) );
}

PropTreePtr EntityContainer::getProperties() const
{
    return mProperties;
}

void EntityContainer::create()
{
    for( boost::property_tree::ptree::iterator x = mProperties->begin();
         x != mProperties->end(); ++x )
    {
        if( x->first == "Entity" )
        {
            int id = x->second.get<int>( "<xmlattr>.ID" );
            std::string name = x->second.get( "<xmlattr>.Name", std::string() );

            PropTreePtr props( new boost::property_tree::ptree() );
            props->put_child( "Entity", x->second );
            EntityPtr entity( new Entity( mDevice, mBulletWorld, props, mBasePath ) );

            mEntities.insert( std::make_pair( id, entity ) );
            mIDs.insert( std::make_pair( name, id ) );
        }
    }
}
