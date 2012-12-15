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

#ifndef PYTHONBINDER_H
#define PYTHONBINDER_H

#include "EngineTypedefs.h"
#include "PythonTools.h"
#include "LoggerSingleton.h"

class PythonBinder;
typedef boost::shared_ptr<PythonBinder> PythonBinderPtr;

class PythonBinder
{
public:
    static void registerAll();

protected:
    virtual void reg() = 0;

    static int registerBinder( PythonBinder *binder );

    static boost::shared_ptr<std::vector<PythonBinderPtr> > sBinders;
};

#define PYTHONBINDER_REGISTER_MODULE( NAME )\
void NAME ## reg();\
class NAME ## Binder : public PythonBinder\
{\
    public:\
    void reg()\
    {\
        NAME ## reg();\
    }\
    private:\
    static int regDummy;\
};\
int NAME ## Binder::regDummy = PythonBinder::registerBinder( new NAME ## Binder );\
void NAME ## reg()

#endif // PYTHONBINDER_H
