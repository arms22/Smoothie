#ifndef JOHANNKOSSELSOLUTION_H
#define ROSTOCKSOLUTION_H
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "BaseSolution.h"
#include "libs/nuts_bolts.h"

#include "libs/Config.h"

#define arm_length_checksum         CHECKSUM("arm_length")
#define arm_radius_checksum         CHECKSUM("arm_radius")

#define arm_length_correction_a_checksum    CHECKSUM("arm_length_correction_a")
#define arm_length_correction_b_checksum    CHECKSUM("arm_length_correction_b")
#define arm_length_correction_c_checksum    CHECKSUM("arm_length_correction_c")

#define arm_radius_correction_a_checksum    CHECKSUM("arm_radius_correction_a")
#define arm_radius_correction_b_checksum    CHECKSUM("arm_radius_correction_b")
#define arm_radius_correction_c_checksum    CHECKSUM("arm_radius_correction_c")

class JohannKosselSolution : public BaseSolution {
    public:
        JohannKosselSolution(Config*);
        void cartesian_to_actuator( float[], float[] );
        void actuator_to_cartesian( float[], float[] );

        bool set_optional(char parameter, float value);
        bool get_optional(char parameter, float *value);

    private:
        void init();

        float arm_length;
        float arm_radius;
        float arm_length_squared[3];

        float DELTA_TOWER1_X;
        float DELTA_TOWER1_Y;
        float DELTA_TOWER2_X;
        float DELTA_TOWER2_Y;
        float DELTA_TOWER3_X;
        float DELTA_TOWER3_Y;

        float arm_length_correction[3];
        float arm_radius_correction[3];
};
#endif // JOHANNKOSSELSOLUTION_H

