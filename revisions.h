/*
* revisions.h
*
* Created: 4/23/2015 2:17:39 PM
*  Author: Ashley M. Kirchner
*/

#ifndef REVISIONS_H_
#define REVISIONS_H_

/* 2014-04
* Initial version 1
*/

/* 2015-04-23
* Changed 'fileStatus' variable from a uint32_t to uint16_t after reading FatFile.h from the SdFat library
* Created 'resetString(int state)' function and changed all running code to call that function instead of explicitly using memset() with multiple LEDS.show() calls
* Created initial 'revisions.h' file.
*/

/* 2015-05-11
* Initial attempt at GitHub
*/

/* 2015-05-18
* Removal of battery gauge control into separate header
* with the goal of selectively adding it in or not
*/

#endif /* REVISIONS_H_ */