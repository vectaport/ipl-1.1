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

#include <IplEdit/ipleditor.h>
#include <IplEdit/iplfunc.h>
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/iplcreator.h>
#include <IplEdit/iplviewer.h>

#include <IplServ/iplcatalog.h>
#include <IplServ/iplfunc.h>
#include <IplServ/iplhandler.h>

#include <OverlayUnidraw/ovunidraw.h>

#include <Unidraw/selection.h>

#include <ComTerp/comterpserv.h>

/*****************************************************************************/

IplEditor::IplEditor(OverlayComp* gc, OverlayKit* ok) 
: DrawEditor((boolean)false, ok)
{
    Init(gc, "IplEditor");
}

IplEditor::IplEditor(boolean initflag, OverlayKit* ok) : DrawEditor(initflag, ok) {
}

IplEditor::IplEditor(const char* file, OverlayKit* ok)
: DrawEditor((boolean)false, ok)
{
    PreInit();

    if (file == nil) {
	Init();

    } else {
	Catalog* catalog = unidraw->GetCatalog();
	OverlayComp* comp;

	if (catalog->Retrieve(file, (Component*&) comp)) {
          Init(comp);
	}
	else {
	  Init();
	  fprintf(stderr, "ipleditor: couldn't open %s\n", file);
	}
    }
}

void IplEditor::PreInit () {
  _curr_others = _prev_others = nil;
  _num_curr_others = _num_prev_others = 0;
  _texteditor = nil;
  _autonewframe = false;
  _autonewframe_tts = nil;
  _terp = new ComTerpServ();
  ((OverlayUnidraw*)unidraw)->comterp(_terp);
  AddCommands(_terp);
  add_comterp("IplEditor", _terp);
  _last_selection = new Selection;
}


void IplEditor::Init (OverlayComp* comp, const char* name) {
  if (!comp) comp = new IplIdrawComp;
  _overlay_kit->Init(comp, name);
  InitFrame();
}


void IplEditor::AddCommands(ComTerp* comterp) {
    comterp->add_command("singlestep", new SingleStepFunc(comterp, this));
    comterp->add_command("showforkvals", new ShowForkValsFunc(comterp, this));
    comterp->add_command("connview", new ConnViewFunc(comterp, this));
    comterp->add_command("pipeview", new PipeViewFunc(comterp, this));
    comterp->add_command("invoview", new InvoViewFunc(comterp, this));

    DrawEditor::AddCommands(comterp);
    IplservHandler::AddCommands(comterp);
}


