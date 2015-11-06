#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "logging.h"

extern void systemDrawScreen (void);
extern bool systemReadJoypads (void);
extern uint32_t systemGetClock (void);
//extern void systemMessage(const char *, ...);
#define systemMessage(args...)		LOGI(args)
#ifdef USE_MOTION_SENSOR
extern void systemUpdateMotionSensor (void);
extern int  systemGetSensorX (void);
extern int  systemGetSensorY (void);
#endif

// sound functions
extern void systemOnWriteDataToSoundBuffer(int16_t * finalWave, int length);
#endif // SYSTEM_H
