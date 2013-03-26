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

#ifndef PYTHONBINDER_H
#define PYTHONBINDER_H

#include <EngineTypedefs.h>
#include <PythonTools.h>
#include <LoggerSingleton.h>

class PythonBinder;
typedef boost::shared_ptr<PythonBinder> PythonBinderPtr;

class APIEXPORT PythonBinder
{
public:
    PythonBinder( int prio = 0 );
    friend bool compareBinders( const PythonBinderPtr&, const PythonBinderPtr& );
    static void registerAll();

protected:
    virtual void reg() = 0;

    static int registerBinder( PythonBinder *binder );

    static boost::shared_ptr<std::vector<PythonBinderPtr> > sBinders;
    int priority;
};

#define PYTHONBINDER_REGISTER_MODULE( NAME ) PYTHONBINDER_REGISTER_MODULE_PRIORITY( NAME, 0 )

#define PYTHONBINDER_REGISTER_MODULE_PRIORITY( NAME, PRIO )\
class NAME ## Binder : public PythonBinder\
{\
    public:\
    NAME ## Binder()\
    : PythonBinder( PRIO )\
    {}\
    void reg();\
    private:\
    static int regDummy;\
};\
int NAME ## Binder::regDummy = PythonBinder::registerBinder( new NAME ## Binder );\
void NAME ## Binder::reg()

#endif // PYTHONBINDER_H
