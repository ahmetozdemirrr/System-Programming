#include "systemHelper.h"

sem_t newPickup;
sem_t inChargeForPickup;
sem_t newAutomobile;
sem_t inChargeForAutomobile;

int mFree_automobile = NUM_AUTO;
int mFree_pickup = NUM_PICK;
int vehicleCount = 0;
int running = TRUE;

pthread_mutex_t lock;

int randomGenerator(void)
{
    int ramNumber = rand() % 2;
    return ramNumber;
}

void initialize()
{
    sem_init(&newPickup, 0, 0);
    sem_init(&inChargeForPickup, 0, 0);
    sem_init(&newAutomobile, 0, 0);
    sem_init(&inChargeForAutomobile, 0, 0);

    pthread_mutex_init(&lock, NULL);
}

void clean()
{
    pthread_mutex_destroy(&lock);
    sem_destroy(&newPickup);
    sem_destroy(&inChargeForPickup);
    sem_destroy(&newAutomobile);
    sem_destroy(&inChargeForAutomobile);
}

void * carOwner(void * arg) 
{
    while (TRUE) 
    {
        pthread_mutex_lock(&lock);
        if (!running) 
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);

        int carType = randomGenerator(); /* 0: Otomobil, 1: Pickup */

        if (carType == 0) 
        {
            /*-------- critical region start --------*/

            pthread_mutex_lock(&lock);

            if (mFree_automobile > 0) 
            {
                mFree_automobile--;
                printf("+ Automobile parked. Available empty automobile spaces: %d pickup spaces: %d\n", mFree_automobile, mFree_pickup);
                sem_post(&inChargeForAutomobile);
            } 

            else 
            {
                printf("x There is no empty space for automobile.\n");
            }
            pthread_mutex_unlock(&lock);

            /*-------- critical region end --------*/
        } 

        else if (carType == 1)
        {
            /*-------- critical region start --------*/

            pthread_mutex_lock(&lock);

            if (mFree_pickup > 0) 
            {
                mFree_pickup--;
                printf("+ Pickup parked. Available empty automobile spaces: %d pickup spaces: %d\n", mFree_automobile, mFree_pickup);
                sem_post(&inChargeForPickup);
            } 

            else 
            {
                printf("x There is no empty space for pickup.\n");
            }
            pthread_mutex_unlock(&lock);

            /*-------- critical region end --------*/
        }

        /*-------- critical region start --------*/

        pthread_mutex_lock(&lock);

        vehicleCount++; /* Araç sayacını artır */

        if (vehicleCount >= TOTAL_VEHICLES) /* TOTAL_VEHICLES kadar araç simüle ediyoruz */
        {
            printf("* Terminating the programme...\n");
            running = FALSE;
            pthread_mutex_unlock(&lock);
            exit(EXIT_SUCCESS);
        }

        pthread_mutex_unlock(&lock);

        /*-------- critical region end --------*/

        usleep(500000); /* Araçların gelişi simule edilir (yarım saniye)*/
    }
    return NULL;
}

void * carAttendant(void * arg) 
{
	VehicleType type = *(VehicleType *)arg;

    while (TRUE) 
    {
        pthread_mutex_lock(&lock);
        if (!running) 
        {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);

    	if (type == PICKUP)
    	{
	    	sem_wait(&inChargeForPickup);
	        sleep(3); /* Pickup otoparkta 3 saniye bekler */

	        /*-------- critical region start --------*/

	        pthread_mutex_lock(&lock);

	        if (mFree_pickup < NUM_PICK) 
	        {
	            mFree_pickup++;
	            printf("- The pickup has been removed. Number of available free pickup parking spaces: %d\n", mFree_pickup);
	        }
	        pthread_mutex_unlock(&lock);

	        /*-------- critical region end --------*/
    	}

    	else if (type == AUTOMOBILE)
    	{
	        sem_wait(&inChargeForAutomobile);
	        sleep(3); /* Otomobil otoparkta 3 saniye bekler */

	        /*-------- critical region start --------*/

	        pthread_mutex_lock(&lock);

	        if (mFree_automobile < NUM_AUTO) 
	        {
	            mFree_automobile++;
	            printf("- The automobile has been removed. Number of available free automobile parking spaces: %d\n", mFree_automobile);
	        }
	        pthread_mutex_unlock(&lock);
	        
	        /*-------- critical region end --------*/
    	}        
    }
    return NULL;
}
