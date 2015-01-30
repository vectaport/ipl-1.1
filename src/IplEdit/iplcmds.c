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

#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcmds.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/ipleditor.h>
#include <IplEdit/iplviewer.h>
#include <IplServ/iplcomps.h>

#include <GraphUnidraw/edgecomp.h>
#include <Unidraw/clipboard.h>
#include <Unidraw/selection.h>

#include <TopoFace/topoedge.h>
#include <TopoFace/toponode.h>

/*****************************************************************************/

static boolean selected(Selection* s, NodeComp* comp) {
    Iterator i;
    for (s->First(i); !s->Done(i); s->Next(i)) {
        GraphicComp* gcomp = s->GetView(i)->GetGraphicComp();
	if (gcomp == comp)
	    return true;
    }
    return false;
}

static int node_index (Selection* s, NodeComp *comp) {
    Iterator i;
    int index = -1;

    for (s->First(i); !s->Done(i); s->Next(i)) {
	GraphicComp* tcomp = s->GetView(i)->GetGraphicComp();
	if (tcomp->IsA(NODE_COMP))
	    index++;
        if (tcomp == comp)
	    return index;
    }
    return -1;
}

static NodeComp* node (Clipboard* cb, int index) {
    if (index == -1)
	return nil;

    Iterator i;
    int count = -1;
    for (cb->First(i); !cb->Done(i); cb->Next(i)) {
	GraphicComp* gcomp = cb->GetComp(i);
	if (gcomp->IsA(NODE_COMP)) {
	    count++;
	    if (count == index)
	        return (NodeComp*)gcomp;
        }
    }
    return nil;
}

static void index_clipboard(Selection* s, Clipboard* cb) {
    Iterator i, j;
    cb->First(j);
    for (s->First(i); !s->Done(i); s->Next(i)) {
        GraphicComp* gcomp = s->GetView(i)->GetGraphicComp();
        if (gcomp->IsA(EDGE_COMP)) {
            GraphicComp* cbgcomp = cb->GetComp(j);
	    EdgeComp* comp = (EdgeComp*)gcomp;
            TopoEdge* topoedge = comp->Edge();
            const TopoNode* node;
            int start = -1;
            int end = -1;
	    if (node = topoedge->start_node()) {
	      PipeComp* pipecomp = (PipeComp*)node->value();
	      if (!pipecomp->onscreen()) pipecomp = pipecomp->invocomp();
	      if (selected(s, (NodeComp*)pipecomp))
		start = node_index(s, (NodeComp*)pipecomp);
	      }
	    if (node = topoedge->end_node()) {
	      PipeComp* pipecomp = (PipeComp*)node->value();
	      if (!pipecomp->onscreen()) pipecomp = pipecomp->invocomp();
	      if (selected(s, pipecomp))
		end = node_index(s, pipecomp);
	      }

	    EdgeComp* cbcomp = (EdgeComp*)cbgcomp;
	    cbcomp->SetStartNode(start);
	    cbcomp->SetEndNode(end);
        }
        cb->Next(j);
    }
}

/*****************************************************************************/

ClassId IplDupCmd::GetClassId () { return IPLDUP_CMD; }

boolean IplDupCmd::IsA (ClassId id) {
    return IPLDUP_CMD == id || GraphDupCmd::IsA(id);
}

IplDupCmd::IplDupCmd (ControlInfo* c, Clipboard* cb) : GraphDupCmd(c, cb) {
}

IplDupCmd::IplDupCmd (Editor* ed, Clipboard* cb) : GraphDupCmd(ed, cb) {
}

IplDupCmd::~IplDupCmd () {
}

Command* IplDupCmd::Copy () {
    Command* copy = new IplDupCmd(CopyControlInfo(), GetClipboard());
    InitCopy(copy);
    return copy;
}

void IplDupCmd::Execute () {
    Editor* editor = GetEditor();
    Selection* s = editor->GetSelection();
    Clipboard* cb = new Clipboard();
    GraphicView* views = ((OverlayViewer*)editor->GetViewer())->GetCurrentGraphicView();
    s->Sort(views);
    cb->CopyInit(s);
    index_clipboard(s, cb);

    editor->GetComponent()->Interpret(this);

    Iterator i, k, j;
    Clipboard* cmdcb = GetClipboard();

    if (cmdcb == nil) {
      cb->DeleteComps();
      delete cb;
      _executed = false;
      return;
    }

    cmdcb->First(k);
    for (cb->First(i); !cb->Done(i); cb->Next(i)) {
        GraphicComp* gcomp = cb->GetComp(i);
        if (gcomp->IsA(EDGE_COMP)) {
	    GraphicComp* cmdgcomp = cmdcb->GetComp(k);
	    ConnComp* comp = (ConnComp*)gcomp;
	    ConnComp* cmdcomp = (ConnComp*)cmdgcomp;
            PipeComp* start = (PipeComp*)node(cmdcb, comp->GetStartNode());
   	    PipeComp* end = (PipeComp*)node(cmdcb, comp->GetEndNode());

	    PipeComp* substart = nil;
	    if (start && start->IsA(INVO_COMP))
	      substart = ((InvoComp*)start)->find_subpipe(false, comp->GetStartSubNode());

	    PipeComp* subend = nil;
	    if (end && end->IsA(INVO_COMP))
	      subend = ((InvoComp*)end)->find_subpipe(true, comp->GetEndSubNode());

            EdgeConnectCmd* cmd = new EdgeConnectCmd(editor, cmdcomp, substart ? substart : start, subend ? subend : end);
	    cmd->Execute();
	    delete cmd;
        }
        cmdcb->Next(k);
    }
    cb->DeleteComps();
    delete cb;

    _executed = true;
}

/*****************************************************************************/

ClassId IplPasteCmd::GetClassId () { return IPLPASTE_CMD; }

boolean IplPasteCmd::IsA (ClassId id) {
    return IPLPASTE_CMD == id || GraphPasteCmd::IsA(id);
}

IplPasteCmd::IplPasteCmd (ControlInfo* c, Clipboard* cb) : GraphPasteCmd(c, cb) {
}

IplPasteCmd::IplPasteCmd (Editor* ed, Clipboard* cb) : GraphPasteCmd(ed, cb) {
}

IplPasteCmd::~IplPasteCmd () {
}

Command* IplPasteCmd::Copy () {
    Command* copy = new IplPasteCmd(CopyControlInfo(), DeepCopyClipboard());
    InitCopy(copy);
    return copy;
}

void IplPasteCmd::Execute () {
    PasteCmd::Execute();
    Clipboard* cb = GetClipboard();
    if (!cb) return;
    Iterator i;
    for (cb->First(i); !cb->Done(i); cb->Next(i)) {
        GraphicComp* gcomp = cb->GetComp(i);
        if (gcomp->IsA(CONN_COMP)) {
	    ConnComp* comp = (ConnComp*)gcomp;
            PipeComp* start = (PipeComp*)node(cb, comp->GetStartNode());
   	    PipeComp* end = (PipeComp*)node(cb, comp->GetEndNode());

	    PipeComp* substart = nil;
	    if (start && start->IsA(INVO_COMP))
	      substart = ((InvoComp*)start)->find_subpipe(false, comp->GetStartSubNode());
	    
	    PipeComp* subend = nil;
	    if (end && end->IsA(INVO_COMP))
	      subend = ((InvoComp*)end)->find_subpipe(true, comp->GetEndSubNode());
	    
            EdgeConnectCmd* cmd = new EdgeConnectCmd(GetEditor(), comp, 
						     substart ? substart : start, 
						     subend ? subend : end);
	    cmd->Execute();
	    delete cmd;
        }
        else if (gcomp->IsA(EDGE_COMP)) {
	    EdgeComp* comp = (EdgeComp*)gcomp;
            NodeComp* start = node(cb, comp->GetStartNode());
   	    NodeComp* end = node(cb, comp->GetEndNode());
            EdgeConnectCmd* cmd = new EdgeConnectCmd(GetEditor(), comp, start, end);
	    cmd->Execute();
	    delete cmd;
        }
    }
}

/*****************************************************************************/

ClassId SquareCmd::GetClassId () { return SQUARE_CMD; }

boolean SquareCmd::IsA (ClassId id) {
    return SQUARE_CMD == id || Command::IsA(id);
}

SquareCmd::SquareCmd (ControlInfo* c) : Command(c) {
}

SquareCmd::SquareCmd (Editor* ed) : Command(ed) {
}

Command* SquareCmd::Copy () {
    Command* copy = new SquareCmd(CopyControlInfo());
    InitCopy(copy);
    return copy;
}

/*****************************************************************************/

ClassId ReorientTextCmd::GetClassId () { return REORIENT_TEXT_CMD; }

boolean ReorientTextCmd::IsA (ClassId id) {
    return REORIENT_TEXT_CMD == id || Command::IsA(id);
}

ReorientTextCmd::ReorientTextCmd (ControlInfo* c) : Command(c) {
}

ReorientTextCmd::ReorientTextCmd (Editor* ed) : Command(ed) {
}

Command* ReorientTextCmd::Copy () {
    Command* copy = new ReorientTextCmd(CopyControlInfo());
    InitCopy(copy);
    return copy;
}

/*****************************************************************************/ 
CopyMoveIplFrameCmd::CopyMoveIplFrameCmd(ControlInfo* i, boolean after)
: CopyMoveGraphFrameCmd(i, after)
{
}

CopyMoveIplFrameCmd::CopyMoveIplFrameCmd(Editor* e, boolean after)
: CopyMoveGraphFrameCmd(e, after)
{
}

ClassId CopyMoveIplFrameCmd::GetClassId() { return COPYMOVEIPLFRAME_CMD; }
boolean CopyMoveIplFrameCmd::IsA(ClassId id) {
    return id == COPYMOVEIPLFRAME_CMD || CopyMoveGraphFrameCmd::IsA(id);
}

Command* CopyMoveIplFrameCmd::Copy() {
    Command* copy = new CopyMoveIplFrameCmd(CopyControlInfo(), _after);
    InitCopy(copy);
    return copy;
}

void CopyMoveIplFrameCmd::Execute() {
  FrameEditor* ed = (FrameEditor*)GetEditor();
  Append(new OvSlctAllCmd(ed));
  Append(new GraphCopyCmd(ed));
  Append(new CreateFrameCmd(ed, _after));
  Append(new MoveFrameCmd(ed, _after ? +1 : -1));
  Append(new IplPasteCmd(ed));
  MacroCmd::Execute();
}

