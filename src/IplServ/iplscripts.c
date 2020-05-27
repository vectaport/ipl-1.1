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

#include <IplServ/iplclasses.h>
#include <IplServ/iplcomps.h>
#include <IplServ/iplscripts.h>

#include <OverlayUnidraw/paramlist.h>
#include <OverlayUnidraw/ovclasses.h>
#include <OverlayUnidraw/ovfile.h>

#include <UniIdraw/idarrows.h>
#include <Unidraw/Graphic/ellipses.h>
#include <Unidraw/Graphic/picture.h>
#include <Unidraw/catalog.h>
#include <Unidraw/editor.h>
#include <Unidraw/iterator.h>
#include <Unidraw/ulist.h>
#include <Unidraw/unidraw.h>

#include <InterViews/transformer.h>

#include <TopoFace/topoedge.h>
#include <TopoFace/toponode.h>

#include <iostream.h>
#include <stdio.h>

/*****************************************************************************/

PipeScript::PipeScript (PipeComp* subj) : NodeScript(subj) { }
ClassId PipeScript::GetClassId () { return PIPE_SCRIPT; }

boolean PipeScript::IsA (ClassId id) { 
    return PIPE_SCRIPT == id || NodeScript::IsA(id);
}

boolean PipeScript::Definition (ostream& out) {
    out << script_name() << "(" ;
    Attributes(out);
    out << ")";
    return out.good();
}

void PipeScript::Attributes(ostream& out) {
    PipeComp* comp = (PipeComp*) GetSubject();

    /* graph */
    GraphComp* graph = comp->GetGraph();
    if (graph)
        out << " :graph \"" << graph->GetPathName() << "\"";

    boolean reqlabel = comp->RequireLabel();
    out << " :reqlabel " << reqlabel;

    /* SF_Ellipse */
    Coord x0, y0;
    int r1, r2;
    SF_Ellipse* ellipse = comp->GetEllipse();
    ellipse->GetOriginal(x0, y0, r1, r2);
    out << " :ellipse " << x0 << "," << y0 << "," << r1 << "," << r2;
    Transformation(out, "ellipsetrans", ellipse);

    /* TextGraphic */
    TextGraphic* textgraphic = comp->GetText();
    int h = textgraphic->GetLineHeight();
    out << " :text " << h << ",";

    if (reqlabel) {
        const char* text = textgraphic->GetOriginal();
        ParamList::output_text(out, text, 0);
    }
    else
        out << "\"\"";
    Transformation(out, "texttrans", textgraphic);

    /* TextGraphic 2 */
    TextGraphic* textgraphic2 = comp->GetText2();
    if (textgraphic2 /* && strlen(textgraphic2->GetOriginal())>0 */) {
      int h2 = textgraphic2->GetLineHeight();
      out << " :text2 " << h2 << ",";
      ParamList::output_text(out, textgraphic2->GetOriginal(), 0);
      Transformation(out, "texttrans2", textgraphic2);
    }

    /* Picture GS*/
    Picture* pic = (Picture*)comp->GetGraphic();
    FullGS(out);
    Annotation(out);
    OverlayScript::Attributes(out);
}


int PipeScript::ReadTextTransform2 (istream& in, void* addr1, void* addr2, void* addr3, void* addr4) {
    float a00, a01, a10, a11, a20, a21;
    char delim;
    Graphic* gs = *(Graphic**)addr1;
 
    ParamList::skip_space(in);
    in >> a00 >> delim >> a01 >> delim >> a10 >> delim >> a11 >> delim >> a20 >> delim >> a21;
    if (!in.good()) {
        return -1;
    }
    else {
        Transformer* t = new Transformer(a00, a01, a10, a11, a20, a21);
        Picture* pic = *(Picture**)addr1;
        Iterator i;
        pic->First(i);
        pic->Next(i);
        pic->Next(i);
	TextGraphic* text = (TextGraphic*)pic->GetGraphic(i);
        text->SetTransformer(t);
	Unref(t);
        return 0;
    }
}

boolean PipeScript::EmitGS(ostream& out, Clipboard* cb, boolean prevout) {
    PipeComp* comp = (PipeComp*) GetSubject();

    return OverlayScript::EmitGS(out, cb, prevout);
}

/*****************************************************************************/

ConnScript::ConnScript (ConnComp* subj) : EdgeScript(subj) { }
ClassId ConnScript::GetClassId () { return CONN_SCRIPT; }

boolean ConnScript::IsA (ClassId id) { 
    return CONN_SCRIPT == id || EdgeScript::IsA(id);
}

boolean ConnScript::Definition (ostream& out) {
    EdgeComp* comp = (EdgeComp*) GetSubject();
    ArrowLine* arrowline = comp->GetArrowLine();
    int start_node_index = -1;
    int end_node_index = -1;
    int start_subnode_index = -1;
    int end_subnode_index = -1;
    int start_srcflag = -1;
    int end_srcflag = -1;
    IndexNodes(start_node_index, end_node_index, start_subnode_index, end_subnode_index, start_srcflag, end_srcflag);

    Coord x0, y0, x1, y1;
    arrowline->GetOriginal(x0, y0, x1, y1);
    float arrow_scale = arrowline->ArrowScale();
    boolean head, tail;
    head = arrowline->Head();
    tail = arrowline->Tail();

    out << script_name() << "(";
    out << x0 << "," << y0 << "," << x1 << "," << y1;
    if (arrow_scale != 1 )
	out << " :arrowscale " << arrow_scale;
    if (head) 
	out << " :head";
    if (tail)
	out << " :tail";
    out << " :startnode " << start_node_index << " :endnode " << end_node_index;
    if (start_subnode_index != -1)
      out << " :startsubnode " << start_subnode_index;
    if (end_subnode_index != -1)
      out << " :endsubnode " << end_subnode_index;
    MinGS(out);
    Annotation(out);
    Attributes(out);
    out << ")";

    return out.good();
}

void ConnScript::IndexNodes(int &start, int &end, int &startsub, int& endsub, int& startsrcflag, int& endsrcflag) {
    TopoEdge* edge = ((EdgeComp*)_subject)->Edge();
    const TopoNode* node;
    if (node = edge->start_node())
	start = IndexNode((PipeComp*)node->value(), startsub, startsrcflag);
    if (node = edge->end_node())
	end  = IndexNode((PipeComp*)node->value(), endsub, endsrcflag);
    return;
}

int ConnScript::IndexNode (PipeComp *comp, int& subindex, int& srcflag) {
    GraphicComp* comps = (GraphicComp*)GetSubject()->GetParent();
    Iterator i;
    #if 0
    int index = -1;
    #endif
    subindex = -1;

    for (comps->First(i); !comps->Done(i); comps->Next(i)) {
	GraphicComp* tcomp = comps->GetComp(i);
	#if 0
	if (tcomp->IsA(NODE_COMP))
	    index++;
	#endif
	// onscreen node
        if (tcomp == comp)
	  return comp->index();
	// offscreen node
	if (tcomp->IsA(INVO_COMP)) {
	  subindex = ((InvoComp*)tcomp)->find_subpipe_index(comp, srcflag);
	  if (subindex!=-1) return ((NodeComp*)tcomp)->index();
	}
    }

    return -1;
}

/*****************************************************************************/

ArbiterScript::ArbiterScript (ArbiterComp* subj) : PipeScript(subj) { }
ClassId ArbiterScript::GetClassId () { return ARBITER_SCRIPT; }

boolean ArbiterScript::IsA (ClassId id) { 
    return ARBITER_SCRIPT == id || PipeScript::IsA(id);
}

/*****************************************************************************/

ForkScript::ForkScript (ForkComp* subj) : PipeScript(subj) { }
ClassId ForkScript::GetClassId () { return FORK_SCRIPT; }

boolean ForkScript::IsA (ClassId id) { 
    return FORK_SCRIPT == id || PipeScript::IsA(id);
}

/*****************************************************************************/

InvoScript::InvoScript (InvoComp* subj) : PipeScript(subj) { }
ClassId InvoScript::GetClassId () { return INVO_SCRIPT; }

boolean InvoScript::IsA (ClassId id) { 
    return INVO_SCRIPT == id || PipeScript::IsA(id);
}

boolean InvoScript::Definition (ostream& out) {
    out << script_name() << "(" ;
    Attributes(out);
    out << ")";
    return out.good();
}

void InvoScript::Attributes(ostream& out) {
    InvoComp* comp = (InvoComp*) GetSubject();

    /* graph */
    GraphComp* graph = comp->GetGraph();
    if (graph && graph->GetPathName())
        out << " :graph \"" << graph->GetPathName() << "\"";

    out << " :width " << comp->width();
    out << " :height " << comp->height();

    out << " :nsrc ";
    if (comp->nsrcsizes()==0) out << "0";
    else {
      for (int i=0; i<comp->nsrcsizes(); i++) {
	if (i!=0) out << ",";
	out << comp->srcsizes()[i];
      }
    }
    out << " :ndst ";
    if (comp->ndstsizes()==0) out << "0";
    else {
      for (int i=0; i<comp->ndstsizes(); i++) {
	if (i!=0) out << ",";
	out << comp->dstsizes()[i];
      }
    }
    out << " :funcname \"" << comp->funcname() << "\"";
    #if 0
    if (comp->rotate() != 0.0)
      out << " :rotate " << comp->rotate();
    #else
    if (!comp->nametrans().identity()) {
      out << " :nametrans ";
      float a00, a01, a10, a11, a20, a21;
      comp->nametrans().matrix(a00,a01,a10,a11,a20,a21);
      out << a00 << "," << a01 << ",";
      out << a10 << "," << a11 << ",";
      out << a20 << "," << a21;
    }
    #endif

    /* Picture GS*/
    Picture* pic = (Picture*)comp->GetGraphic();
    FullGS(out);
    Annotation(out);
    OverlayScript::Attributes(out);
}

int InvoScript::ReadTextRotate (istream& in, void* addr1, void* addr2, void* addr3, void* addr4) {
    int rotate;
 
    ParamList::skip_space(in);
    in >> rotate;
    if (!in.good()) {
        return -1;
    }
    else {
        Transformer* t = (Transformer*)addr1;
	// setup rotate transform for later use
	float a00, a01, a10, a11, a20, a21;
	a20 = a21 = 0.0;
	switch (rotate) {
	case 0:
	default:
	  a00 = 1.0; a01 = 0.0; a10 = 0.0; a11 = 1.0; break;
	case 90:
	case -270:
	  a00 = 0.0; a01 = 1.0; a10 = -1.0; a11 = 0.0; break;
	case 180:
	case -180:
	  a00 = -1.0; a01 = 0.0; a10 = 0.0; a11 = -1.0; break;
	case 270:
	case -90:
	  a00 = 0.0; a01 = -1.0; a10 = 1.0; a11 = 0.0; break;
	}
	Transformer newt(a00, a01, a10, a11, a20, a21);
	*t = newt;
        return 0;
    }
}


int InvoScript::ReadNameTransform (istream& in, void* addr1, void* addr2, void* addr3, void* addr4) {
    float a00, a01, a10, a11, a20, a21;
    char delim;
 
    ParamList::skip_space(in);
    in >> a00 >> delim >> a01 >> delim >> a10 >> delim >> a11 >> delim >> a20 >> delim >> a21;
    if (!in.good()) {
        return -1;
    }
    else {
        Transformer* t = (Transformer*)addr1;
	*t = new Transformer(a00, a01, a10, a11, a20, a21);
        return 0;
    }
}

/*****************************************************************************/

#if defined(HAVE_ACE)
DistantScript::DistantScript (DistantComp* subj) : PipeScript(subj) { }
ClassId DistantScript::GetClassId () { return DISTANT_SCRIPT; }

boolean DistantScript::IsA (ClassId id) { 
    return DISTANT_SCRIPT == id || PipeScript::IsA(id);
}

boolean DistantScript::Definition (ostream& out) {
    out << script_name() << "(" ;
    Attributes(out);
    out << ")";
    return out.good();
}

void DistantScript::Attributes(ostream& out) {
  PipeScript::Attributes(out);
}
#endif /* defined(HAVE_ACE) */

/*****************************************************************************/

IplIdrawScript::IplIdrawScript (IplIdrawComp* subj) : DrawIdrawScript(subj) 
{
}

ClassId IplIdrawScript::GetClassId () { return IPL_IDRAW_SCRIPT; }

boolean IplIdrawScript::IsA (ClassId id) { 
    return IPL_IDRAW_SCRIPT == id || DrawIdrawScript::IsA(id);
}


