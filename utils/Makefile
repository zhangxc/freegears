include ../Make.defines.in

TARGETS = bitreverse chksum 2byteswap 4byteswap seeker fft

all: $(TARGETS)

bitreverse: bitreverse.c

chksum: chksum.c

2byteswap: 2byteswap.cpp

4byteswap: 4byteswap.cpp

seeker: seeker.c

fft: fft.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBFLAGS)

clean:
	rm -f $(TARGETS) $(CLEANFILES)
