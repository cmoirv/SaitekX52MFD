//#############################################################################
// Deklaration für das ganze Projekt
//#############################################################################
#pragma once

#include "projectCommon.h"
#include <utility>

enum saColor { SA_OFF,SA_RED,SA_GREEN,SA_AMBER };                    // Farben der LED
enum saBlink { SA_NOBLINK, SA_BLINK };                               // Blinkend oder nicht
// Bezeichnungen der LED
enum saLED { NONE_LED = 0, FIRE_A, FIRE_B, FIRE_D, FIRE_E, T1, T2, T3, POV2, CLUTCH, FIRE_BUTTON, THROTTLE_AXIS };
enum saSigStat {SA_OK1, SA_TRANS, SA_OK2, SA_FAILED };               // Zustände für Special-LED
typedef std::pair<saColor,saBlink> saLedStat;                        // Speicher für LED
