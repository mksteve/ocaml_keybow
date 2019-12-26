OCAMLC = ocamlc
OCAMLOPT = ocamlopt
OCAMLDEP = ocamldep
#INCLUDES = -I ~/keybow-firmware/bcm2835-1.58/src/
OCAMLFLAGS = -custom $(INCLUDES)
TARGETS = Solver.cmo Notaktodef.cmo Keybow.cmo KeybowNotaktoGame.cmo
LIBS = libkeybow.a
KEYBOW_FILES = lights.o ocaml_bind.o keybow-firmware/bcm2835-1.58/src/bcm2835.o
KEYBOW_CCFLAGS=-I ./keybow-firmware/bcm2835-1.58/src/

.SUFFIXES: .ml .mli .cmo .cmi .cmx

.ml.cmo:
	$(OCAMLC) $(OCAMLFLAGS) -c $<

.mli.cmi:
	$(OCAMLC) $(OCAMLFLAGS) -c $<

.ml.cmx:
	echo "test"
	$(OCAMLOPT) $(OCAMLFLAGS) -c $<

.c.o:
	$(CC) $(KEYBOW_CCFLAGS) -c $< 
	
	
Notakto: $(TARGETS) libkeybow.a
	$(OCAMLC) -o Notakto $(OCAMLFLAGS) $^

make: $(FILE)
	$(OCAMLC) $(OCAMLFLAGS) $^

libkeybow.a : $(KEYBOW_FILES)
	ar rs libkeybow.a $(KEYBOW_FILES)

.depend:
	$(OCAMLDEP) $(INCLUDES) *.mli *.ml > .depend

include .depend
