#include <assert.h>
#include <time.h>

#include "frame_difference.h"

#define NUMBER_OF_FRAMES 3

static inline int abs_uint(const int i)
{
  return (i >= 0) ? i : -i;
}

struct vibeFrameDifference
{
  /* Parameters. */
  uint32_t width;
  uint32_t height;
  uint32_t frameDifferenceThreshold;
  uint32_t numberOfFramesStacked;

  /* Storage for the model. */
  uint8_t *imageBuffer;
  uint32_t *min_val;
  uint32_t *max_val;

};

// -----------------------------------------------------------------------------
// Creates the data structure
// -----------------------------------------------------------------------------
vibeFrameDifference_t *vibeFrameDifference_New()
{
  /* Model structure alloc. */
  vibeFrameDifference_t *fDmodel = NULL;
  fDmodel = (vibeFrameDifference_t*)calloc(1, sizeof(*fDmodel));
  assert(fDmodel != NULL);

  /* Default parameters values. */
  fDmodel->frameDifferenceThreshold         = 0;

  /* Storage for the model. */
  fDmodel->imageBuffer            = NULL;
  fDmodel->min_val                = NULL;
  fDmodel->max_val                = NULL;

  return(fDmodel);
}

// -----------------------------------------------------------------------------
// Some "Get-ers"
// -----------------------------------------------------------------------------
uint32_t vibeFrameDifference_GetFrameDifferenceThreshold(const vibeFrameDifference_t *fDmodel)
{
  assert(fDmodel != NULL);
  return(fDmodel->frameDifferenceThreshold);
}

// -----------------------------------------------------------------------------
// Some "Set-ers"
// -----------------------------------------------------------------------------
int32_t vibeFrameDifference_SetFrameDifferenceThreshold(
  vibeFrameDifference_t *fDmodel,
  const uint32_t frameDifferenceThreshold
) {
  assert(fDmodel != NULL);
  assert(frameDifferenceThreshold >= 0);

  fDmodel->frameDifferenceThreshold = frameDifferenceThreshold;

  return(0);
}

// ----------------------------------------------------------------------------
// Frees the structure
// ----------------------------------------------------------------------------
int32_t vibeFrameDifference_Free(vibeFrameDifference_t *fDmodel)
{
  if (fDmodel == NULL)
    return(-1);

  free(fDmodel->imageBuffer);
  free(fDmodel);

  return(0);
}

// -----------------------------------------------------------------------------
// Initializes frame differencing structure and adds image
// -----------------------------------------------------------------------------
int32_t vibeFrameDifference_Init(
  vibeFrameDifference_t *fDmodel,
  const uint8_t *image_data,
  const uint32_t width,
  const uint32_t height
) {
  /* Some basic checks. */
  assert((image_data != NULL) && (fDmodel != NULL));
  assert((width > 0) && (height > 0));

  /* Finish model alloc - parameters values cannot be changed anymore. */
  fDmodel->width = width;
  fDmodel->height = height;

  /* Creates the historyImage structure. */
  fDmodel->imageBuffer = NULL;

  /* Creates the history buffer. */
  fDmodel->imageBuffer = (uint8_t *)malloc(width * height * NUMBER_OF_FRAMES * sizeof(uint8_t));
  assert(fDmodel->imageBuffer != NULL);

  /* Fills the history buffer */
  for (int index = 0; index <  width * height; index++) {
    uint8_t mean_value = (image_data[3*index] + image_data[3*index + 1] + image_data[3*index + 2]) / 3;

    for (int x = 0; x < NUMBER_OF_FRAMES; ++x) {

      fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + x] = mean_value;
    }
  }

  fDmodel->min_val = (uint32_t*)malloc(sizeof(*(fDmodel->min_val)));
  fDmodel->max_val = (uint32_t*)malloc(sizeof(*(fDmodel->max_val)));

  return(0);
}

// ----------------------------------------------------------------------------
// Add an image into the buffer
// ----------------------------------------------------------------------------
int32_t vibeFrameDifference_Add_Frame(
  vibeFrameDifference_t *fDmodel,
  const uint8_t *image_data
) {
  /* Basic checks. */
  assert((fDmodel->width > 0) && (fDmodel->height > 0));
  assert(fDmodel->imageBuffer != NULL);


  /* Some variables. */
  uint32_t width = fDmodel->width;
  uint32_t height = fDmodel->height;

  uint8_t *imageBuffer = fDmodel->imageBuffer;
  *fDmodel->min_val = 255;
  *fDmodel->max_val = 0;

  /* Move old image values and introduce new image value */
  for (int index = 0; index <  width * height; index++) {
    uint8_t mean_value = (image_data[3*index] + image_data[3*index + 1] + image_data[3*index + 2]) / 3;

    if(mean_value < *fDmodel->min_val){
      *fDmodel->min_val = mean_value;
    }
    if(mean_value > *fDmodel->max_val){
      *fDmodel->max_val = mean_value;
    }

    for (int x = 0; x < NUMBER_OF_FRAMES - 1; ++x) {

      fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + x] = fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + (x + 1)];
    }

    /* Write new image data into the buffer*/
    fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + (NUMBER_OF_FRAMES - 1)] = mean_value;
  }

  return(0);
}


// -----------------------------------------------------------------------------
// Apply frame difference and generate mask
// -----------------------------------------------------------------------------
int32_t vibeFrameDifference_ComputeFrameDifference(
  vibeFrameDifference_t *fDmodel,
  uint8_t *segmentation_map,
  uint8_t *frame_difference_map
) {
  /* Basic checks. */
  assert((fDmodel != NULL) && (frame_difference_map != NULL));
  assert((fDmodel->width > 0) && (fDmodel->height > 0));
  assert((fDmodel->max_val != NULL) && (fDmodel->min_val != NULL));
  assert(fDmodel->imageBuffer != NULL);

  /* Some variables. */
  uint32_t width = fDmodel->width;
  uint32_t height = fDmodel->height;
  uint32_t imageBuffer = *fDmodel->imageBuffer;
  float thr, thr_f, thr_b, mean_val;
  int N_b = 0, N_f = 0, count_b = 0, count_f = 0;

  /* Compute threshold automatically. */
  memset(frame_difference_map, 0, width * height);
  thr = (*fDmodel->max_val + *fDmodel->min_val) / 2;
  
  for (int index = 0; index <  width * height; index++) {
    // Take mean of the three images at each position
    uint8_t value_im1 = fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + 0];
    uint8_t value_im2 = fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + 1];
    uint8_t value_im3 = fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + 2];
    mean_val = (value_im1 + value_im2 + value_im1) / 3;   

    /* Count and add the positions clustered to foreground and background 
    according to the initial threshold */ 
    if (mean_val >= thr){
      frame_difference_map[index] = 1;
      N_f = N_f + 1;
      count_f = count_f + mean_val;
    } else {
      count_b = count_b + mean_val;
    }
  } // for
  // Compute final threshold
  N_b = (width * height) - N_f;
  thr_f = count_f / N_f;
  thr_b = count_b / N_b;
  thr = (thr_f + thr_b) / 100;

  memset(frame_difference_map, 0, width * height);
  
  // Compute frame difference D = |img3 - img2| * |img2 - img1| and generate frame difference mask
  for (int index = 0; index <  width * height; index++) {

    uint8_t value_im1 = fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + 0];
    uint8_t value_im2 = fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + 1];
    uint8_t value_im3 = fDmodel->imageBuffer[index * NUMBER_OF_FRAMES + 2];

    /* Compute frame difference and set the corresponding value:
        - 1 if frame difference >= threshold
        - 0 if frame difference < threshold
    */
    if (abs(value_im3 - value_im2) * abs(value_im2 - value_im1) < thr){
      segmentation_map[index] = 0;
      
    } // if
  } // for

  return(0);
}