#include <bcm2835.h>
#include <pthread.h>
#include "lights.h"
//#include "lua-config.h"

#define NUM_KEYS 12

extern char keybow_home_value[ 1024];
#ifndef KEYBOW_HOME
#define KEYBOW_HOME keybow_home_value
#endif

pthread_mutex_t lights_mutex;

unsigned short last_state[NUM_KEYS];

int lights_auto;

typedef struct keybow_key {
    unsigned short gpio_bcm;
    unsigned short hid_code;
    unsigned short led_index;
} keybow_key;

unsigned short mapping_table[36];

void *run_lights(void *void_ptr);
keybow_key get_key(unsigned short index);
int initUSB();
int initGPIO();
int main();
typedef struct _keybow_thread {
  int       mCreated;
  int       mStop;
  pthread_t mThread;
} keybow_thread;

extern keybow_thread t_run_lights;
void stopLights();

