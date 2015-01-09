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

class StreamOutput;

// Object to represent a Gcode command
class Gcode {
    public:
        Gcode(const string&, StreamOutput*, bool strip=true);
        Gcode(const Gcode& to_copy);
        Gcode& operator= (const Gcode& to_copy);
        ~Gcode();

        const char* get_command() const { return command; }
        float get_value ( char letter, char **ptr= nullptr ) const;
        int get_int ( char letter, char **ptr= nullptr ) const;
        uint32_t get_uint ( char letter, char **ptr= nullptr ) const;
        int get_num_args() const;
        void mark_as_taken();
        void strip_parameters();

        // FIXME these should be private
        unsigned int m;
        unsigned int g;
        float millimeters_of_travel;
        float x, y, z, e, f;

        struct {
            bool add_nl:1;
            bool accepted_by_module:1;
            int  letter_bit:26;
        };

        #define LETTER_BIT(letter)  (1 << ((letter) - 'A'))
        inline bool has_letter(char c) const {
          return ((letter_bit & LETTER_BIT( c )) != 0);
        }
        #define has_g   has_letter('G')
        #define has_m   has_letter('M')

        StreamOutput* stream;
        string txt_after_ok;

    private:
        void prepare_cached_values(bool strip=true);
        char *command;
};
#endif
