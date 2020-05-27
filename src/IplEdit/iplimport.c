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
 * IplImportCmd implementation.
 */

#include <IplEdit/iplimport.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcmds.h>
#include <IplEdit/ipleditor.h>
#include <IplServ/iplcatalog.h>

#include <DrawServ/drawcmds.h>
#include <FrameUnidraw/framecmds.h>

#include <IVGlyph/importchooser.h>

#include <Unidraw/clipboard.h>
#include <Unidraw/iterator.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/viewer.h>

#include <string.h>

/*****************************************************************************/

ClassId IplImportCmd::GetClassId () { return IPLIMPORT_CMD; }

boolean IplImportCmd::IsA (ClassId id) {
    return IPLIMPORT_CMD == id || DrawImportCmd::IsA(id);
}

IplImportCmd::IplImportCmd (ControlInfo* c, ImportChooser* f) : DrawImportCmd(c, f) { 
}

IplImportCmd::IplImportCmd (Editor* ed, ImportChooser* f) : DrawImportCmd(ed, f) { 
}

Command* IplImportCmd::Copy () {
    IplImportCmd* copy = new IplImportCmd(CopyControlInfo(), chooser_);
    InitCopy(copy);
    return copy;
}

void IplImportCmd::Execute () { 
    GraphicComp* comps = PostDialog();

    Clipboard* cb;
    if (comps != nil) {
      DrawImportPasteCmd* paste_cmd = new DrawImportPasteCmd(GetEditor(), new Clipboard(comps));
      paste_cmd->Execute();
      paste_cmd->Log();
      
      if(!comps->IsA(FRAME_IDRAW_COMP) && !comps->IsA(DRAW_IDRAW_COMP)
	 && !comps->IsA(IPL_IDRAW_COMP)) {
	if (chooser_->centered())
	  GetEditor()->GetViewer()->Align(comps, /* Center */ 4);
	
	if (!chooser_->by_pathname()) {
	  FrameUngroupCmd* ungroup_cmd = new FrameUngroupCmd(GetEditor());
	  ungroup_cmd->Execute();
	  MacroCmd* macro_cmd = new MacroCmd(GetEditor(), paste_cmd, ungroup_cmd);
	  macro_cmd->Log();
	} else {
	  paste_cmd->Log();
        }
      } else 
	delete comps;
    }
} 



GraphicComp* IplImportCmd::Import (const char* pathname) {
    GraphicComp* comp = nil;
    const char* creator = ReadCreator(pathname);
    IplCatalog* catalog = (IplCatalog*)unidraw->GetCatalog();

    if (strcmp(creator, "ipledit") == 0) {
        catalog->SetImport(true);
        if (catalog->IplCatalog::Retrieve(pathname, (Component*&) comp)) {
            catalog->SetImport(false);
	    catalog->Forget(comp);
	    return comp;
	}
        catalog->SetImport(false);
	return nil;
    } else 
        return DrawImportCmd::Import(pathname);
}
