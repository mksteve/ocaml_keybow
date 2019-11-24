#include "keybow.h"

#include "serial.h"

#ifndef KEYBOW_NO_USB_HID
#include "gadget-hid.h"
#endif

#include "lights.h"
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <bcm2835.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <unistd.h>

int hid_output;
int midi_output;
int running = 0;
int key_index = 0;

keybow_thread t_run_lights = {0};

void signal_handler(int dummy) {
    running = 0;
}

keybow_key get_key(unsigned short index){
    keybow_key key;
    index *= 3;
    key.gpio_bcm = mapping_table[index + 0];
    key.hid_code = mapping_table[index + 1];
    key.led_index = mapping_table[index + 2];
    return key;
}

void add_key(unsigned short gpio_bcm, unsigned short hid_code, unsigned short led_index){
    mapping_table[(key_index * 3) + 0] = gpio_bcm;
    mapping_table[(key_index * 3) + 1] = hid_code;
    mapping_table[(key_index * 3) + 2] = led_index;
    key_index+=1;
}

int initGPIO() {
    if (!bcm2835_init()){
        return 1;
    }
    int x = 0;
    for(x = 0; x < NUM_KEYS; x++){
        keybow_key key = get_key(x);
        bcm2835_gpio_fsel(key.gpio_bcm, BCM2835_GPIO_FSEL_INPT);
        bcm2835_gpio_set_pud(key.gpio_bcm, BCM2835_GPIO_PUD_UP);
    }
    return 0;
}

int updateKeys() {
    int x = 0;
    //unsigned short buf[HID_REPORT_SIZE];
    //for(x = 0; x < HID_REPORT_SIZE; x++){
    //    buf[x] = 0;
    //}
    //int buf_index = 2; // Skip modifier and padding bytes
    for(x = 0; x < NUM_KEYS; x++){
        keybow_key key = get_key(x);
        int state = bcm2835_gpio_lev(key.gpio_bcm) == 0;
        if(state != last_state[x]){
            luaHandleKey(x, state);            
        }
        last_state[x] = state;
        //if(state){
        //    buf[buf_index] = key.hid_code;
        //    buf_index++;
        //}
    }
    /*for(x = 0; x < HID_REPORT_SIZE; x++){
        printf("0x%02x ", buf[x]);
    }
    printf("\n");*/
    //write(hid_output, buf, HID_REPORT_SIZE);
    return 0;
}

void *run_lights(void *void_ptr){
    while(running && t_run_lights.mStop == 0){
	if( height ){
	    int delta = (millis() / (1000/60)) % height;
	    if (lights_auto) {
		pthread_mutex_lock( &lights_mutex );
		lights_drawPngFrame(delta);
		pthread_mutex_unlock( &lights_mutex );
	    }
	    lights_show();
	}
        usleep(16666); // About 60fps
    }
    return NULL;
}

void stopLights()
{
    if( t_run_lights.mCreated ){
	t_run_lights.mStop = 1;
	pthread_join( t_run_lights.mThread, NULL );
	t_run_lights.mCreated = 0;
    }
    t_run_lights.mStop    = 0;

}
char keybow_home_value[ 1024] = "/boot/";
char keybow_script[1024] = "keys.lua";

int main( int argc, char * argv[] ) {
    int ret;
    int i;
    for( i = 1; i < argc; i++){
      if( strcmp( argv[i], "--home" ) == 0 && i < argc -1 ){
	strcpy( keybow_home_value, argv[i+1]);
	i++;
	continue;
      }
      if( strcmp( argv[i], "--script" ) == 0 && i < argc -1 ){
	strcpy( keybow_script, argv[i+1]);
	i++;
	continue;
      }
    }
    chdir(KEYBOW_HOME);

    pthread_mutex_init ( &lights_mutex, NULL );

    add_key(RPI_V2_GPIO_P1_11, 0x27, 3);
    add_key(RPI_V2_GPIO_P1_13, 0x37, 7);
    add_key(RPI_V2_GPIO_P1_16, 0x28, 11);
    add_key(RPI_V2_GPIO_P1_15, 0x1e, 2);
    add_key(RPI_V2_GPIO_P1_18, 0x1f, 6);
    add_key(RPI_V2_GPIO_P1_29, 0x20, 10);
    add_key(RPI_V2_GPIO_P1_31, 0x21, 1);
    add_key(RPI_V2_GPIO_P1_32, 0x22, 5);
    add_key(RPI_V2_GPIO_P1_33, 0x23, 9);
    add_key(RPI_V2_GPIO_P1_38, 0x24, 0);
    add_key(RPI_V2_GPIO_P1_36, 0x25, 4);
    add_key(RPI_V2_GPIO_P1_37, 0x26, 8);


    if (initGPIO() != 0) {
        return 1;
    }

#ifndef KEYBOW_NO_USB_HID
    ret = initUSB();
    //if (ret != 0 && ret != USBG_ERROR_EXIST) {
    //    return 1;
    //}

    do {
        hid_output = open("/dev/hidg0", O_WRONLY | O_NDELAY);
    } while (hid_output == -1 && errno == EINTR);
    if (hid_output == -1){
        printf("Error opening /dev/hidg0 for writing.\n");
        return 1;
    }

    do {
        midi_output = open("/dev/snd/midiC1D0", O_WRONLY | O_NDELAY);
    } while (midi_output == -1 && errno == EINTR);
    if (midi_output == -1){
        printf("Error opening /dev/snd/midiC1D0 for writing.\n");
        return 1;
    }
#else
    printf("Opening /dev/null for output.\n");
    hid_output = open("/dev/null", O_WRONLY);
    midi_output = open("/dev/null", O_WRONLY);
#endif

#ifdef KEYBOW_DEBUG
    printf("Initializing LUA\n");
#endif

    ret = initLUA(keybow_script);
    if (ret != 0){
        return ret;
    }

    int x = 0;
    for(x = 0; x < NUM_KEYS; x++){
        last_state[x] = 0;
    }

    lights_auto = 1;

#ifdef KEYBOW_DEBUG
    printf("Initializing Lights\n");
#endif
    initLights();
    read_png_file("default.png");

    serial_open();

    printf("Running...\n");
    running = 1;
    signal(SIGINT, signal_handler);

    luaCallSetup();

    while (running){
        /*int delta = (millis() / (1000/60)) % height;
        if (lights_auto) {
            lights_drawPngFrame(delta);
        }
        lights_show();*/
        luaTick();
        updateKeys();
        usleep(1000);
        //usleep(250000);
    }      

    stopLights();

    printf("Closing LUA\n");
    luaClose();
#ifndef KEYBOW_NO_USB_HID
    printf("Cleanup USB\n");
    cleanupUSB();
#endif
    printf("Cleanup BCM2835\n");
    bcm2835_close();

    return 0;
}
