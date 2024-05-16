#include "systemHelper.h"

int main(int argc, char const *argv[]) 
{
    VehicleType types[2] = {AUTOMOBILE, PICKUP};

    pthread_t owner;
    pthread_t attendant[2];

    initialize(); /* Kaynakları başlat */

    pthread_create(&owner, NULL, carOwner, NULL); /* aynı anda sadece bir araç girişini sağlamak için sadece bir tane owner yaratıyoruz */
    pthread_create(&attendant[0], NULL, carAttendant, &types[0]);
    pthread_create(&attendant[1], NULL, carAttendant, &types[1]);

    pthread_join(owner, NULL);

    pthread_mutex_lock(&lock);
    running = FALSE;
    pthread_mutex_unlock(&lock);

    sem_post(&inChargeForPickup);  /* pickup thread'inin çıkışını sağlamak için */
    sem_post(&inChargeForAutomobile); /* automobile thread'inin çıkışını sağlamak için */

    pthread_join(attendant[0], NULL);
    pthread_join(attendant[1], NULL);

    clean(); /* Kaynakları temizle */

    return EXIT_SUCCESS;
}
