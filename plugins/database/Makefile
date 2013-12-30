BASEDIRS = postgres
           

######################################################################
# Configuration should occur above this line

include ../../iRODS/config/platform.mk
include ../../iRODS/config/config.mk

GCC = g++ -Wno-write-strings -DRODS_SERVER 

ifeq ($(IRODS_BUILD_COVERAGE), 1)
GCC += -fprofile-arcs -ftest-coverage -lgcov
endif

export OBJDIR = .objs
export DEPDIR = .deps
export IRODSTOPDIR = ../../iRODS
export SOTOPDIR = .

SUBS = ${BASEDIRS}

ifeq ($(IRODS_BUILD_DEBUG), 0)
#SUBS += ${HPSSDIRS}
endif

.PHONY: ${SUBS} clean

default: ${SUBS}

${SUBS}:
	@-mkdir -p $@/${OBJDIR} > /dev/null 2>&1
	@-mkdir -p $@/${DEPDIR} > /dev/null 2>&1
	${MAKE} -C $@

clean:
	@-for dir in ${SUBS}; do \
	echo "Cleaning $$dir"; \
	rm -f $$dir/${OBJDIR}/*.o > /dev/null 2>&1; \
        rm -f $$dir/${DEPDIR}/*.d > /dev/null 2>&1; \
        rm -f $$dir/.depends > /dev/null 2>&1; \
	rm -f $$dir/*.o > /dev/null 2>&1; \
	done
	@-rm -f ${SOTOPDIR}/*.so > /dev/null 2>&1
