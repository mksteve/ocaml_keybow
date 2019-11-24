#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/misc.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <stdio.h>
#include <bcm2835.h>
#include "keybow.h"
#include "lights.h"

int key_index = 0;
void add_key(unsigned short gpio_bcm, unsigned short hid_code, unsigned short led_index);

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
    initLights();
    return 0;
}



CAMLprim value keybow_display( value led_values, value num_leds )
{
  CAMLparam2( led_values, num_leds );

  for( size_t i = 0; i < (size_t)Int_val(num_leds); i++ ){
    value led_x = Field( led_values, i );
    value r = Field( led_x, 0 );
    value g = Field( led_x, 1 );
    value b = Field( led_x, 2 );
    lights_setPixel(i, Int_val(r), Int_val(g), Int_val(b) );
    //    printf( " RGB %d = (%d,%d,%d)\n",i, Int_val(r), Int_val(g), Int_val(b) );
    lights_show();
    
  }
  CAMLreturn( Val_unit );
}
CAMLprim value keybow_init( value un )
{
  CAMLparam1( un );
  initGPIO();
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


  
  CAMLreturn( Val_unit );
}
CAMLprim value keybow_term( value un)
{
  CAMLparam1( un );
  bcm2835_close();
  lights_cleanup();
  CAMLreturn( Val_unit );  
}

void add_key(unsigned short gpio_bcm, unsigned short hid_code, unsigned short led_index){
    mapping_table[(key_index * 3) + 0] = gpio_bcm;
    mapping_table[(key_index * 3) + 1] = hid_code;
    mapping_table[(key_index * 3) + 2] = led_index;
    key_index+=1;
}


CAMLprim value keybow_keypress()
{
  CAMLparam0();
  int x = 0;
  while( 1 ){
    for(x = 0; x < NUM_KEYS; x++){
      keybow_key key = get_key(x);
      int state = bcm2835_gpio_lev(key.gpio_bcm) == 0;
      if(state != last_state[x]){
	last_state[x] = state;
	if( state == 1 ) {
	  CAMLreturn( Val_int(key.led_index) );
	}
      }
      last_state[x] = state;
    }
    usleep( 100000 );
  }
  CAMLreturn ( Val_unit);
}
