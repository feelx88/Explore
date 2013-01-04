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


#ifndef PATHTOOLS_H
#define PATHTOOLS_H

#include "EngineTypedefs.h"
#include <vector>
#include <boost/filesystem.hpp>

typedef std::vector<boost::filesystem::path> PathVector;

class APIEXPORT PathTools
{
public:
    static std::string getAbsolutePath( const std::string &fileName,
                                        const std::string &additionalBasePath = "" );

    static void addPath( const std::string &pathName );

    static std::string getAbsoluteFileNameFromFolder( const std::string &folder,
                                                      const std::string &extension );

    static std::string getBasePathFromFile( const std::string &fileName );

private:
    static PathVector sPaths;
};

#endif // PATHTOOLS_H
