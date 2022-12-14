/*----------------------------------------------------------------------------

  Copyright (c) Xavier Bou Hernandez <xavibouhae@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  ----------------------------------------------------------------------------*/


#ifdef __cplusplus
extern "C"
{
#endif 

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define COLOR_BACKGROUND   0 /*!< Default label for background pixels */
#define COLOR_FOREGROUND 255 /*!< Default label for foreground pixels. Note that some authors chose any value different from 0 instead */

/**
 * \typedef struct vibeModel_Sequential_t
 * \brief Data structure for the background subtraction model.
 *
 * This data structure contains the background model as well as some paramaters value.
 * The code is designed to hide all the implementation details to the user to ease its use.
 */
typedef struct vibeFrameDifference vibeFrameDifference_t;

/**
 * Allocation of a new data structure where the background model will be stored.
 * Please note that this function only creates the structure to host the data.
 * This data structures will only be filled with a call to \ref libvibeModel_Sequential_AllocInit_8u_C1R.
 *
 * \result A pointer to a newly allocated \ref vibeModel_Sequential_t
 * structure, or <tt>NULL</tt> in the case of an error.
 */
vibeFrameDifference_t *vibeFrameDifference_New();

/**
 * Setter.
 *
 * @param model The data structure with ViBe's background subtraction model and parameters.
 * @param numberOfSamples
 * @return
 */
int32_t vibeFrameDifference_SetFrameDifferenceThreshold(
  vibeFrameDifference_t *fDmodel,
  const uint32_t frameDifferenceThreshold
);

/**
 * Getter.
 *
 * @param model The data structure with ViBe's background subtraction model and parameters.
 * @return
 */
uint32_t vibeFrameDifference_GetFrameDifferenceThreshold(const vibeFrameDifference_t *fDmodel);

/**
 * \brief Frees all the memory used by the <tt>model</tt> and deallocates the structure.
 *
 * This function frees all the memory allocated by \ref libvibeModel_SequentialNew and
 * \ref libvibeModel_Sequential_AllocInit_8u_C1R or \ref libvibeModel_Sequential_AllocInit_8u_C3R.
 * @param model The data structure with ViBe's background subtraction model and parameters.
 * @return
 */
int32_t vibeFrameDifference_Free(vibeFrameDifference_t *fDmodel);

/**
 * The two following functions allocate the required memory according to the
 * model parameters and the dimensions of the input images.
 * You must use the "C1R" function for grayscale images and the "C3R" for color
 * images. 
 * These 2 functions also initialize the background model using the content
 * of *image_data which is the pixel buffer of the first image of your stream.
 */
// -------------------------  Single channel images ----------------------------
/**
 *
 * @param model The data structure with ViBe's background subtraction model and parameters.
 * @param image_data
 * @param width
 * @param height
 * @return
 */
int32_t vibeFrameDifference_Init(
  vibeFrameDifference_t *fDmodel,
  const uint8_t *image_data,
  const uint32_t width,
  const uint32_t height
);

int32_t vibeFrameDifference_Add_Frame(
  vibeFrameDifference_t *fDmodel,
  const uint8_t *image_data
);

int32_t vibeFrameDifference_ComputeFrameDifference(
  vibeFrameDifference_t *fDmodel,
  uint8_t *segmentation_map,
  uint8_t *frame_difference_map
);
