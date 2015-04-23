TESTS = addsub hello var ptr lazy textrel regparm
all: $(addprefix test,$(TESTS))

.PRECIOUS: %.so

%.so: %.c hello.h
	gcc -m32 -shared -fPIC -o $@ $< -nostdlib -Wl,--hash-style=sysv

textrel.so: textrel.c hello.h
	gcc -m32 -shared -o $@ $< -nostdlib -Wl,--hash-style=sysv

test%: test%.c %.so
	gcc -m32 -o $@ $< libloader.so -Wl,-rpath,.

test: all $(addprefix call-,$(TESTS))

call-%:
	-./test$*

clean:
	rm -f *.o $(addprefix test,$(TESTS)) $(addsuffix .so,$(TESTS))
