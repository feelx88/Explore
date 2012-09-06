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
#include <stdint.h>

#define MANTISSA_SIZE 23  //ieee 754
#define INT_SIZE 4

//TODO: make float mantissa size independant

/*
 * Idea: Split into normalized mantissa and exponent, shift mantissa comma to the
 * end and send both of them as integers. Sadly, this gives 32bit overhead over
 * transferring float bits at the moment, but its more portable.
 */

std::string FloatToBinSerializer::serialize( float value )
{
    std::stringstream stream;

    bool negative = value < 0.f;

    if( negative )
        value *= -1.f;

    int32_t exponent = 0;
    float mantissa = frexp( value, &exponent );
    uint32_t mantissaInt = ldexp( mantissa, MANTISSA_SIZE );

    if( negative )
        exponent *= -1;

    char exponentBits[INT_SIZE], mantissaBits[INT_SIZE];
    memcpy( &exponentBits, &exponent, INT_SIZE );
    memcpy( &mantissaBits, &mantissaInt, INT_SIZE );

    stream.write( exponentBits, INT_SIZE );
    stream.write( mantissaBits, INT_SIZE );

    return stream.str();
}

float FloatToBinSerializer::deserialize( const std::string &value )
{
    uint32_t mantissaInt = 0;
    int32_t exponent = 0;

    bool negative= false;

    const char *bits = value.c_str();

    memcpy( &exponent, bits, INT_SIZE );
    memcpy( &mantissaInt, bits + 4, INT_SIZE );

    if( exponent < 0 )
    {
        exponent *= -1;
        negative = true;
    }

    float mantissa = (float)( mantissaInt ) / pow( 2.f, MANTISSA_SIZE );

    float v = ldexp( mantissa, exponent );

    if( negative )
        v *= -1.f;

    return v;
}
