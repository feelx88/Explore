#include "PathTools.h"

PathVector PathTools::sPaths;

std::string PathTools::getAbsolutePath( std::string fileName )
{
    boost::filesystem3::path file( fileName );
    for( PathVector::iterator x = sPaths.begin(); x != sPaths.end(); ++x )
    {
        if( boost::filesystem3::exists( *x / file ) )
            return boost::filesystem3::absolute( *x / file ).string();
    }

    return fileName;
}

void PathTools::addPath( std::string path )
{
    sPaths.push_back( boost::filesystem3::path( path ) );
}
