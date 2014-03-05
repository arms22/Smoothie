#ifndef ROSTOCKSOLUTION_H
#define ROSTOCKSOLUTION_H
#include "libs/Module.h"
#include "libs/Kernel.h"
#include "BaseSolution.h"
#include "libs/nuts_bolts.h"

#include "libs/Config.h"

#define arm_length_checksum         CHECKSUM("arm_length")
#define arm_radius_checksum         CHECKSUM("arm_radius")

#define alpha_angle_checksum                 CHECKSUM("alpha_angle")
#define beta_relative_angle_checksum         CHECKSUM("beta_relative_angle")
#define gamma_relative_angle_checksum        CHECKSUM("gamma_relative_angle")

#define arm_length_correction_a_checksum    CHECKSUM("arm_length_correction_a")
#define arm_length_correction_b_checksum    CHECKSUM("arm_length_correction_b")
#define arm_length_correction_c_checksum    CHECKSUM("arm_length_correction_c")

#define arm_radius_correction_a_checksum    CHECKSUM("arm_radius_correction_a")
#define arm_radius_correction_b_checksum    CHECKSUM("arm_radius_correction_b")
#define arm_radius_correction_c_checksum    CHECKSUM("arm_radius_correction_c")

class RostockSolution : public BaseSolution {
    public:
        RostockSolution(Config*);
        void cartesian_to_actuator( float[], float[] );
        void actuator_to_cartesian( float[], float[] );

        bool set_optional(char parameter, float value);
        bool get_optional(char parameter, float *value);
        
        float solve_arm( int tower, float millimeters[] );
        void rotate( float in[], float out[], float sin, float cos );

        float arm_length;
        float arm_radius;
        float arm_length_squared[3];
        float arm_length_correction[3];
        float arm_radius_correction[3];
        float arm_radius_corrected[3];

        float sin_alpha;
        float cos_alpha;
        float sin_beta;
        float cos_beta;
        float sin_gamma;
        float cos_gamma;
    private:
        void init();
};






#endif // ROSTOCKSOLUTION_H
