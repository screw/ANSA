.PHONY: all clean cleanall makefiles makefiles-so makefiles-lib makefiles-exe checkmakefiles doxy doc

ifeq ($(INET_ROOT),)
INET_ROOT := ../inet/
endif

all: checkmakefiles src/ansa/features.h
	cd ${INET_ROOT} && $(MAKE) all #how to get INET project path from the IDE?
	cd src && $(MAKE) all

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@rm -f src/Makefile src/ansa/features.h
	@cd tutorials && $(MAKE) clean && rm -rf doc/tutorials

cleantmp:
	cd src && rm -f .tmp* sta*	

MAKEMAKE_OPTIONS := -f --deep -o ANSA -O out -pANSA -KINET_PROJ=$(abspath $(INET_ROOT)) -DINET_IMPORT -I. --no-deep-includes -I. -I$$\(INET_PROJ\)/src -L$$\(INET_PROJ\)/src -lINET$$\(D\)

makefiles: src/ansa/features.h makefiles-so

makefiles-so:
	@FEATURE_OPTIONS=$$(./inet_featuretool options -f -l -c) && cd src && opp_makemake --make-so $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

makefiles-lib:
	@FEATURE_OPTIONS=$$(./inet_featuretool options -f -l -c) && cd src && opp_makemake --make-lib $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

makefiles-exe:
	@FEATURE_OPTIONS=$$(./inet_featuretool options -f -l -c) && cd src && opp_makemake $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '========================================================================'; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '========================================================================'; \
	echo; \
	exit 1; \
	fi

# generate an include file that contains all the WITH_FEATURE macros according to the current enablement of features
src/ansa/features.h: $(wildcard .oppfeaturestate) .oppfeatures
	@./inet_featuretool defines >src/ansa/features.h

doc:
	cd tutorials && $(MAKE) && mkdir -p ../doc/tutorials/wireless && cp -r wireless/html/* ../doc/tutorials/wireless
	cd doc/src/tcp && $(MAKE)
	cd doc/src/manual && $(MAKE)
	doxygen doxy.cfg
