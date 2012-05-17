#ifndef ENGINETYPEDEFS_H
#define ENGINETYPEDEFS_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <irrlicht/IrrlichtDevice.h>

typedef boost::shared_ptr<irr::IrrlichtDevice> IrrlichtDevicePtr;
typedef irr::video::IVideoDriver* IVideoDriverPtr;

#endif // ENGINETYPEDEFS_H
