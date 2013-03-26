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


#include <PathTools.h>

using namespace boost;
using namespace filesystem;

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
