#include "PathTools.h"

using namespace boost;
using namespace filesystem3;

PathVector PathTools::sPaths;

std::string PathTools::getAbsolutePath( const std::string &fileName,
                                        const std::string &additionalBasePath )
{
    path file( fileName );

    if( !additionalBasePath.empty() )
    {
        path x( additionalBasePath );
        if( exists( x / file ) )
            return absolute( x / file ).string();
    }

    if( exists( file ) )
        return absolute( file ).string();

    for( PathVector::iterator x = sPaths.begin(); x != sPaths.end(); ++x )
    {
        if( exists( *x / file ) )
            return absolute( *x / file ).string();
    }

    return fileName;
}

void PathTools::addPath( const std::string &pathName )
{
    sPaths.push_back( path( pathName ) );
}

std::string PathTools::getAbsoluteFileNameFromFolder( const std::string &folder,
                                                      const std::string &extension )
{
    path p( getAbsolutePath( folder ) );
    if( is_directory( p ) )
    {
        p /= p.stem();
        p.replace_extension( std::string( "." ) + extension );
    }

    return p.string();
}

std::string PathTools::getBasePathFromFile( const std::string &fileName )
{
    path f( fileName );
    if( is_directory( f ) )
        return fileName;
    else
        return f.parent_path().string();
}
