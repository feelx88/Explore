#ifndef GUITOOLS_H
#define GUITOOLS_H

#include <string>
#include <irrlicht/irrlicht.h>

class GUITools
{
public:
    static irr::gui::IGUIButton *centerFittingButton( irr::gui::IGUIEnvironment *env,
                                                      const std::wstring &label,
                                                       int x, int y );
};

#endif // GUITOOLS_H
