#include "Entity.h"
#include "PropertyTreeTranslators.h"
#include "IrrlichtTools.h"
#include "LoggerSingleton.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

Entity::Entity( const PropTreePtr &properties )
    : mProperties( properties )
{
    create();
}

Entity::Entity( const std::string &propFileName )
    : mProperties( new boost::property_tree::ptree() )
{
    boost::property_tree::xml_parser::read_xml( propFileName, *mProperties );
    create();
}

void Entity::internalCreate()
{
    preCreate();

    internalCreateSceneNode();
    internalCreateRigidBody();

    postCreate();
}

void Entity::internalCreateSceneNode()
{
    //mProperties->put( "lol", irr::core::vector3df(), boost::property_tree::VectorTranslator );
}

void Entity::internalCreateRigidBody()
{
}

Entity::~Entity()
{
}

void Entity::create()
{
    internalCreate();
}

void Entity::preCreate()
{
}

void Entity::postCreate()
{
}
