// Copyright 2020 Oprea Olivia Maria-Magdalena 323CA <opreaolivia73@gmail.com>
// In colaborare cu echipa de pc care a facut laburile din care am luat codul :)

#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define BUFLEN		256	// dimensiunea maxima a calupului de date
#define MAX_CLIENTS	10	// numarul maxim de clienti in asteptare

#endif
