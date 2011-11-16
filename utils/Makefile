CFLAGS	+=


UTILS= 	bitreverse	\
	chksum		\
	2byteswap	\
	4byteswap	\
	seeker

all: $(UTILS)

bitreverse: bitreverse.c

chksum: chksum.c

2byteswap: 2byteswap.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

4byteswap: 4byteswap.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

seeker: seeker.c

clean:
	rm -f *.o $(UTILS)
