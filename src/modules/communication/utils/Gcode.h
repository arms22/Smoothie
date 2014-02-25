/*
      This file is part of Smoothie (http://smoothieware.org/). The motion control part is heavily based on Grbl (https://github.com/simen/grbl).
      Smoothie is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
      Smoothie is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
      You should have received a copy of the GNU General Public License along with Smoothie. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef GCODE_H
#define GCODE_H
#include <string>
using std::string;
#include "libs/StreamOutput.h"
// Object to represent a Gcode command
#include <stdlib.h>

class Gcode {
    public:
        Gcode(const string&, StreamOutput*);
        Gcode(const Gcode& to_copy); 
        Gcode& operator= (const Gcode& to_copy);

        float get_value  ( char letter );
        float get_double ( char letter );
        int   get_int    ( char letter );

        int   get_num_args();
        void  prepare_cached_values();
        void  mark_as_taken();

        struct {
          int  f_add_nl             :1;
          int  f_accepted_by_module :1;
          int  f_reserve            :4;
          int  f_has_letter         :26;
        } flags;

        #define LETTER_BIT(letter)  (1 << ((letter) - 'A'))

        inline bool has_letter(char c) const {
          return ((flags.f_has_letter & LETTER_BIT( c )) != 0);
        }
        inline bool has_g() const {
          return ((flags.f_has_letter & LETTER_BIT('G')) != 0);
        }
        inline bool has_m() const {
          return ((flags.f_has_letter & LETTER_BIT('M')) != 0);
        }
        inline bool has_x() const {
          return ((flags.f_has_letter & LETTER_BIT('X')) != 0);
        }
        inline bool has_y() const {
          return ((flags.f_has_letter & LETTER_BIT('Y')) != 0);
        }
        inline bool has_z() const {
          return ((flags.f_has_letter & LETTER_BIT('Z')) != 0);
        }
        inline bool has_e() const {
          return ((flags.f_has_letter & LETTER_BIT('E')) != 0);
        }
        inline bool has_f() const {
          return ((flags.f_has_letter & LETTER_BIT('F')) != 0);
        }

        #define add_nl              flags.f_add_nl
        #define accepted_by_module  flags.f_accepted_by_module

        uint16_t g, m;
        float x, y, z, e, f;
        float millimeters_of_travel;

        StreamOutput* stream;

        string command;
        string txt_after_ok;
};
#endif
