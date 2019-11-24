#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/misc.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <stdio.h>

CAMLprim value keybow_display( value led_values, value num_leds )
{
  CAMLparam2( led_values, num_leds );

  for( size_t i = 0; i < (size_t)Val_int(num_leds); i++ ){
    value led_x = Field( led_values, i );
    value r = Field( led_x, 0 );
    value g = Field( led_x, 1 );
    value b = Field( led_x, 2 );
    printf( " RGB %d = (%ld,%ld,%ld)\n",i, Val_int(r), Val_int(g), Val_int(b) );
    
  }
  CAMLreturn( Val_unit );
}
