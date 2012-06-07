#ifndef PATHTOOLS_H
#define PATHTOOLS_H

#include "EngineTypedefs.h"
#include <vector>
#include <boost/filesystem.hpp>

typedef std::vector<boost::filesystem3::path> PathVector;

class PathTools
{
public:
    static std::string getAbsolutePath( std::string fileName );

    static void addPath( std::string path );

private:
    static PathVector sPaths;
};

#endif // PATHTOOLS_H
