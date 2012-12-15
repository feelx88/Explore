/*
    Copyright 2012 Felix Müller.

    This file is part of %ProjectName%.

    %ProjectName% is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    %ProjectName% is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with %ProjectName%.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PythonBinder.h"

boost::shared_ptr<std::vector<PythonBinderPtr> > PythonBinder::sBinders(
        PythonBinder::sBinders );

void PythonBinder::registerAll()
{
    foreach_( PythonBinderPtr &binder, *PythonBinder::sBinders )
    {
        binder->reg();
    }
}

int PythonBinder::registerBinder( PythonBinder *binder )
{
    if( !sBinders )
        sBinders.reset( new std::vector<PythonBinderPtr>() );

    sBinders->push_back( PythonBinderPtr( binder ) );
    return 0;
}