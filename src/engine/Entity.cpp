#include "Entity.h"
#include "IrrlichtTools.h"
#include "LoggerSingleton.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

Entity::Entity( const PropTreePtr &properties )
    : mProperties( properties )
{
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
}

void Entity::internalCreateRigidBody()
{
}

EntityPtr Entity::create( const PropTreePtr &properties )
{
    Entity *e = new Entity( properties );
    e->internalCreate();

    return EntityPtr( e );
}

EntityPtr Entity::create( const std::string &propFileName )
{
    boost::property_tree::ptree *ptree = new boost::property_tree::ptree();
    boost::property_tree::xml_parser::read_xml( propFileName, *ptree );

    Entity *e = new Entity( PropTreePtr( ptree ) );
    e->internalCreate();

    return EntityPtr( e );
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
