CFLAGS_ALL=-I../libusbgx/build/include -I../keybow-firmware/bcm2835-1.58/src -L../bcm2835-1.58/build/lib -I../lua-5.3.5/src -L../libusbgx/build/lib -L../libserialport/build/lib -L../lua-5.3.5/src -lpthread -lm -lbcm2835 -ldl

OCAML_FLAGS=-I ../keybow-firmware/bcm2835-1.58/src
ocaml_bind : CFLAGS+= -g
ocaml_bind : ocaml_bind.c 
	ocamlc  $(OCAML_FLAGS)  $^


luatest: CFLAGS+=-static -I../lua-5.3.5/src -L../lua-5.3.5/src -llua -lm -ldl
luatest: luatest.c
	$(CC) $^ $(CFLAGS) -o $@


lightstest: CFLAGS+=-L../bcm2835-1.58/src -lpng -lbcm2835 -lserialport
lightstest: lightstest.c lights.c
	$(CC) $^ $(CFLAGS) -o $@

lightstest2: CFLAGS+=-I../bcm2835-1.58/build/include -L../bcm2835-1.58/build/lib -lbcm2835 -lpng -g
lightstest2: lightstest2.c lights.c
	$(CC) $^ $(CFLAGS) -o $@


keybow-test: CFLAGS+=-static -Wall -DKEYBOW_DEBUG -DKEYBOW_NO_USB_HID -DKEYBOW_HOME='"../sdcard"' -DKEYBOW_SERIAL='"/dev/tnt0"' $(CFLAGS_ALL)
keybow-test: keybow.c lights.c lua-config.c serial.c
	$(CC) $^ $(CFLAGS) -o $@

keybow-usbtest: CFLAGS+=-static -Wall -DKEYBOW_DEBUG -DKEYBOW_HOME='"../sdcard"' $(CFLAGS_ALL) -lusbgx -lconfig
keybow-usbtest: keybow.c lights.c lua-config.c gadget-hid.c serial.c
	$(CC) $^ $(CFLAGS) -o $@

clean:
	-rm keybow-test
	-rm keybow-usbtest
	-rm keybow
	-rm luatest
	-rm lightstest
