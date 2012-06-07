#include "Item.h"
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;

Item::Item(ExplorePtr explore, Player *owner, std::string fileName )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mBulletWorld( explore->getBulletWorld() ),
      mLua( explore->getLuaVM() ),
      mOwner( owner )
{
    if( fileName.empty() )
        return;

    mProperties.reset( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml( fileName, *mProperties );

    create();
    loadIcon();
}

Item::~Item()
{
}

void Item::create()
{
    mEntities.reset( new EntityContainer( mDevice, mBulletWorld, mProperties ) );

    for( boost::property_tree::ptree::iterator x = mProperties->begin();
         x != mProperties->end(); ++x )
    {
        if( x->first == "Script" )
        {
            LuaScriptPtr script( new LuaScript(
                                     mLua, x->second.data(),
                                     x->second.get( "<xmlattr>.File", false ) ) );
            mScripts.push_back( script );
        }
    }
}

void Item::loadIcon()
{
    std::string iconFileName = mProperties->get( "Item.Icon", "data/Textures/defaultIcon.png" );

    mIcon = mDevice->getVideoDriver()->getTexture( iconFileName.c_str() );
}


void Item::startAction( int actionID )
{
    mScripts.at( actionID )->exec();
}

IGUIElementPtr Item::getGUIElement() const
{
    return IGUIElementPtr();
}

ITexturePtr Item::getIcon() const
{
    return mIcon;
}
