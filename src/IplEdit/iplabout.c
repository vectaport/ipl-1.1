/*
 * Copyright 2005-2008 Theseus Research Inc., All Rights Reserved.
 * 
 * This file may be used under the terms of the GNU General Public
 * License version 2.0 as published by the Free Software Foundation                                
 * and appearing in the file LICENSE.GPL included in the packaging of
 * this file. For commercial licensing contact info@vectaport.com
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND
 *
 */

#include <IplEdit/iplabout.h>
#include <IplEdit/iplclasses.h>
#include <OverlayUnidraw/oved.h>
#include <InterViews/action.h>
#include <IV-3_1/InterViews/dialog.h>
#include <InterViews/layout.h>
#include <InterViews/polyglyph.h>
#include <InterViews/session.h>
#include <InterViews/window.h>
#include <IV-look/kit.h>
#include <string.h>

class IplAboutDialog : public Dialog {
public:
  IplAboutDialog(Glyph*, Style*);
  void dismiss();
};

IplAboutDialog::IplAboutDialog(Glyph* g, Style* s) 
: Dialog(g,s)
{
}

void IplAboutDialog::dismiss() {
    Dialog::dismiss(true);
}

declareActionCallback(IplAboutDialog)
implementActionCallback(IplAboutDialog)

ClassId IplAboutCmd::GetClassId () { return IPLABOUT_CMD; }

boolean IplAboutCmd::IsA (ClassId id) {
    return IPLABOUT_CMD == id || Command::IsA(id);
}

IplAboutCmd::IplAboutCmd (ControlInfo* c) : Command(c) { Init(); }
IplAboutCmd::IplAboutCmd (Editor* ed) : Command(ed) { Init(); }

void IplAboutCmd::Init() {
    const LayoutKit& layout = *LayoutKit::instance();
    const WidgetKit& kit = *WidgetKit::instance();
    PolyGlyph* vb = layout.vbox(25);
    char banner[] = "\
\n\
Copyright 2005-2008  Theseus Research Inc., All Rights Reserved.\n\
\n\
 ";
    vb->append(kit.label(strtok(banner, "\n")));
    char* line = strtok(nil, "\n");
    while (line) {
	vb->append(kit.label(line));
	line = strtok(nil, "\n");
    }
    vb->append(layout.vglue(5));
    Button* but;
    aboutdialog = new IplAboutDialog(kit.outset_frame(layout.margin(vb,5.0)), Session::instance()->style());
    Action* act = new ActionCallback(IplAboutDialog)(this->aboutdialog, &IplAboutDialog::dismiss);
    vb->append(layout.hbox(layout.hglue(), but = kit.push_button("OK", act), layout.hglue()));
    aboutdialog->append_input_handler(but);
    aboutdialog->focus(but);
    Resource::ref(aboutdialog);
}

Command* IplAboutCmd::Copy () {
    Command* copy = new IplAboutCmd(CopyControlInfo());
    InitCopy(copy);
    return copy;
}

void IplAboutCmd::Execute () {
    OverlayEditor* editor = (OverlayEditor*)GetEditor();
    aboutdialog->post_for(editor->GetWindow());
}

boolean IplAboutCmd::Reversible () { return true; }
