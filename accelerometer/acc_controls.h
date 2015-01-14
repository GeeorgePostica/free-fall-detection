/* 
 * File:   acc_controls.h
 * Author: george
 *
 * Created on January 13, 2015, 11:46 PM
 */

#ifndef ACC_CONTROLS_H
#define	ACC_CONTROLS_H

#define ACC_G           9.8
#define ACC_FLOAT_2G    1/(2*ACC_G)
#define ACC_FLOAT_4G    1/(4*ACC_G)
#define ACC_FLOAT_6G    1/(6*ACC_G)
#define ACC_FLOAT_8G    1/(8*ACC_G)
#define ACC_FLOAT_16G   1/(16*ACC_G)

void vAccInit();
float fAccGetX();
float fAccGetY();
float fAccGetZ();
float* pfAccGetXYZ();
void vAccSoftReset();
void vAccReboot();



#endif	/* ACC_CONTROLS_H */

