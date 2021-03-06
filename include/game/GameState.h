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


#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <boost/shared_ptr.hpp>

enum E_GAME_STATE
{
    EGS_QUIT = 0,
    EGS_MAIN_MENU,
    EGS_CONNECT,
    EGS_LOAD,
    EGS_GAME
};

enum E_NETWORKSYNCABLE_TYPE_IDS
{
    ENTI_EXPLORE_SERVER = 1,
    ENTI_ITEM,
    ENTI_PLAYER,
    ENTI_WORLD
};

enum E_ACTIONID
{
    EAID_UPDATE = 0,
    EAID_CREATE,
    EAID_DESTROY,
    EAID_COUNT
};

enum E_ITEM_ACTIONID
{
    EIAID_FIRST_ACTION = EAID_COUNT,
    EIAID_SECOND_ACTION,
    EIAID_USE_ACTION,
    EIAID_COUNT
};

class Explore;
typedef Explore* ExplorePtr;

#endif // GAMESTATE_H
