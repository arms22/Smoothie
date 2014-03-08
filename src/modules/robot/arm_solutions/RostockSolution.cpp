#include "RostockSolution.h"
#include <fastmath.h>

#define PIOVER180       0.01745329251994329576923690768489F

RostockSolution::RostockSolution(Config* config)
{
    float alpha_angle  = PIOVER180 * config->value(alpha_angle_checksum)->by_default(30.0f)->as_number();
    sin_alpha     = sinf(alpha_angle);
    cos_alpha     = cosf(alpha_angle);
    float beta_angle   = PIOVER180 * config->value( beta_relative_angle_checksum)->by_default(120.0f)->as_number();
    sin_beta      = sinf(beta_angle);
    cos_beta      = cosf(beta_angle);
    float gamma_angle  = PIOVER180 * config->value(gamma_relative_angle_checksum)->by_default(240.0f)->as_number();
    sin_gamma     = sinf(gamma_angle);
    cos_gamma     = cosf(gamma_angle);

    // arm_length is the length of the arm from hinge to hinge
    arm_length         = config->value(arm_length_checksum)->by_default(250.0f)->as_number();
    // arm_radius is the horizontal distance from hinge to hinge when the effector is centered
    arm_radius         = config->value(arm_radius_checksum)->by_default(124.0f)->as_number();

    arm_length_correction[0] = config->value(arm_length_correction_a_checksum)->by_default(0.0f)->as_number();
    arm_length_correction[1] = config->value(arm_length_correction_b_checksum)->by_default(0.0f)->as_number();
    arm_length_correction[2] = config->value(arm_length_correction_c_checksum)->by_default(0.0f)->as_number();

    arm_radius_correction[0] = config->value(arm_radius_correction_a_checksum)->by_default(0.0f)->as_number();
    arm_radius_correction[1] = config->value(arm_radius_correction_b_checksum)->by_default(0.0f)->as_number();
    arm_radius_correction[2] = config->value(arm_radius_correction_c_checksum)->by_default(0.0f)->as_number();

    init();
}

void RostockSolution::init()
{
    arm_length_squared[0] = powf(arm_length + arm_length_correction[0], 2);
    arm_length_squared[1] = powf(arm_length + arm_length_correction[1], 2);
    arm_length_squared[2] = powf(arm_length + arm_length_correction[2], 2);

    arm_radius_corrected[0] = arm_radius + arm_radius_correction[0];
    arm_radius_corrected[1] = arm_radius + arm_radius_correction[1];
    arm_radius_corrected[2] = arm_radius + arm_radius_correction[2];
}

void RostockSolution::cartesian_to_actuator( float cartesian_mm[], float actuator_mm[] ){
    float alpha_rotated[3], rotated[3];
    
    if( sin_alpha == 0 && cos_alpha == 1){
        alpha_rotated[X_AXIS] = cartesian_mm[X_AXIS];
        alpha_rotated[Y_AXIS] = cartesian_mm[Y_AXIS];
        alpha_rotated[Z_AXIS] = cartesian_mm[Z_AXIS];
    }else{
        rotate( cartesian_mm, alpha_rotated, sin_alpha, cos_alpha );
    }
    actuator_mm[ALPHA_STEPPER] = solve_arm( 0, alpha_rotated );

    rotate( alpha_rotated, rotated, sin_beta, cos_beta );
    actuator_mm[BETA_STEPPER ] = solve_arm( 1, rotated );

    rotate( alpha_rotated, rotated, sin_gamma, cos_gamma );
    actuator_mm[GAMMA_STEPPER] = solve_arm( 2, rotated );
}

void RostockSolution::actuator_to_cartesian( float actuator_mm[], float cartesian_mm[] ){
    // unimplemented
}

float RostockSolution::solve_arm(int tower, float cartesian_mm[]) {
    return sqrtf(arm_length_squared[tower] - powf(cartesian_mm[X_AXIS] - arm_radius_corrected[tower], 2) - powf(cartesian_mm[Y_AXIS], 2)) + cartesian_mm[Z_AXIS];
}

void RostockSolution::rotate(float in[], float out[], float sin, float cos ){
    out[X_AXIS] = cos * in[X_AXIS] - sin * in[Y_AXIS];
    out[Y_AXIS] = sin * in[X_AXIS] + cos * in[Y_AXIS];
    out[Z_AXIS] = in[Z_AXIS];
}

bool RostockSolution::set_optional(char parameter, float value) {

    switch(parameter) {
        case 'L': // sets arm_length
            arm_length = value;
            break;
        case 'R': // sets arm_radius
            arm_radius = value;
            break;
        case 'A':
            arm_length_correction[0] = value;
            break;
        case 'B':
            arm_length_correction[1] = value;
            break;
        case 'C':
            arm_length_correction[2] = value;
            break;
        case 'D':
            arm_radius_correction[0] = value;
            break;
        case 'E':
            arm_radius_correction[1] = value;
            break;
        case 'F':
            arm_radius_correction[2] = value;
            break;
        default:
            return false;
    }
    init();
    return true;
}

bool RostockSolution::get_optional(char parameter, float *value) {
    if(value == NULL) return false;

    switch(parameter) {
        case 'L': // get arm_length
            *value = this->arm_length;
            break;
        case 'R': // get arm_radius
            *value = this->arm_radius;
            break;
        case 'A':
            *value = this->arm_length_correction[0];
            break;
        case 'B':
            *value = this->arm_length_correction[1];
            break;
        case 'C':
            *value = this->arm_length_correction[2];
            break;
        case 'D':
            *value = this->arm_radius_correction[0];
            break;
        case 'E':
            *value = this->arm_radius_correction[1];
            break;
        case 'F':
            *value = this->arm_radius_correction[2];
            break;
        default:
            return false;
    }

    return true;
}
