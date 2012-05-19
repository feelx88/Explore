#include "GUITools.h"

using namespace irr;
using namespace core;
using namespace gui;

irr::gui::IGUIButton *GUITools::centerFittingButton( IGUIEnvironment *env,
                                                     const std::wstring &label,
                                                     int x, int y )
{
    const wchar_t *labelW = label.c_str();
    dimension2du dim= env->getSkin()->getFont()->getDimension( labelW );
    position2di pos( x - dim.Width, y - dim.Height );
    return env->addButton( recti( pos, dim * 2 ), 0, -1, labelW );
}
