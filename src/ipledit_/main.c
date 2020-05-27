/*
 * Copyright 2014-2018 Vectaport Consulting
 * Copyright 2005-2008 Theseus Research Inc.
 * 
 * This file may be used under the terms of the GNU General Public
 * License version 2.0 as published by the Free Software Foundation                                
 * and appearing in the file LICENSE.GPL included in the packaging of
 * this file. For commercial licensing contact info@vectaport.com
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND
 *
 */

/*
 * ipledit main program.
 */


#include <IplEdit/iplcreator.h>
#include <IplEdit/ipleditor.h>

#include <IplServ/iplcatalog.h>
#include <DrawServ/drawserv.h>

#include <InterViews/world.h>

#ifdef HAVE_ACE
#include <DrawServ/drawserv-handler.h>
#include <OverlayUnidraw/aceimport.h>
#include <AceDispatch/ace_dispatcher.h>
#include <ComTerp/comhandler.h>
#endif

#include <stream.h>
#include <ipl/version.h>
using std::cerr;

/*****************************************************************************/

static PropertyData properties[] = {
    { "*IplEdit*name", "ipl editor" },
    { "*IplEdit*iconName", "ipledit]" },
    { "*domain",  "ipl" },
    { "*initialbrush",  "2" },
    { "*initialfgcolor","1" },
    { "*initialbgcolor","10" },
    { "*initialfont",   "4" },
    { "*initialpattern","1" },
    { "*initialarrow", "none" },
    { "*pagewidth", "11" },
    { "*pageheight", "8.5" },
    { "*pagecols", "0" },
    { "*pagerows", "0" },
    { "*gridxincr", "8" },
    { "*gridyincr", "8" },
    { "*font1", "-*-courier-medium-r-normal-*-8-*-*-*-*-*-*-* Courier 8" },
    { "*font2", "-*-courier-medium-r-normal-*-10-*-*-*-*-*-*-* Courier 10" },
    { "*font3", "-*-courier-bold-r-normal-*-12-*-*-*-*-*-*-* Courier-Bold 12" },
    { "*font4",
	"-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-* Helvetica 12"
    },
    { "*font5",
	"-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-* Helvetica 14"
    },
    { "*font6",
	"-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-* Helvetica-Bold 14"
    },
    { "*font7",
	"-*-helvetica-medium-o-normal-*-14-*-*-*-*-*-*-* Helvetica-Oblique 14"
    },
    { "*font8",
	"-*-times-medium-r-normal-*-12-*-*-*-*-*-*-*  Times-Roman 12"
    },
    { "*font9", "-*-times-medium-r-normal-*-14-*-*-*-*-*-*-* Times-Roman 14" } ,
    { "*font10", "-*-times-bold-r-normal-*-14-*-*-*-*-*-*-*  Times-Bold 14" },
    { "*font11",
	"-*-times-medium-i-normal-*-14-*-*-*-*-*-*-* Times-Italic 14"
    },
    { "*font12", "-*-courier-medium-r-normal-*-16-*-*-*-*-*-*-* Courier 16" },
    { "*font13", "-*-courier-medium-r-normal-*-18-*-*-*-*-*-*-* Courier 18" },
    { "*font14", "-*-courier-medium-r-normal-*-20-*-*-*-*-*-*-* Courier 20" },
    { "*font15", "-*-courier-medium-r-normal-*-24-*-*-*-*-*-*-* Courier 24" },
    { "*brush1",	"none" },
    { "*brush2",	"ffff 0" },
    { "*brush3",	"ffff 1" },
    { "*brush4",	"ffff 2" },
    { "*brush5",	"ffff 3" },
    { "*brush6",	"fff0 0" },
    { "*brush7",	"fff0 1" },
    { "*brush8",	"fff0 2" },
    { "*brush9",	"fff0 3" },
    { "*pattern1",	"none" },
    { "*pattern2",	"0.0" },
    { "*pattern3",	"1.0" },
    { "*pattern4",	"0.75" },
    { "*pattern5",	"0.5" },
    { "*pattern6",	"0.25" },
    { "*pattern7",	"1248" },
    { "*pattern8",	"8421" },
    { "*pattern9",	"f000" },
    { "*pattern10",	"8888" },
    { "*pattern11",	"f888" },
    { "*pattern12",	"8525" },
    { "*pattern13",	"cc33" },
    { "*pattern14",	"7bed" },
    { "*fgcolor1",	"Black" },
    { "*fgcolor2",	"Brown 42240 10752 10752" },
    { "*fgcolor3",	"Red" },
    { "*fgcolor4",	"Orange" },
    { "*fgcolor5",	"Yellow" },
    { "*fgcolor6",	"Green" },
    { "*fgcolor7",	"Blue" },
    { "*fgcolor8",	"Indigo 48896 0 65280" },
    { "*fgcolor9",	"Violet 20224 12032 20224" },
    { "*fgcolor10",	"White" },
    { "*fgcolor11",	"LtGray 50000 50000 50000" },
    { "*fgcolor12",	"DkGray 33000 33000 33000" },
    { "*bgcolor1",	"Black" },
    { "*bgcolor2",	"Brown 42240 10752 10752" },
    { "*bgcolor3",	"Red" },
    { "*bgcolor4",	"Orange" },
    { "*bgcolor5",	"Yellow" },
    { "*bgcolor6",	"Green" },
    { "*bgcolor7",	"Blue" },
    { "*bgcolor8",	"Indigo 48896 0 65280" },
    { "*bgcolor9",	"Violet 20224 12032 20224" },
    { "*bgcolor10",	"White" },
    { "*bgcolor11",	"LtGray 50000 50000 50000" },
    { "*bgcolor12",	"DkGray 33000 33000 33000" },
    { "*history",	"20" },
    { "*tile",          "false" },
    { "*twidth",        "512" },
    { "*theight",       "512" },
#ifdef HAVE_ACE
    { "*comdraw",       "20010" },
    { "*import",        "20011" },
#endif
    { "*font",          "-adobe-helvetica-medium-r-normal--14-140-75-75-p-77-iso8859-1"  },
    { "*arch",          "true" },
    { "*mem",          "true" },
    { nil }
};

static OptionDesc options[] = {
    { "-history", "*history", OptionValueNext },
    { "-tile", "*tile", OptionValueImplicit, "true" },
    { "-twidth", "*twidth", OptionValueNext },
    { "-theight", "*theight", OptionValueNext },
    { "-pagecols", "*pagecols", OptionValueNext },
    { "-pagerows", "*pagerows", OptionValueNext },
#ifdef HAVE_ACE
    { "-comdraw", "*comdraw", OptionValueNext },
    { "-import", "*import", OptionValueNext },
#endif
    { "-arch", "*arch", OptionValueImplicit, "true" },
    { "-mem", "*mem", OptionValueImplicit, "true" },
    { nil }
};

/*****************************************************************************/

int main (int argc, char** argv) {
#ifdef HAVE_ACE
    Dispatcher::instance(new AceDispatcher(ComterpHandler::reactor_singleton()));
#endif

    IplEditCreator creator;
    IplCatalog* catalog = new IplCatalog("ipledit", &creator);
    DrawServ* unidraw = new DrawServ(
        catalog, argc, argv, options, properties
    );
    OverlayUnidraw::deferred_notifications(true);

#ifdef HAVE_ACE
   // Acceptor factory.
    UnidrawComterpAcceptor* peer_acceptor = new UnidrawComterpAcceptor();

    const char* comdrawstr = catalog->GetAttribute("comdraw");
    int comdrawnum = atoi(comdrawstr);
    if (peer_acceptor->open 
	(ACE_INET_Addr (comdrawnum)) == -1)
        cerr << "ipledit]:  unable to open comterp comdraw " << comdrawnum << "\n";

    else if (ComterpHandler::reactor_singleton()->register_handler 
	     (peer_acceptor, ACE_Event_Handler::READ_MASK) == -1)
        cerr << "ipledit]:  unable to register ComterpAcceptor with ACE reactor\n";
    else
      cerr << "accepting comdraw port (" << comdrawnum << ") connections\n";

    UnidrawImportAcceptor* import_acceptor = new UnidrawImportAcceptor();

    const char* importstr = catalog->GetAttribute("import");
    int importnum = atoi(importstr);
    if (import_acceptor->open 
	(ACE_INET_Addr (importnum)) == -1)
        cerr << "ipledit]:  unable to open import port " << importnum << "\n";

    else if (ComterpHandler::reactor_singleton()->register_handler 
	     (import_acceptor, ACE_Event_Handler::READ_MASK) == -1)
        cerr << "ipledit]:  unable to register UnidrawImportAcceptor with ACE reactor\n";
    else
      cerr << "accepting import port (" << importnum << ") connections\n";

    // Register COMTERP_QUIT_HANDLER to receive SIGINT commands.  When received,
    // COMTERP_QUIT_HANDLER becomes "set" and thus, the event loop below will
    // exit.
#if 0
    if (ComterpHandler::reactor_singleton()->register_handler 
	     (SIGINT, COMTERP_QUIT_HANDLER::instance ()) == -1)
        cerr << "ipledit]:  unable to register quit handler with ACE reactor\n";
#endif

#endif
    int exit_status = 0;
    if (argc > 2) {
#ifdef HAVE_ACE
	cerr << "Usage: ipledit] [-tile] [-twidth n] [-theight n] [-pagecols n] [-pagerows n] [-comdraw n] [-import n] [file]\n";
#else
	cerr << "Usage: ipledit [-tile] [-twidth n] [-theight n] [-pagecols n] [-pagerows n] [file]\n";
#endif
	exit_status = 1;

    } else {
	const char* initial_file = (argc == 2) ? argv[1] : nil;
	IplEditor* ed = new IplEditor(initial_file);

	unidraw->Open(ed);
#ifdef HAVE_ACE
	/*  Start up one on stdin */
	DrawServHandler* stdin_handler = new DrawServHandler(((OverlayUnidraw*)unidraw)->comterp());
#if 0
	if (ACE::register_stdin_handler(stdin_handler, ComterpHandler::reactor_singleton(), nil) == -1)
#else
	if (ComterpHandler::reactor_singleton()->register_handler(0, stdin_handler, 
							  ACE_Event_Handler::READ_MASK)==-1)
#endif
	  cerr << "ipledit: unable to open stdin with ACE\n";
	ed->SetComTerp(stdin_handler->comterp());
	fprintf(stderr, "ipledit-%s: type help here for command info\n", IplVersionString);
#else
	fprintf(stderr, "ipledit-%s", IplVersionString);
#endif
	unidraw->Run();
    }

    delete unidraw;
    return exit_status;
}
