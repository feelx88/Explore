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


#include <IrrlichtTools.h>

using namespace irr;
using namespace core;
using namespace gui;

IrrlichtDevicePtr IrrlichtTools::createIrrlichtDevicePtr(IrrlichtDevice *device)
{
    return IrrlichtDevicePtr( device, specialDeleters::IrrlichtDeviceDeleter() );
}

irr::gui::IGUIButton *IrrlichtTools::guiCenterFittingButton( IGUIEnvironment *env,
                                                     const std::wstring &label,
                                                     int x, int y )
{
    const wchar_t *labelW = label.c_str();
    dimension2du dim= env->getSkin()->getFont()->getDimension( labelW );
    position2di pos( x - dim.Width, y - dim.Height );
    return env->addButton( recti( pos, dim * 2 ), 0, -1, labelW );
}

void IrrlichtTools::guiCenterElement( IGUIElementPtr element, int x, int y )
{
    recti pos = element->getAbsolutePosition();
    element->move( position2di( -( pos.getCenter().X + pos.getWidth() / 2 ),
                                -( pos.getCenter().Y + pos.getHeight() / 2 ) ) );
    pos = element->getAbsolutePosition();
    element->move( position2di( x - pos.getCenter().X, y - pos.getCenter().Y ) );
}
