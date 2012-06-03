#include "EntityContainer.h"
#include "LoggerSingleton.h"
#include <boost/property_tree/xml_parser.hpp>

EntityContainer::EntityContainer( IrrlichtDevicePtr device, BulletWorldPtr world,
                                 std::string propFileName )
    : mDevice( device ),
      mBulletWorld( world )
{
    if( propFileName.find( ".xml" ) == std::string::npos )
    {
        mBasePath = propFileName + "/";
        std::string file = propFileName.substr( propFileName.find_last_of( '/' ) );
        propFileName = propFileName + file + ".xml";
    }
    else
    {
        mBasePath = propFileName.substr( 0, propFileName.find_last_of( '/' ) ) + "/";
    }

    mProperties.reset( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml( propFileName, *mProperties );
    create();
}

EntityContainer::EntityContainer( IrrlichtDevicePtr device, BulletWorldPtr world,
                                  PropTreePtr properties, std::string basePath )
    : mDevice( device ),
      mBulletWorld( world ),
      mProperties( properties )
{
    mBasePath = basePath;
    create();
}

EntityPtr EntityContainer::getEntity( const int &id )
{
    return mEntities.at( id );
}

EntityPtr EntityContainer::getEntity( const std::string &name )
{
    return mEntities.at( mIDs.at( name ) );
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