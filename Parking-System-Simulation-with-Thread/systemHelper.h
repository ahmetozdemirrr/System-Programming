#ifndef SYSTEM_HELPER_H
#define SYSTEM_HELPER_H

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TOTAL_VEHICLES 30
#define NUM_PICK 4
#define NUM_AUTO 8
#define FALSE 0
#define TRUE 1

extern sem_t newPickup;
extern sem_t inChargeForPickup;
extern sem_t newAutomobile;
extern sem_t inChargeForAutomobile;

extern int mFree_automobile;
extern int mFree_pickup;
extern int running;
extern int vehicleCount;

extern pthread_mutex_t lock;

typedef enum 
{ 
	AUTOMOBILE, 
	PICKUP 
} 
VehicleType;

int randomGenerator(void);
void initialize();
void clean();
void * carOwner(void *);
void * carAttendant(void *);

#endif /* SYSTEM_HELPER_H */
