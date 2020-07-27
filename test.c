#include <stdio.h>
#include <inttypes.h>
#include "./src/diskhash.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

uint64_t GetTimeNsec()
{
    struct timespec nowtime;
    clock_gettime(CLOCK_REALTIME, &nowtime);
    return nowtime.tv_sec * 1000000000 + nowtime.tv_nsec;
}

int main(void) {
    HashTableOpts opts;
    opts.key_maxlen = 15;
    opts.object_datalen = sizeof(uint64_t);
    char* err = NULL;
    HashTable* ht = dht_open("testing.dht", opts, O_RDWR|O_CREAT, &err);
    //HashTable* ht = dht_open("/mnt/pmem/zwh_test/testing.dht", opts, O_RDWR|O_CREAT, &err);
    if (!ht) {
        if (!err) err = "Unknown error";
        fprintf(stderr, "Failed opening hash table: %s.\n", err);
        return 1;
    }
    if (ht->is_pmem_) {
        printf("Working on pmem...\n");
    } else {
        printf("Working on disk...\n");
    }
    char key[16];
    key[15] = '\0';
    uint64_t entry_to_put = 100*1024*1024;
    printf("Begin to put\n");
    fflush(stdout);
    uint64_t start_time = GetTimeNsec();
    uint64_t t_key;
    for (uint64_t i = 0; i < entry_to_put; i++) {
        t_key = ~i;
        memcpy(key, &t_key, sizeof(uint64_t));
        dht_insert(ht, key, &i, NULL);
    }
    uint64_t time_span = GetTimeNsec()-start_time;
    printf("Put Ops %lf\n", ((double)entry_to_put*1000000000.0)/time_span);
    printf("Begin to get\n");
    fflush(stdout);
    //long i = 9;
    //dht_insert(ht, "key", &i, NULL);
    uint64_t *value;
    start_time = GetTimeNsec();
    for (uint64_t i = 0; i < entry_to_put; i++) {
        t_key = ~i;
        memcpy(key, &t_key, sizeof(uint64_t));
        value = (uint64_t *)dht_lookup(ht, key);
        /*
        if (*value != (uint64_t)*(uint64_t *)key) {
            printf("Wrong value %ld for key %ld\n", *value, *(uint64_t *)key);
        }
        */
    }
    time_span = GetTimeNsec()-start_time;
    printf("Get Ops %lf, get lat %lfns\n", ((double)entry_to_put*1000000000.0)/time_span, time_span/((double)entry_to_put));
    //long* val = (long*) dht_lookup(ht, "key");
    //printf("Looked up value: %ld\n", *val);

    dht_free(ht);
    return 0;
}
