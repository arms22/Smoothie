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
        int    get_int    ( char letter );

        int    get_num_args();
        void   prepare_cached_values();
        void   mark_as_taken();

        #define LETTER_BIT(letter) (1 << ((letter) - 'A'))

        inline bool has_letter(char c) const {
          return f_has_letter & LETTER_BIT(c);
        }
        inline bool has_g() const {
          return f_has_letter & LETTER_BIT('G');
        }
        inline bool has_m() const {
          return f_has_letter & LETTER_BIT('M');
        }
        inline bool has_x() const {
          return f_has_letter & LETTER_BIT('X');
        }
        inline bool has_y() const {
          return f_has_letter & LETTER_BIT('Y');
        }
        inline bool has_z() const {
          return f_has_letter & LETTER_BIT('Z');
        }
        inline bool has_e() const {
          return f_has_letter & LETTER_BIT('E');
        }
        inline bool has_f() const {
          return f_has_letter & LETTER_BIT('F');
        }
        unsigned int f_has_letter;

        unsigned int g, m;
        float x, y, z, e, f;

        string command;
        float millimeters_of_travel;

        bool add_nl;
        StreamOutput* stream;

        string txt_after_ok;
        bool accepted_by_module;

};
#endif
