#include "ch32v00x_conf.h"
/**
 * @brief Remember to init millis, otherwise it wont work,
 * This library uses TIM2 to determine current milliseconds !
 * @param periodCorrection Period correction, to grow accuracy of millis.
 */
void Millis_Init(int16_t periodCorrection);
uint64_t millis();
uint64_t seconds();