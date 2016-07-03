####################
#
# Makefile for Cornell DB Group packages
#
# 2009-09-27 ajd
#
#
# This Makefile controls a single "Package"
# Logically there is one target, which is either
#   a set of (dynamic) libraries or a set of executables.
#
# Usage:
#
#   make debug
#     create debuggable version (-g)
#   make opt
#	  create optimized version
#   make
#     bring current (debug or opt) version up to date
#   make clean
#	  delete all derived files
#	make deepclean
#	  recursive make clean for all dependencies
# TODO: make check is NYI
#   make check
#     (optional) regression test on current version
#
# In theory there is no need to change this Makefile.
# You just create a MakefileInner file for inclusion below.
#
# MakefileInner receives an input environment:
#
# - OSNAME = name of OS
#		(currently in variable OSX_NAME or LINUX_NAME or WINDOWS_NAME)
# - OPTLEVEL = optimization level
#		(debug or opt)
# - CMD_EXT = extension of a command file name
#		(exec on Darwin or linux, exe on WindowsNT)
# - LIB_EXT = extension of a library file name
#		(dylib on Darwin, so on linux, dll on WindowsNT)
# - BIN, LIB, SRC, OBJ, INCLUDE, HDR, CHK_SRC, CHK_OBJ, CHK_HDR = names of our dirs
#
# - CPPFLAGS_* CXXFLAGS_* = compiler flags for build commands
# - INCLUDES_FROM_DIRS (INCLUDES_FROM_PKGS) = list of -I directives
#		for include file lookup
# - IMPORTS_FROM_DIRS (IMPORTS_FROM_PKGS) = list of -L and -l and .o directives
#		for libraries and .o files for linking
# - COMP_* = compile commands
# TODO: debug the above stuff
# The following *should* be defined by MakefileInner
#	(using either = or := or += in gmake is okay)
#
# - PKG = package name
#		(e.g. DBUtl)
# - PKG_KIND = is it a library or command package
#		(lib or cmd)
# - PKG_VER = version of the package
#		(e.g. 1.0.0 - see the dylib documentation)
# - TARGETS = list of target file names with directories
#		(e.g.
#			$(BIN)/start$(PKG).$(CMD_EXT) $(BIN)/stop$(PKG).$(CMD_EXT)
#		or
#			$(LIB)/lib$(PKG).$(PKG_VER).$(LIB_EXT)
#		)
# - BUILDS = list of target file names for which Makefile should provide
#		default build commands based on PKG_KIND.  Note BUILDS logically
#		should be a subset of TARGETS, though this is not enforced.
#
# The following *may*  be defined by MakefileInner
#
# - INCLUDE_PKGS = a list of package names for the compile include path
#		TODO: This also does "cd there && make OPTLEVEL=whatever"
#		for each included PKG.
# - INCLUDE_DIRS = additional include paths for compiling
#		This may also be specified on the command line.
#		It is not necessary (and redundant) to specify a package her
#		if it was in INCLUDE_PKGS above 
# - IMPORT_DIRS (IMPORT_PKGS) = list of absolute
#		File path names (resp path names relative to ..)
#		for libraries or .o files to be imported when linking.
#		If a name has no extension, it is assumed to be a library
#		with the appropriate conventions for the platform, but
#		the full file name may be used as well (e.g. in MacOS X
#		processing of IMPORT_PKGS results compiler command line args:
#			PSimFW/lib/foo.o -> ../PSimFW/lib/foo.o
#			PSimFW/lib/PSimFW -> -L../PSimFW/lib -lPSimFW
#			PSimFW/lib/libPSimFW.dylib -> -L../PSimFW/lib -lPSimFW
#		Here `.o' is really $(OBJ_EXT) and `.dylib' is $(LIB_EXT).
# - MAKE_IMPORTS
#		If this is not defined (default) then recursive make calls
#		are made on each library specified in IMPORT_PKGS, to ensure
#		cross-package references are up-to-date.
#		Define MAKE_IMPORTS as the empty string to suppress this behavior.
#		Or define MAKE_IMPORTS to be a list of (relative or absolute)
#		paths for which ( cd path && make ) will be performed.
# - IMPORT_STD = list of library names for -lfoo directives
# - IMPORT_FWKS = framework names (MacOS X only, sigh)
#
# There should also be dependencies for all build products, e.g.
#	$(OBJ)/RNG.o: $(SRC)/RNG.cpp $(INCLUDE)/RNG.h
#
# There should be dependencies for recursively included header files, e.g.
#	$(HDR)/Basics.h: $(HDR)/Config.h
#
# Implicit rules exist for building object files from .c and .cpp files,
#	for building .h files (simply by touching them) and for building
#   libraries from object files.  You can provide explicit build
#	rules for other cases in MakefileInner if necessary.
#
# TODO: INCLUDE_PKGS a list of package root dirs for include path
#	This also does "cd there && make OPTLEVEL=whatever" for each included PKG.
#	That part is NYI
# TODO: make check support
#	This is not yet completely designed.
# TODO: variables communicated from MakefileInner to tail of Makefile
#	should be smashed on the way in?  Dubito - ajd
#
####################

#
# Directory structure definitions
#

BIN = bin
LIB = lib

SRC = src
OBJ = obj
INCLUDE = include
HDR = include

CHK_SRC = chk/src
CHK_OBJ = chk/obj
CHK_HDR = chk/include

CERT_opt = TIMESTAMP-opt
CERT_debug = TIMESTAMP-debug

#
# Determine OS parameters
#

.SUFFIXES:

ifndef OSNAME
OSNAME := $(shell uname -s)
endif

OSX_NAME = Darwin
LINUX_NAME = Linux
WINDOWS_NAME = WindowsNT

ifeq "$(OSNAME)" "linux"
$(info Changing OSNAME from $(OSNAME) to $(LINUX_NAME))
override OSNAME := $(LINUX_NAME)
endif

export OSNAME

ifeq "$(OSNAME)" "$(OSX_NAME)"

.SUFFIXES: .c .cpp .h .hpp .o .exec .dylib
LIB_EXT = dylib
CMD_EXT = exec
OBJ_EXT = o
SHELL = /bin/sh

else ifeq "$(OSNAME)" "$(LINUX_NAME)"

.SUFFIXES: .c .cpp .h .hpp .o .exec .so
LIB_EXT = so
CMD_EXT = exec
OBJ_EXT = o
SHELL = /bin/sh

else ifeq "$(OSNAME)" "$(WINDOWS_NAME)"

.SUFFIXES: .c .cpp .h .hpp .obj .exe .dll
LIB_EXT = dll
CMD_EXT = exe
OBJ_EXT = obj
$(error WindowsNT shell NYI)

else

$(error unknown OSNAME $(OSNAME))

endif
$(info OSNAME = $(OSNAME))

OPTLEVEL := $(shell if [ $(CERT_opt) -nt $(CERT_debug) ] ; then echo opt ; else echo debug ; fi )
$(info OPTLEVEL = $(OPTLEVEL))
# Note OPTLEVEL is **not** exported from Makefile
NOPTLEVEL_opt = debug
NOPTLEVEL_debug = opt
NOPTLEVEL := $(NOPTLEVEL_$(OPTLEVEL))
$(info NOPTLEVEL = $(NOPTLEVEL))

.PHONY: default default_inner opt debug clean check

default: default_inner

#
# These definitions control compiler flags
#   for various platforms and build levels.
# They can be changed inside the inner Makefile
#   (power users only :-)
#

CXXFLAGS_Darwin_lib_debug = -fno-common -dynamic -g
CXXFLAGS_Linux_lib_debug = -fPIC -g
CXXFLAGS_WindowsNT_lib_debug = XXX
CXXFLAGS_Darwin_cmd_debug = -g
CXXFLAGS_Linux_cmd_debug = -g
CXXFLAGS_WindowsNT_cmd_debug = XXX
CXXFLAGS_Darwin_lib_opt = -fno-common -dynamic -O3
CXXFLAGS_Linux_lib_opt = -fPIC -O3
CXXFLAGS_WindowsNT_lib_opt = XXX
CXXFLAGS_Darwin_cmd_opt = -O3
CXXFLAGS_Linux_cmd_opt = -O3
CXXFLAGS_WindowsNT_cmd_opt = XXX

CXXFLAGS_ALL_lib := $(CXXFLAGS_$(OSNAME)_lib_$(OPTLEVEL))
ifeq "$(CXXFLAGS_ALL_lib)" "XXX"
$(error CXXFLAGS_ALL_lib not defined)
endif
CXXFLAGS_ALL_lib := $(CXXFLAGS_ALL_lib) $(CXXFLAGS)

CXXFLAGS_ALL_cmd := $(CXXFLAGS_$(OSNAME)_cmd_$(OPTLEVEL))
ifeq "$(CXXFLAGS_ALL_cmd)" "XXX"
$(error CXXFLAGS_ALL_cmd not defined)
endif
CXXFLAGS_ALL_cmd := $(CXXFLAGS_ALL_cmd) $(CXXFLAGS)

COMP_CC_lib = $(CXX) -c $(CPPFLAGS_ALL_lib) $(CXXFLAGS_ALL_lib)
COMP_CC_cmd = $(CXX) -c $(CPPFLAGS_ALL_cmd) $(CXXFLAGS_ALL_cmd)
COMP_CC = $(COMP_CC_$(PKG_KIND))
COMP_CXX_lib = $(CXX) -c $(CPPFLAGS_ALL_lib) $(CXXFLAGS_ALL_lib)
COMP_CXX_cmd = $(CXX) -c $(CPPFLAGS_ALL_cmd) $(CXXFLAGS_ALL_cmd)
COMP_CXX = $(COMP_CXX_$(PKG_KIND))

#
# 
include MakefileInner
#
#
$(info TARGETS = $(TARGETS))
$(info INCLUDE_DIRS = $(INCLUDE_DIRS))
$(info INCLUDE_PKGS = $(INCLUDE_PKGS))

#
# Generic targets
#

default_inner:
	$(MAKE) $(OPTLEVEL)
	touch $(CERT_$(OPTLEVEL))

opt:
	if [ $(OPTLEVEL) = debug ] ;\
		then $(MAKE) clean;  sleep 1; touch $(CERT_opt) ;\
	fi
	$(MAKE) $(TARGETS)

debug:
	if [ $(OPTLEVEL) = opt ] ;\
		then $(MAKE) clean;  sleep 1; touch $(CERT_debug) ;\
	fi
	$(MAKE) $(TARGETS)

clean:
	find . -name \*.o | xargs rm -f
	rm -f $(LIB)/*.$(LIB_EXT) $(BIN)/*.$(CMD_EXT) $(CHK_OBJ)/*.$(OBJ_EXT)
	touch $(CERT_debug); touch $(CERT_opt); sleep 1; touch $(CERT_$(OPTLEVEL))

check:
	$(error make check NYI)


CXXFLAGS_ALL := $(CXXFLAGS_ALL_$(PKG_KIND))
$(info CXXFLAGS_ALL = $(CXXFLAGS_ALL))

INCLUDES_FROM_DIRS := $(patsubst %,-I%,$(INCLUDE_DIRS))
INCLUDES_FROM_PKGS := $(patsubst %,-I../%/include,$(INCLUDE_PKGS))
INCLUDES_ALL := $(INCLUDES_FROM_DIRS) $(INCLUDES_FROM_PKGS)

OBJ_IMPORTS := $(filter %.$(OBJ_EXT),$(IMPORT_PKGS))
OBJ_IMPORTS := $(patsubst %,../%,$(OBJ_IMPORTS))
OBJ_IMPORTS += $(filter %.$(OBJ_EXT),$(IMPORT_DIRS))

LIB_IMPORTS := $(filter-out %.$(OBJ_EXT),$(IMPORT_PKGS))
LIB_IMPORTS := $(patsubst %,../%,$(LIB_IMPORTS))
LIB_IMPORTS += $(filter-out %.$(OBJ_EXT),$(IMPORT_DIRS))
LIB_IMPORTS_L := $(foreach x,$(LIB_IMPORTS),-L$(dir $(x)))
#
ifndef MAKE_IMPORTS
MAKE_IMPORTS := $(subst /$(LIB)/,/,$(foreach x,$(LIB_IMPORTS),$(dir $(x))))
$(info MAKE_IMPORTS = $(MAKE_IMPORTS))
else ifeq "$(MAKE_IMPORTS)X" "X"
$(info MAKE_IMPORTS empty ... skipping)
endif
#
LIB_IMPORTS_l := $(foreach x,$(LIB_IMPORTS),-l$(notdir $(x)))
LIB_IMPORTS_l := $(patsubst -llib%.$(LIB_EXT),-l%,$(LIB_IMPORTS_l))
LIB_IMPORTS_l += $(patsubst %,-l%,$(IMPORT_STD))
LIB_IMPORTS_l += $(patsubst %,-framework %,$(IMPORT_FWKS))

IMPORTS_ALL := $(OBJ_IMPORTS) $(LIB_IMPORTS_L) $(LIB_IMPORTS_l)
$(info IMPORTS_ALL = $(IMPORTS_ALL))

CPPFLAGS_ALL_lib := -I$(HDR) -I$(CHK_HDR) \
	-I.. $(INCLUDES_FROM_PKGS) $(INCLUDES_FROM_DIRS) \
	-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS $(CPPFLAGS)

CPPFLAGS_ALL_cmd := -I$(HDR) -I$(CHK_HDR) \
	-I.. $(INCLUDES_FROM_PKGS) $(INCLUDES_FROM_DIRS) \
	-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS $(CPPFLAGS)

CPPFLAGS_ALL := $(CPPFLAGS_ALL_$(PKG_KIND))
$(info CPPFLAGS_ALL = $(CPPFLAGS_ALL))

$(OBJ)/%.$(OBJ_EXT): $(SRC)/%.c
	$(COMP_CC) -o $@ $<
#	$(CXX) -c $(CPPFLAGS_ALL) $(CXXFLAGS_ALL) -o $@ $<

$(OBJ)/%.$(OBJ_EXT): $(SRC)/%.cpp
	$(COMP_CXX) -o $@ $<
#	$(CXX) -c $(CPPFLAGS_ALL) $(CXXFLAGS_ALL) -o $@ $<

$(CHK_OBJ)/%.$(OBJ_EXT): $(CHK_SRC)/%.c
	$(COMP_CC) -o $@ $<
#	$(CXX) -c $(CPPFLAGS_ALL) $(CXXFLAGS_ALL) -o $@ $<

$(CHK_OBJ)/%.$(OBJ_EXT): $(CHK_SRC)/%.cpp
	$(COMP_CXX) -o $@ $<
#	$(CXX) -c $(CPPFLAGS_ALL) $(CXXFLAGS_ALL) -o $@ $<

%.h:
	touch $@

deepclean:
ifneq "$(MAKE_IMPORTS)X" "X"
	for x in $(MAKE_IMPORTS) ; do ( cd $$x && $(MAKE) deepclean ); done
endif
	$(MAKE) clean


$(BUILDS):
ifneq "$(MAKE_IMPORTS)X" "X"
	for x in $(MAKE_IMPORTS) ; do ( cd $$x && $(MAKE) $(OPTLEVEL) ); done
endif
ifeq "$(PKG_KIND)" "lib"
ifeq "$(OSNAME)" "$(OSX_NAME)"
	$(CXX) -dynamiclib $(CXXFLAGS_ALL_lib) $(IMPORTS_ALL) -o $@ -dylib $^
	rm -f $(@D)/$(OPTLEVEL)-$(@F) ; rm -f $(@D)/$(NOPTLEVEL)-$(@F)
	ln -s $@ $(@D)/$(OPTLEVEL)-$(@F)
else ifeq "$(OSNAME)" "$(LINUX_NAME)"
	$(CXX) -shared -Wl,-soname,$(@F) $(CXXFLAGS_ALL_lib) $(IMPORTS_ALL) -o $@ $^ -lc
	rm -f $(@D)/$(OPTLEVEL)-$(@F) ; rm -f $(@D)/$(NOPTLEVEL)-$(@F)
	cd $(@D) && ln -s $(@F) $(OPTLEVEL)-$(@F) && cd ..
else ifeq "$(OSNAME)" "$(WINDOWS_NAME)"
	$(error command for WindowsNT dll creation from $(OBJ_EXT) files NIY)
endif
else ifeq "$(PKG_KIND)" "cmd"
ifeq "$(OSNAME)" "$(OSX_NAME)"
	$(CXX) $(CXXFLAGS_ALL_cmd) $(IMPORTS_ALL) -o $@ $^
	rm -f $(@D)/$(OPTLEVEL)-$(@F) ; rm -f $(@D)/$(NOPTLEVEL)-$(@F)
	ln -s $@ $(@D)/$(OPTLEVEL)-$(@F)
else ifeq "$(OSNAME)" "$(LINUX_NAME)"
	$(CXX) $(CXXFLAGS_ALL_cmd) $(IMPORTS_ALL) -o $@ $^
	rm -f $(@D)/$(OPTLEVEL)-$(@F) ; rm -f $(@D)/$(NOPTLEVEL)-$(@F)
	cd $(@D) && ln -s $(@F) $(OPTLEVEL)-$(@F) && cd ..
else ifeq "$(OSNAME)" "$(WINDOWS_NAME)"
	$(error c++ command for WindowsNT NYI)
endif
endif
