#ifndef PATHTOOLS_H
#define PATHTOOLS_H

#include "EngineTypedefs.h"
#include <vector>
#include <boost/filesystem.hpp>

typedef std::vector<boost::filesystem3::path> PathVector;

class PathTools
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
