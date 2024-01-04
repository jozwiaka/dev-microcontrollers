
/*
 * Include Files
 *
 */
#if defined(MATLAB_MEX_FILE)
#include "tmwtypes.h"
#include "simstruc_types.h"
#else
#include "rtwtypes.h"
#endif



/* %%%-SFUNWIZ_wrapper_includes_Changes_BEGIN --- EDIT HERE TO _END */
#include <math.h>
    float Tp, kp, ki, kd;
    float Smin, Smax;
    float sum, inPrev;
    float in, out;
/* %%%-SFUNWIZ_wrapper_includes_Changes_END --- EDIT HERE TO _BEGIN */
#define u_width 1
#define y_width 1

/*
 * Create external references here.  
 *
 */
/* %%%-SFUNWIZ_wrapper_externs_Changes_BEGIN --- EDIT HERE TO _END */
/* extern double func(double a); */
/* %%%-SFUNWIZ_wrapper_externs_Changes_END --- EDIT HERE TO _BEGIN */

/*
 * Start function
 *
 */
void PID_Discrete_Start_wrapper(const real_T *p_Tp, const int_T p_width0,
			const real_T *p_kp, const int_T p_width1,
			const real_T *p_Ti, const int_T p_width2,
			const real_T *p_Td, const int_T p_width3,
			const real_T *p_Smin, const int_T p_width4,
			const real_T *p_Smax, const int_T p_width5)
{
/* %%%-SFUNWIZ_wrapper_Start_Changes_BEGIN --- EDIT HERE TO _END */
/*
 * Custom Start code goes here.
 */
    in = 0;
    out = 0;
    Tp = *p_Tp;
    kp = *p_kp;
    ki = *p_kp * *p_Tp / *p_Ti;
    kd = *p_kp * *p_Td / *p_Tp;
    Smin = *p_Smin;
    Smax = *p_Smax;
    sum=0;
/* %%%-SFUNWIZ_wrapper_Start_Changes_END --- EDIT HERE TO _BEGIN */
}
/*
 * Output function
 *
 */
void PID_Discrete_Outputs_wrapper(const real_T *e,
			real_T *u,
			const real_T *p_Tp, const int_T p_width0,
			const real_T *p_kp, const int_T p_width1,
			const real_T *p_Ti, const int_T p_width2,
			const real_T *p_Td, const int_T p_width3,
			const real_T *p_Smin, const int_T p_width4,
			const real_T *p_Smax, const int_T p_width5)
{
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_BEGIN --- EDIT HERE TO _END */
in = e[0]; 
    sum += in * ki;
    if (sum > Smax)
        sum = Smax;
    if (sum < Smin)
        sum = Smin;
    out = sum + in * kp + (in-inPrev) * kd;
      u[0] = out;
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_END --- EDIT HERE TO _BEGIN */
}


