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

#include "PythonBinder.h"
#include <typeinfo>

boost::shared_ptr<std::vector<PythonBinderPtr> > PythonBinder::sBinders(
        PythonBinder::sBinders );

bool compareBinders( const PythonBinderPtr &a, const PythonBinderPtr &b )
{
    return a->priority < b->priority;
}

PythonBinder::PythonBinder( int prio )
    : priority( prio )
{
}

void PythonBinder::registerAll()
{
    std::sort( sBinders->begin(), sBinders->end(), compareBinders );
    foreach_( PythonBinderPtr &binder, *sBinders )
        binder->reg();
}

int PythonBinder::registerBinder( PythonBinder *binder )
{
    if( !sBinders )
        sBinders.reset( new std::vector<PythonBinderPtr>() );

    sBinders->push_back( PythonBinderPtr( binder ) );
    return 0;
}
