/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/


#include "Gcode.h"
#include "libs/StreamOutput.h"
#include "utils.h"
#include <stdlib.h>
#include <algorithm>

// This is a gcode object. It reprensents a GCode string/command, an caches some important values about that command for the sake of performance.
// It gets passed around in events, and attached to the queue ( that'll change )
Gcode::Gcode(const string &command, StreamOutput *stream, bool strip)
{
    this->command= strdup(command.c_str());
    this->m= 0;
    this->g= 0;
    this->x= 0;
    this->y= 0;
    this->z= 0;
    this->e= 0;
    this->f= 0;
    this->add_nl= false;
    this->stream= stream;
    this->millimeters_of_travel = 0.0F;
    this->accepted_by_module = false;
    prepare_cached_values(strip);
}

Gcode::~Gcode()
{
    if(command != nullptr) {
        // TODO we can reference count this so we share copies, may save more ram than the extra count we need to store
        free(command);
    }
}

Gcode::Gcode(const Gcode &to_copy)
{
    this->command               = strdup(to_copy.command); // TODO we can reference count this so we share copies, may save more ram than the extra count we need to store
    this->millimeters_of_travel = to_copy.millimeters_of_travel;
    this->letter_bit            = to_copy.letter_bit;
    this->m                     = to_copy.m;
    this->g                     = to_copy.g;
    this->x                     = to_copy.x;
    this->y                     = to_copy.y;
    this->z                     = to_copy.z;
    this->e                     = to_copy.e;
    this->f                     = to_copy.f;
    this->add_nl                = to_copy.add_nl;
    this->stream                = to_copy.stream;
    this->accepted_by_module    = false;
    this->txt_after_ok.assign( to_copy.txt_after_ok );
}

Gcode &Gcode::operator= (const Gcode &to_copy)
{
    if( this != &to_copy ) {
        this->command               = strdup(to_copy.command); // TODO we can reference count this so we share copies, may save more ram than the extra count we need to store
        this->millimeters_of_travel = to_copy.millimeters_of_travel;
        this->letter_bit            = to_copy.letter_bit;
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
    this->accepted_by_module = false;
    return *this;
}

// Retrieve the value for a given letter
float Gcode::get_value( char letter, char **ptr ) const
{
    if( 'X' == letter ){
        return this->x;
    }else if( 'Y' == letter ){
        return this->y;
    }else if( 'Z' == letter ){
        return this->z;
    }else if( 'E' == letter ){
        return this->e;
    }else if( 'F' == letter ){
        return this->f;
    }
    else{
    const char *cs = command;
    char *cn = NULL;
    for (; *cs; cs++) {
        if( letter == *cs ) {
            cs++;
            float r = strtof(cs, &cn);
            if(ptr != nullptr) *ptr= cn;
            if (cn > cs)
                return r;
        }
    }
    if(ptr != nullptr) *ptr= nullptr;
    }
    return 0;
}

int Gcode::get_int( char letter, char **ptr ) const
{
    const char *cs = command;
    char *cn = NULL;
    for (; *cs; cs++) {
        if( letter == *cs ) {
            cs++;
            int r = strtol(cs, &cn, 10);
            if(ptr != nullptr) *ptr= cn;
            if (cn > cs)
                return r;
        }
    }
    if(ptr != nullptr) *ptr= nullptr;
    return 0;
}

uint32_t Gcode::get_uint( char letter, char **ptr ) const
{
    const char *cs = command;
    char *cn = NULL;
    for (; *cs; cs++) {
        if( letter == *cs ) {
            cs++;
            int r = strtoul(cs, &cn, 10);
            if(ptr != nullptr) *ptr= cn;
            if (cn > cs)
                return r;
        }
    }
    if(ptr != nullptr) *ptr= nullptr;        
    return 0;
}

int Gcode::get_num_args() const
{
    int count = 0;
    for( char c = 'A'; c <= 'Z'; c++ ){
        if( c == 'G' ) continue; // ignore Gxxx
        if( c == 'M' ) continue; // ignore Mxxx
        if( has_letter(c) ){
            count++;
        }
    }
    return count;
}

// Cache some of this command's properties, so we don't have to parse the string every time we want to look at them
void Gcode::prepare_cached_values(bool strip)
{
    const char *cs = command;
    char *p = nullptr;
    char c;
    letter_bit = 0;
    do {
        c = *cs++;
        if ('A' <= c && c <= 'Z') {
            if(letter_bit & LETTER_BIT(c)){
                ;
            } else {
                letter_bit |= LETTER_BIT(c);
                if ('G' == c) {
                    this->g = strtol(cs, &p, 10);
                    cs = p;
                }else if ('M' == c) {
                    this->m = strtol(cs, &p, 10);
                    cs = p;
                }else if( 'X' == c ){
                    this->x = strtof(cs, &p);
                    cs = p;
                }else if( 'Y' == c ){
                    this->y = strtof(cs, &p);
                    cs = p;
                }else if( 'Z' == c ){
                    this->z = strtof(cs, &p);
                    cs = p;
                }else if( 'E' == c ){
                    this->e = strtof(cs, &p);
                    cs = p;
                }else if( 'F' == c ){
                    this->f = strtof(cs, &p);
                    cs = p;
                }
            }
        }
    } while (c != '\0');

    if(!strip) return;

    // remove the Gxxx or Mxxx from string
    if (p != nullptr) {
        char *n= strdup(p); // create new string starting at end of the numeric value
        free(command);
        command= n;
    }
}

void Gcode::mark_as_taken()
{
    this->accepted_by_module = true;
}

// strip off X Y Z I J K parameters if G0/1/2/3
void Gcode::strip_parameters()
{
    if(has_g && g < 4){
        // strip the command of the XYZIJK parameters
        string newcmd;
        char *cn= command;
        // find the start of each parameter
        char *pch= strpbrk(cn, "XYZIJK");
        while (pch != nullptr) {
            if(pch > cn) {
                // copy non parameters to new string
                newcmd.append(cn, pch-cn);
            }
            // find the end of the parameter and its value
            char *eos;
            strtof(pch+1, &eos);
            cn= eos; // point to end of last parameter
            pch= strpbrk(cn, "XYZIJK"); // find next parameter
        }
        // append anything left on the line
        newcmd.append(cn);

        // strip whitespace to save even more, this causes problems so don't do it
        //newcmd.erase(std::remove_if(newcmd.begin(), newcmd.end(), ::isspace), newcmd.end());

        // release the old one
        free(command);
        // copy the new shortened one
        command= strdup(newcmd.c_str());
    }
}
