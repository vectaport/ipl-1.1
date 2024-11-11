# Makefile generated by imake - do not edit!

# Read "template" to understand how this Makefile was generated.
# Edit <arch.def> to add support for a new platform.
# Edit <InterViews/iv-darwin.cf> to change platform-specific parameters.
# Edit <local.def> to change site-specific parameters.
# Edit <Imakefile> to change actions that make should perform.

# architecture:  DARWIN

 # -------------------------------------------------------------------------
# from <local.def>:

          TOOL_INCLUDE_DIR = /usr/include

    NORM_CCINCLUDES = -I$(CURRENT_DIR)/.. -I$(CURRENT_DIR)/../.. -I$(TOP)/src -I$(TOP)/src/include -I$(IVTOOLSSRC) $(BACKWARD_CCINCLUDES) -I$(IVTOOLSSRC)/include -I$(IVTOOLSSRC)/include/ivstd $(X_CCINCLUDES)

     APP_CCINCLUDES = $(NORM_CCINCLUDES)

	IVTOOLSLIBDIR = /usr/local/lib

        IVTOOLSSRC = /usr/local
   IVTOOLS_VERSION = 2.1.1

LIBUNIDRAWCOMMON = -L$(LIBDIR) -lUnidraw-common
LIBIVCOMMON = -L$(LIBDIR) -lIV-common
LIBTIME = -L$(LIBDIR) -lTime
LIBATTRIBUTE = -L$(LIBDIR) -lAttribute
LIBCOMUTIL = -L$(LIBDIR) -lComUtil
LIBCOMTERP = -L$(LIBDIR) -lComTerp
LIBIVGLYPH = -L$(LIBDIR) -lIVGlyph
LIBATTRGLYPH = -L$(LIBDIR) -lAttrGlyph
LIBCOMGLYPH = -L$(LIBDIR) -lComGlyph
LIBGLYPHTERP = -L$(LIBDIR) -lGlyphTerp
LIBUNIIDRAW = -L$(LIBDIR) -lUniIdraw
LIBTOPOFACE = -L$(LIBDIR) -lTopoFace
LIBOVERLAYUNIDRAW = -L$(LIBDIR) -lOverlayUnidraw

LIBACEDISPATCH = -L$(LIBDIR) -lAceDispatch

LIBCOMUNIDRAW = -L$(LIBDIR) -lComUnidraw
LIBFRAMEUNIDRAW = -L$(LIBDIR) -lFrameUnidraw
LIBGRAPHUNIDRAW = -L$(LIBDIR) -lGraphUnidraw
LIBDRAWSERV = -L$(LIBDIR) -lDrawServ

DEPUNIDRAWCOMMON = $(LIBDIR)/libUnidraw-common.$(IVTOOLS_VERSION).dylib
DEPIVCOMMON = $(LIBDIR)/libIV-common.$(IVTOOLS_VERSION).dylib
DEPTIME = $(LIBDIR)/libTime.$(IVTOOLS_VERSION).dylib
DEPATTRIBUTE = $(LIBDIR)/libAttribute.$(IVTOOLS_VERSION).dylib
DEPCOMUTIL = $(LIBDIR)/libComUtil.$(IVTOOLS_VERSION).dylib
DEPCOMTERP = $(LIBDIR)/libComTerp.$(IVTOOLS_VERSION).dylib
DEPIVGLYPH = $(LIBDIR)/libIVGlyph.$(IVTOOLS_VERSION).dylib
DEPATTRGLYPH = $(LIBDIR)/libAttrGlyph.$(IVTOOLS_VERSION).dylib
DEPCOMGLYPH = $(LIBDIR)/libComGlyph.$(IVTOOLS_VERSION).dylib
DEPGLYPHTERP = $(LIBDIR)/libGlyphTerp.$(IVTOOLS_VERSION).dylib
DEPUNIIDRAW = $(LIBDIR)/libUniIdraw.$(IVTOOLS_VERSION).dylib
DEPTOPOFACE = $(LIBDIR)/libTopoFace.$(IVTOOLS_VERSION).dylib
DEPOVERLAYUNIDRAW = $(LIBDIR)/libOverlayUnidraw.$(IVTOOLS_VERSION).dylib

DEPACEDISPATCH = $(LIBDIR)/libAceDispatch.$(IVTOOLS_VERSION).dylib

DEPCOMUNIDRAW = $(LIBDIR)/libComUnidraw.$(IVTOOLS_VERSION).dylib
DEPFRAMEUNIDRAW = $(LIBDIR)/libFrameUnidraw.$(IVTOOLS_VERSION).dylib
DEPGRAPHUNIDRAW = $(LIBDIR)/libGraphUnidraw.$(IVTOOLS_VERSION).dylib
DEPDRAWSERV = $(LIBDIR)/libDrawServ.$(IVTOOLS_VERSION).dylib

          IPLSRC = $(TOP)/src

LIBIPL = -L$(IPLSRC)/Ipl/$(CPU) -lIpl
LIBIPLSERV = -L$(IPLSRC)/IplServ/$(CPU) -lIplServ
LIBIPLEDIT = -L$(IPLSRC)/IplEdit/$(CPU) -lIplEdit

DEPIPL = $(IPLSRC)/Ipl/$(CPU)/libIpl.$(VERSION).dylib
DEPIPLSERV = $(IPLSRC)/IplServ/$(CPU)/libIplServ.$(VERSION).dylib
DEPIPLEDIT = $(IPLSRC)/IplEdit/$(CPU)/libIplEdit.$(VERSION).dylib

# -------------------------------------------------------------------------

   RELEASE = ipl-1.1.1

   VERSION = 1.1.1

   REPOSITORY_FILES = *.c *.cc *.cxx *.C *.h Imakefile *.def template README INSTALL VERSION MANIFEST README.ivmkcm COPYRIGHT ANNOUNCE *.SGI *.LINUX *.SUN4 *.HP800 *.cf *.sh CHANGES CHANGES-0.5 CHANGES-0.6 configure configure.in config.defs.in *.mk *.m4 *.notes install-sh config.sub config.guess

              SHELL = /bin/sh

              IMAKE = imake
         IMAKEFLAGS = \
 -T "template"\
 -I$(TOP)/config -I$(CONFIGSRC) -I$(XCONFIGDIR)\
 $(SPECIAL_IMAKEFLAGS)
 SPECIAL_IMAKEFLAGS =
             DEPEND = g++ -M
     DEPEND_CCFLAGS = -w -DMAKEDEPEND $(CCDEFINES) $(CCINCLUDES) -I$(TOOL_INCLUDE_DIR) -UHAVE_ACE
               MAKE = make
           PASSARCH = ARCH="$(ARCH)" SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS)" CMFLAGS="$(CMFLAGS)" CMMSG="$(CMMSG)"
          ARCHORCPU = $(CPU)
               ARCH = $(ARCHORCPU)$(SPECIAL_ARCH)
       SPECIAL_ARCH =

           CCDRIVER = g++
           CCSUFFIX = c
            CDRIVER = gcc
            CSUFFIX = c
                SRC = /Users/scottjohnston/src/ipl-1.1/.
              SLASH = /
               SRCS = $(SRC)$(SLASH)*.$(CCSUFFIX)
               OBJS = *.o
               AOUT = a.out

            CCFLAGS = $(APP_CCFLAGS) $(IV_CCFLAGS) $(OTHER_CCFLAGS) $(EXTRA_CCFLAGS)
         IV_CCFLAGS = \
 -g\
 $(OPTIMIZE_CCFLAGS)\
 $(SHARED_CCFLAGS)\
 $(CCDEFINES)\
 $(CCINCLUDES)
      DEBUG_CCFLAGS = -g
   OPTIMIZE_CCFLAGS =
     SHARED_CCFLAGS =
      EXTRA_CCFLAGS = -fno-common

          CCDEFINES = $(APP_CCDEFINES) $(IV_CCDEFINES) $(OTHER_CCDEFINES) $(EXTRA_CCDEFINES)
       IV_CCDEFINES = $(LANGUAGE_CCDEFINES) $(BACKWARD_CCDEFINES)
 LANGUAGE_CCDEFINES = -std=c++11 -Dcplusplus_2_1 -Wno-deprecated
 BACKWARD_CCDEFINES =
    EXTRA_CCDEFINES =

      VTK_CCDEFINES =
 CLIPPOLY_CCDEFINES =
      ACE_CCDEFINES = -DHAVE_ACE -std=c++14

         CCINCLUDES = $(APP_CCINCLUDES) $(IV_CCINCLUDES) $(OTHER_CCINCLUDES) $(EXTRA_CCINCLUDES)
      IV_CCINCLUDES = \
 $(BACKWARD_CCINCLUDES)\
 $(TOP_CCINCLUDES)\
 $(X_CCINCLUDES)
BACKWARD_CCINCLUDES =
     TOP_CCINCLUDES = -I$(INCSRC)
       X_CCINCLUDES = -I$(XINCDIR)
   EXTRA_CCINCLUDES =

     VTK_CCINCLUDES =
    MESA_CCINCLUDES =
CLIPPOLY_CCINCLUDES =
     ACE_CCINCLUDES = -I$(ACEDIR)
    PROJ_CCINCLUDES =
             VTKDIR = VtkDir
        CLIPPOLYDIR = ClipPolyDir

             ACEDIR = /opt/homebrew/Cellar/ace/7.1.0/include

          ACELIBDIR = /opt/homebrew/lib

          CCLDFLAGS = $(APP_CCLDFLAGS) $(IV_CCLDFLAGS) $(OTHER_CCLDFLAGS) $(EXTRA_CCLDFLAGS)
       IV_CCLDFLAGS = \
 -g\
 $(OPTIMIZE_CCFLAGS)\
 $(NONSHARED_CCLDFLAGS)
NONSHARED_CCLDFLAGS =
    EXTRA_CCLDFLAGS = -Wl,-bind_at_load

          CCDEPLIBS = $(APP_CCDEPLIBS) $(IV_CCDEPLIBS) $(OTHER_CCDEPLIBS) $(EXTRA_CCDEPLIBS)
       IV_CCDEPLIBS = \
 $(DEPLIBUNIDRAW)\
 $(DEPLIBGRAPHIC)\
 $(DEPLIBIV)\
 $(DEPLIBXEXT)\
 $(DEPLIBX11)\
 $(DEPLIBM)
    EXTRA_CCDEPLIBS =

	       PROJDIR = ProjDir
            PROJLIBDIR = ProjDir/lib
           PROJLIBBASE = libproj.so.4.3

            LIBPROJ = -L$(PROJLIBDIR) -lproj

            MESALIBDIR = MesaDir/lib
           MESALIBBASE = libMesaGL.so.2
          OPENGLLDLIBS = -L$(MESALIBDIR) -lMesaGL

           CCLDLIBS = $(APP_CCLDLIBS) $(OTHER_CCLDLIBS) $(IV_CCLDLIBS) $(EXTRA_CCLDLIBS)
        IV_CCLDLIBS = \
 $(LIBDIRPATH)\
 $(LDLIBUNIDRAW)\
 $(LDLIBGRAPHIC)\
 $(LDLIBIV)\
 $(XLIBDIRPATH)\
 $(LDLIBXEXT)\
 $(LDLIBXT)\
 $(LDLIBX11)\
 $(ABSLIBDIRPATH)
     EXTRA_CCLDLIBS = -lstdc++ -lm

       VTK_CCLDLIBS =
          VTKLIBDIR =
         VTKLIBBASE = libvtk.so
  CLIPPOLY_CCLDLIBS =
     CLIPPOLYLIBDIR =
    CLIPPOLYLIBBASE = libclipoly.so.1.0
       ACE_CCLDLIBS = -L$(ACELIBDIR) -lACE
          ACELIBDIR = /opt/homebrew/lib
         ACELIBBASE = libACE.so

            INSTALL = install
       INSTPGMFLAGS = -s
       INSTBINFLAGS = -m 0755
       INSTUIDFLAGS = -m 4755
       INSTLIBFLAGS = -m 0644
       INSTINCFLAGS = -m 0444
       INSTMANFLAGS = -m 0444
       INSTDATFLAGS = -m 0444
      INSTKMEMFLAGS = -m 4755

                 AR = ar clq
                 AS = as
                 CP = cp
                CPP = c++ -E $(EXTRA_CCDEFINES) $(OTHER_CCDEFINES)
      PREPROCESSCMD = $(CCDRIVER) -E $(EXTRA_CCDEFINES) $(OTHER_CCDEFINES)
                 LD = ld
                 LN = ln -s
          MKDIRHIER = $(SHELL) $(SCRIPTSRC)/mkdirhier.sh
                 MV = mv
             RANLIB = ranlib
    RANLIBINSTFLAGS =
                 RM = rm -f
              RMDIR = rm -rf
             RM_CMD = $(RM) ,* .emacs_* *..c *.BAK *.CKP *.a *.bak *.ln *.o a.out core errs make.log make.out tags TAGS
              TROFF = groff

                TOP = /Users/scottjohnston/src/ipl-1.1
             RELTOP = .
        CURRENT_DIR = /Users/scottjohnston/src/ipl-1.1

              IVSRC = $(IVTOOLSSRC)
             BINSRC = $(VHCLMAPSSRC)/bin
          CONFIGSRC = $(RELTOP)/config
             INCSRC = $(IVSRC)/include
             LIBSRC = $(VHCLMAPSSRC)
             MANSRC = $(VHCLMAPSSRC)/man
          SCRIPTSRC = $(RELTOP)/src/scripts

             BINDIR = /usr/local/bin
          CONFIGDIR = /usr/local/lib/vhclmaps/config
             INCDIR = /usr/local/include
             LIBDIR = /usr/local/lib
          LIBALLDIR = /usr/local/lib/vhclmaps
             MANDIR = /usr/local/man

       ABSCONFIGDIR = /usr/local/lib/vhclmaps/config
          ABSLIBDIR = /usr/local/lib
       ABSLIBALLDIR = /usr/local/lib/vhclmaps
         XCONFIGDIR = /opt/X11/lib/X11/config
            XINCDIR = /opt/X11/include
            XLIBDIR = /opt/X11/lib
          PSFONTDIR = /usr/lib/ps

include $(RELTOP)/config/config.mk

all::

Makefile::
	-@if [ -f Makefile ]; then \
	$(RM) Makefile.bak; \
	$(MV) Makefile Makefile.bak; \
	else exit 0; fi
	-@echo "Making Makefile"
	-@if [ $(TOP) = $(CURRENT_DIR) ]; then \
	echo >.toplevel-make-makefile; else \
	echo >.not-toplevel-make-makefile; fi
	-@if [ -f .toplevel-make-makefile ]; then \
	echo;\
	echo "remaking top-level Makefile using:";\
	echo "    top-level directory -- $(ABSTOP)";\
	echo "    ipl config directory -- $(CONFIGSRC)";\
	echo "    X11 config directory -- $(XCONFIGDIR)";\
	echo;\
	$(IMAKE) $(IMAKEFLAGS) -I$(ABSTOP)/config \
	-DTOPDIR=$(ABSTOP) -DRELTOPDIR=$(RELTOP) -DCURDIR=$(ABSTOP);fi
	-@if [ -f .not-toplevel-make-makefile ]; then \
	$(IMAKE) $(IMAKEFLAGS) -DTOPDIR=$(TOP) \
	-DRELTOPDIR=$(RELTOP) -DCURDIR=$(CURRENT_DIR); fi
	-@if [ $(TOP) = $(CURRENT_DIR) ]; then \
	$(RM) .toplevel-make-makefile; else \
	$(RM) .not-toplevel-make-makefile; fi
Makefiles::
depend::
install::
uninstall::

clean::
	@$(RM_CMD) "#"*

# -------------------------------------------------------------------------

 #
# "make CPU" prints the name used for machine-specific subdirectories
# "make" builds everything.
# "make install" installs everything.
#

PACKAGE = top_ipl

WORLDOPTS =
SUBDIRS = src config

CPU:
	@$(SHELL) $(SCRIPTSRC)/cpu.sh $(CONFIGSRC)/arch.def

World::
	@echo ""
	@echo "Building $(RELEASE) on `date`"
	@echo ""
	$(MAKE) Makefile CONFIGSRC=$(CONFIGSRC) XCONFIGDIR=$(XCONFIGDIR)
	@echo ""
	@echo "twice to propogate new toplevel pathname"
	$(MAKE) Makefile
	@echo ""
	$(MAKE) Makefiles
	@echo ""
	$(MAKE) depend
	@echo ""
	$(MAKE) $(WORLDOPTS) subdirs
	@echo ""
	@echo "$(RELEASE) build completed on `date`"
	@echo ""

World.debug::
	@echo ""
	@echo "Building $(RELEASE) on `date`"
	@echo ""
	$(MAKE) Makefile XCONFIGDIR=$(XCONFIGDIR)
	@echo ""
	$(MAKE) Makefiles.debug
	@echo ""
	$(MAKE) depend.debug
	@echo ""
	$(MAKE) $(WORLDOPTS) all.debug
	@echo ""
	@echo "$(RELEASE) build completed on `date`"
	@echo ""

World.noshared::
	@echo ""
	@echo "Building $(RELEASE) on `date`"
	@echo ""
	$(MAKE) Makefile XCONFIGDIR=$(XCONFIGDIR)
	@echo ""
	$(MAKE) Makefiles.noshared
	@echo ""
	$(MAKE) depend.noshared
	@echo ""
	$(MAKE) $(WORLDOPTS) all.noshared
	@echo ""
	@echo "$(RELEASE) build completed on `date`"
	@echo ""

all::
	-@if [ ! -f make.makefile ]; then \
	echo ""; \
	echo "Building $(RELEASE) on `date`"; \
	echo ""; \
	$(MAKE) Makefile CONFIGSRC=$(CONFIGSRC) XCONFIGDIR=$(XCONFIGDIR);\
	echo "twice to propogate new toplevel pathname"; \
	$(MAKE) Makefile; \
	touch make.makefile; fi
	-@if [ ! -f make.makefiles ]; then \
	echo ""; \
	$(MAKE) Makefiles; \
	touch make.makefiles; fi
	-@if [ ! -f make.depend ]; then \
	echo ""; \
	$(MAKE) depend; \
	touch make.depend; fi
	-@if [ ! -f make.make ]; then \
	echo ""; fi
	$(MAKE) $(WORLDOPTS) subdirs
	-@if [ ! -f make.make ]; then \
	echo ""; \
	echo "$(RELEASE) build completed on `date`"; \
	echo ""; \
	touch make.make; fi

Makefiles::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "Making Makefiles" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	if [ -f $$i/Makefile ]; then \
	$(RM) $$i/Makefile.bak; \
	$(MV) $$i/Makefile $$i/Makefile.bak; \
	fi; \
	$(IMAKE) $(IMAKEFLAGS) \
	-DTOPDIR=$(TOP) -DRELTOPDIR=../$(RELTOP) \
	-DCURDIR=$(CURRENT_DIR)/$$i \
	-f $$i/Imakefile -s $$i/Makefile; \
	cd $$i; \
	$(MAKE) $(PASSARCH) Makefiles; \
	) else continue; fi; \
	done

depend::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "depending" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) depend; \
	) else continue; fi; \
	done

subdirs::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "making all" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) all; \
	) else continue; fi; \
	done

install::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "installing" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) install; \
	) else continue; fi; \
	done

uninstall::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "uninstalling" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) uninstall; \
	) else continue; fi; \
	done

clean::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "cleaning" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) clean; \
	) else continue; fi; \
	done

Makefiles.debug:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	Makefiles
depend.debug:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	depend
all.debug:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	all
install.debug:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	install
clean.debug:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	clean

Makefiles.noshared:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	Makefiles
depend.noshared:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	depend
all.noshared:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	all
install.noshared:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	install
clean.noshared:
	@$(MAKE) ARCH="$(ARCH).special" \
	SPECIAL_IMAKEFLAGS="$(SPECIAL_IMAKEFLAGS) imakeflags" \
	clean

cmchkin::
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	if [ ! -d $(TOP)/cm ]; then \
	(cd $(TOP); cminstall); fi; fi
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	csh -c '(setenv REPOSITORY_FILES "$(REPOSITORY_FILES)"; \
	cmchkin package $(TOP)/cm $(TOP) $(CURRENT_DIR))'; fi

cmtag::
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	if [ ! -d $(TOP)/cm ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; \
	if [ ! -d $(CURRENT_DIR)/CVS ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; fi
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	cmtag $(CMFLAGS) $(CMARGS); fi

cmadd::
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	if [ ! -d $(TOP)/cm ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; \
	if [ ! -d $(CURRENT_DIR)/CVS ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; fi
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	csh -c '(setenv REPOSITORY_FILES "$(REPOSITORY_FILES)"; cmadd -m null_message)'; fi

cmupdate::
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	if [ ! -d $(TOP)/cm ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; \
	if [ ! -d $(CURRENT_DIR)/CVS ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; fi
	-@if [ $(NO_IVMKCM) ]; then exit 0; else cmupdate; fi

cmcommit::
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	if [ ! -d $(TOP)/cm ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; \
	if [ ! -d $(CURRENT_DIR)/CVS ]; then \
	(echo "Build repository first with cmchkin"; exit 0;); fi; fi
	-@if [ $(NO_IVMKCM) ]; then exit 0; else \
	if [ -z "$(CMMSG)" ]; then cmcommit $(CMFLAGS); \
	else cmcommit $(CMFLAGS) -m "$(CMMSG)"; fi; fi

cmchkin::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "checking into the repository" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) cmchkin; \
	) else continue; fi; \
	done

cmtag::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "tagging the repository" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) cmtag; \
	) else continue; fi; \
	done

cmadd::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "adding to the repository" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) cmadd; \
	) else continue; fi; \
	done

cmupdate::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "updating from repository" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) cmupdate; \
	) else continue; fi; \
	done

cmcommit::
	-@for i in $(SUBDIRS); \
	do \
	if [ -d $$i ]; then ( \
	echo "committing to repository" \
	"for $(ARCH) in $(CURRENT_DIR)/$$i"; \
	cd $$i; \
	$(MAKE) $(PASSARCH) cmcommit; \
	) else continue; fi; \
	done

clean::
	@$(RM_CMD) make.makefile make.makefiles make.depend make.make

