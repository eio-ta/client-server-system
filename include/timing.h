#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>


// STRUCTURE //

/* Structure time : trois paramètres pour les minutes, l'heure et le jour de la semaine */
typedef struct timing {
	uint64_t minutes;
	uint32_t hours;
	uint8_t daysofweek;
} timing;

#endif // TIMING_H
