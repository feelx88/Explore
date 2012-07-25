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

#include "FloatToBinSerializer.h"
#include <sstream>
#include <cmath>
#include <cstring>
#include <iostream>

//TODO: make float mantissa size independant

/*
 * Idea: Split into normalized mantissa and exponent, shift mantissa comma to the
 * end and send both of them as integers. Sadly, this gives 32bit overhead over
 * transferring float bits, but its more portable.
 */

std::string FloatToBinSerializer::serialize( float value )
{
    std::stringstream stream;

    bool negative = value < 0.f;

    if( negative )
        value *= -1.f;

    int exponent = 0;
    float mantissa = frexp( value, &exponent );
    unsigned int mantissaInt = ldexp( mantissa, 23 ); //23 because of ieee 754

    if( negative )
        exponent *= -1;

    char exponentBits[4], mantissaBits[4];
    memcpy( &exponentBits, &exponent, 4 );
    memcpy( &mantissaBits, &mantissaInt, 4 );

    stream.write( exponentBits, 4 );
    stream.write( mantissaBits, 4 );

    return stream.str();
}

float FloatToBinSerializer::deserialize( const std::string &value )
{
    unsigned int mantissaInt = 0;
    int exponent = 0;

    bool negative= false;

    const char *bits = value.c_str();

    memcpy( &exponent, bits, 4 );
    memcpy( &mantissaInt, bits + 4, 4 );

    if( exponent < 0 )
    {
        exponent *= -1;
        negative = true;
    }

    float mantissa = (float)( mantissaInt ) / pow( 2.f, 23 ); //23 because of ieee 754

    float v = ldexp( mantissa, exponent );

    if( negative )
        v *= -1.f;

    return v;
}
