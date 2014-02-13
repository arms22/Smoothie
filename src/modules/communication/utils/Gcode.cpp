/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/


#include <string>
using std::string;
#include "Gcode.h"
#include "libs/StreamOutput.h"
#include "utils.h"

#include <stdlib.h>

// This is a gcode object. It reprensents a GCode string/command, an caches some important values about that command for the sake of performance.
// It gets passed around in events, and attached to the queue ( that'll change )
Gcode::Gcode(const string& command, StreamOutput* stream) : command(command), m(0), g(0), x(.0F), y(.0F), z(.0F), e(.0F), f(.0F), add_nl(false), stream(stream) {
    prepare_cached_values();
    this->millimeters_of_travel = 0.0F;
    this->accepted_by_module=false;
}

Gcode::Gcode(const Gcode& to_copy){
    this->command.assign( to_copy.command );
    this->millimeters_of_travel = to_copy.millimeters_of_travel;
    this->f_has_letter          = to_copy.f_has_letter;
    this->m                     = to_copy.m;
    this->g                     = to_copy.g;
    this->x                     = to_copy.x;
    this->y                     = to_copy.y;
    this->z                     = to_copy.z;
    this->e                     = to_copy.e;
    this->f                     = to_copy.f;
    this->add_nl                = to_copy.add_nl;
    this->stream                = to_copy.stream;
    this->accepted_by_module=false;
    this->txt_after_ok.assign( to_copy.txt_after_ok );
}

Gcode& Gcode::operator= (const Gcode& to_copy){
    if( this != &to_copy ){
        this->command.assign( to_copy.command );
        this->millimeters_of_travel = to_copy.millimeters_of_travel;
        this->f_has_letter          = to_copy.f_has_letter;
        this->m                     = to_copy.m;
        this->g                     = to_copy.g;
        this->x                     = to_copy.x;
        this->y                     = to_copy.y;
        this->z                     = to_copy.z;
        this->e                     = to_copy.e;
        this->f                     = to_copy.f;
        this->add_nl                = to_copy.add_nl;
        this->stream                = to_copy.stream;
        this->txt_after_ok.assign( to_copy.txt_after_ok );
    }
    this->accepted_by_module=false;
    return *this;
}

// Retrieve the value for a given letter
// We don't use the high-level methods of std::string because they call malloc and it's very bad to do that inside of interrupts
float Gcode::get_value( char letter ){
    //__disable_irq();
    const char* cs = command.c_str();
    char* cn = NULL;
    for (; *cs; cs++){
         if( letter == *cs ){
             cs++;
             float r = strtof(cs, &cn);
             if (cn > cs)
                 return r;
         }
    }
    //__enable_irq();
    return 0;
}

int Gcode::get_int( char letter )
{
    const char* cs = command.c_str();
    char* cn = NULL;
    for (; *cs; cs++){
        if( letter == *cs ){
            cs++;
            int r = strtol(cs, &cn, 10);
            if (cn > cs)
                return r;
        }
    }
    return 0;
}

int Gcode::get_num_args(){
    int args = 0;
    for( char c = 'A'; c <= 'Z'; c++ ){
        if( c == 'G' ) continue;
        if( c == 'M' ) continue;
        if( f_has_letter & LETTER_BIT(c) ){
            args++;
        }
    }
    return args;
}

// Cache some of this command's properties, so we don't have to parse the string every time we want to look at them
void Gcode::prepare_cached_values(){
    const char* cs = command.c_str();
    char* cn;
    char c;
    do {
        c = *cs++;
        if( 'A' <= c && c <= 'Z' ){
            f_has_letter |= LETTER_BIT(c);
            if( c == 'G' ) {
                this->g = strtol(cs, &cn, 10);
                cs = cn;
            }else if( c == 'M' ){
                this->m = strtol(cs, &cn, 10);
                cs = cn;
            }else if( 'X' == c ){
                this->x = strtof(cs, &cn);
                cs = cn;
            }else if( 'Y' == c ){
                this->y = strtof(cs, &cn);
                cs = cn;
            }else if( 'Z' == c ){
                this->z = strtof(cs, &cn);
                cs = cn;
            }else if( 'E' == c ){
                this->e = strtof(cs, &cn);
                cs = cn;
            }else if( 'F' == c ){
                this->f = strtof(cs, &cn);
                cs = cn;
    }
    }
    } while( c != '\0' );
}

void Gcode::mark_as_taken(){
    this->accepted_by_module = true;
}
