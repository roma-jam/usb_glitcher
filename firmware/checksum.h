/*
 * checksum.h
 *
 *  Created on: 15 рту. 2018 у.
 *      Author: RLeonov
 */

#ifndef CHECKSUM_H_
#define CHECKSUM_H_


#include "app.h"

extern unsigned int __ETEXT[];

void checksum_init(APP* app);


#endif /* CHECKSUM_H_ */
