/*
    Copyright 2013 Felix Müller.

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

#ifndef NETWORKTOOLS_H
#define NETWORKTOOLS_H

#include "EngineTypedefs.h"

class APIEXPORT NetworkTools
{
public:
    static StringVector resolveName( const std::string &name,
                                     IOServicePtr io = IOServicePtr() );
};

#endif // NETWORKTOOLS_H
