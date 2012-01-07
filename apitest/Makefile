include ../Make.defines.in

TRICKS	= anykey bar 
APITEST	= crypt38 getaddrinfo gethostbyname getopt_long ipaddr selectin \
	  stdio wait mq_test

TARGETS = $(TRICKS) $(APITEST)

all: $(TARGETS)

clean:
	rm -f $(TARGETS) $(CLEANFILES)
