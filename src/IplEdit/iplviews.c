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

#include <IplEdit/iplcmds.h>
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/ipleditor.h>
#include <IplEdit/iplviewer.h>
#include <IplEdit/iplviews.h>
#include <IplServ/iplcomps.h>

#include <GraphUnidraw/graphcmds.h>
#include <GraphUnidraw/graphkit.h>
#include <OverlayUnidraw/ovcatalog.h>
#include <UniIdraw/idarrows.h>
#include <Unidraw/Tools/grcomptool.h>
#include <Unidraw/Commands/edit.h>
#include <Unidraw/Commands/transforms.h>
#include <Unidraw/Components/gvupdater.h>
#include <Unidraw/Graphic/ellipses.h>
#include <Unidraw/Graphic/polygons.h>
#include <Unidraw/Graphic/picture.h>
#include <Unidraw/clipboard.h>
#include <Unidraw/manips.h>
#include <Unidraw/selection.h>
#include <Unidraw/statevars.h>
#include <Unidraw/unidraw.h>
#include <IVGlyph/observables.h>
#include <IVGlyph/stredit.h>
#include <IV-2_6/InterViews/painter.h>
#include <IV-2_6/InterViews/rubcurve.h>
#include <IV-2_6/InterViews/rubgroup.h>
#include <IV-2_6/InterViews/rubline.h>
#include <IV-2_6/InterViews/rubrect.h>
#include <InterViews/transformer.h>
#include <InterViews/window.h>
#include <TopoFace/topoedge.h>
#include <TopoFace/toponode.h>
#include <Attribute/paramlist.h>
#include <OS/math.h>
#include <iostream.h>
#include <strstream>
#include <streambuf>
using std::streambuf;


FullGraphic* ConnView::_cv_gs = nil;

/****************************************************************************/

IplEditor* IplIdrawView::_ed = nil;

IplIdrawView::IplIdrawView (IplIdrawComp* subj) : DrawIdrawView(subj) { 
}

ClassId IplIdrawView::GetClassId () { return IPL_IDRAW_VIEW; }

boolean IplIdrawView::IsA (ClassId id) {
    return IPL_IDRAW_VIEW == id || DrawIdrawView::IsA(id);
}


/*****************************************************************************/

PipeView::PipeView(PipeComp* comp) : NodeView(comp) {
  _textscale = ZoomAdjustment();
}

PipeView::~PipeView() {
}

ClassId PipeView::GetClassId() { return PIPE_VIEW; }

boolean PipeView::IsA(ClassId id) {
    return id == PIPE_VIEW || NodeView::IsA(id);
}

Manipulator* PipeView::CreateManipulator(Viewer* v, Event& e, Transformer* rel,
					 Tool* tool)
{
    Rubberband* rub = nil;
    Manipulator* m = nil;
    Coord l, r, b, t;
    Editor* ed = v->GetEditor();
    int tabWidth = Math::round(.5*ivinch);

    if (tool->IsA(GRAPHIC_COMP_TOOL)) {
	if (!((NodeComp*)GetGraphicComp())->RequireLabel()) {
	    v->Constrain(e.x, e.y);
	    m = new DragManip(v, nil, rel, tool, DragConstraint(XFixed | YFixed));
	}
	else {
            ((OverlayEditor*)v->GetEditor())->MouseDocObservable()->textvalue(GraphKit::mouse_labl);
	    FontVar* fontVar = (FontVar*) ed->GetState("FontVar");
	    ColorVar* colVar = (ColorVar*) ed->GetState("ColorVar");
	    PSFont* font = (fontVar == nil) ? psstdfont : fontVar->GetFont();
	    PSColor* fg = (colVar == nil) ? psblack : colVar->GetFgColor();
	    int lineHt = font->GetLineHt();

	    Painter* painter = new Painter;
	    painter->FillBg(false);
	    painter->SetFont(font);
	    painter->SetColors(fg, nil);
	    painter->SetTransformer(rel);

	    m = new TextManip(v, painter, lineHt, tabWidth, tool);
	}

    } else {
        m = NodeView::CreateManipulator(v, e, rel, tool);
    }
    return m;
}


Command* PipeView::InterpretManipulator(Manipulator* m) {
    Tool* tool = m->GetTool();
    Command* cmd = nil;
    OverlayEditor* ed = (OverlayEditor*)m->GetViewer()->GetEditor();
    
    if (tool->IsA(RESHAPE_TOOL)) {
       Command* reshape_cmd = NodeView::InterpretManipulator(m);
       ReorientTextCmd* reorient_text_cmd = new ReorientTextCmd(ed);
       cmd = new MacroCmd(ed, reshape_cmd, reorient_text_cmd);
    } else {
      cmd = NodeView::InterpretManipulator(m);
    }
    return cmd;
}

void PipeView::Update() {
    Graphic* list = GetGraphic();
    IncurDamage(list);
    *list = *((PipeComp*)GetGraphicComp())->GetGraphic();

    SF_Ellipse* view_ellipse = GetEllipse();
    SF_Ellipse* comp_ellipse = ((PipeComp*)GetGraphicComp())->GetEllipse();
    *(Graphic*) view_ellipse = *comp_ellipse;
    Coord x0, y0;
    int r1, r2;
    comp_ellipse->GetOriginal(x0, y0, r1, r2);
    view_ellipse->SetOriginal(x0, y0, r1, r2);

    TextGraphic* view_text = GetText();
    TextGraphic* comp_text = ((PipeComp*)GetGraphicComp())->GetText();
    *(Graphic*)view_text = *(Graphic*)comp_text;
    view_text->SetFont(comp_text->GetFont());
    view_text->SetLineHeight(comp_text->GetLineHeight());
    view_text->SetOriginal(comp_text->GetOriginal());
    if (_textscale != 1.0) {
      float cx, cy;
      view_text->GetCenter(cx, cy);
      view_text->Scale(_textscale, _textscale, cx, cy);
    }

    TextGraphic* view_text2 = GetText2();
    TextGraphic* comp_text2 = ((PipeComp*)GetGraphicComp())->GetText2();
    if (comp_text2) {
      if (!view_text2) {
	view_text2 = new TextGraphic("", stdgraphic);
	GetGraphic()->Append(view_text2);
      }
      *(Graphic*)view_text2 = *(Graphic*)comp_text2;
      view_text2->SetFont(comp_text2->GetFont());
      view_text2->SetLineHeight(comp_text2->GetLineHeight());
      view_text2->SetOriginal(comp_text2->GetOriginal());
      if (_textscale != 1.0) {
	float cx, cy;
	view_text2->GetCenter(cx, cy);
	view_text2->Scale(_textscale, _textscale, cx, cy);
      }
    }

    if (((PipeComp*)GetGraphicComp())->GetGraph()) {
        SF_Ellipse* view_ellipse = GetEllipse2();
        SF_Ellipse* comp_ellipse = ((PipeComp*)GetGraphicComp())->GetEllipse2();
        *(Graphic*) view_ellipse = *comp_ellipse;
        Coord x0, y0;
        int r1, r2;
        comp_ellipse->GetOriginal(x0, y0, r1, r2);
        view_ellipse->SetOriginal(x0, y0, r1, r2);

        Iterator ci;
        Picture* comp_pic = (Picture*)((PipeComp*)GetGraphicComp())->GetGraphic();
        comp_pic->First(ci);
        comp_pic->Next(ci);
        comp_pic->Next(ci);
        comp_pic->Next(ci);

        Picture* pic = (Picture*)GetGraphic();
        Iterator vi;
        pic->First(vi);
        pic->Next(vi);
        pic->Next(vi);
        pic->Next(vi);

        ArrowLine* comp_a;
        ArrowLine* view_a;
        for (; !comp_pic->Done(ci); ) {
	    comp_a = (ArrowLine*)comp_pic->GetGraphic(ci);
            view_a = (ArrowLine*)pic->GetGraphic(vi);
            *(Graphic*) view_a = *comp_a;
            IntCoord y1, y2;
            comp_a->GetOriginal(x0, y0, y1, y2);
            view_a->SetOriginal(x0, y0, y1, y2);
	    comp_a->Hidden() ? view_a->Hide() : view_a->Show();
	    comp_a->Desensitized() ? view_a->Desensitize() : view_a->Sensitize();
            comp_pic->Next(ci);
            pic->Next(vi);
        }
    }
    IncurDamage(list);
    EraseHandles();
}

TextGraphic* PipeView::GetText2() {
    Picture* pic = (Picture*)GetGraphic();
    Iterator i;
    pic->First(i);
    pic->Next(i);
    pic->Next(i);
    if (((PipeComp*)GetGraphicComp())->GetGraph())
        pic->Next(i);
    return (TextGraphic*)pic->GetGraphic(i);
}    

float PipeView::ZoomAdjustment(float factor) {
  Iterator it;
  unidraw->First(it);
  Editor* ed  = unidraw->GetEditor(it);
  if (!ed) return 1.0;
  float mag = ed->GetViewer()->GetMagnification()*factor;
  if (mag < 1.0) {
    if (mag < 0.5) {
      if(mag < 0.25) {
	return 1.9*1.9*1.9;
      } else {
	return 1.9*1.9;
      }
    } else {
      return 1.9; 
    }
  } else if (mag > 1.0) {
    if (mag > 2.0) {
      if (mag > 4.0) {
	return .9*.9*.9;
      } else {
	return .9*.9;
      }
    } else {
      return .9;
    }
  } else {
    return 1.0;
  }
}

void PipeView::AdjustForZoom(float factor, Coord cx, Coord cy) {
#if 1
  _textscale = ZoomAdjustment(factor);
  GetGraphicComp()->Notify();
#else
#if 1
  float mag = GetViewer()->GetMagnification()*factor;
  if (mag==1.0) {
    _textscale = 1.0;
  } else {
    float zfactor;
    if (factor<1)
      zfactor = mag < 1 ? 1.9 : .9;
    else
      zfactor = mag < 1 ? 1.0/1.9 : 1.0/.9;
    _textscale *= zfactor;  
  }
  GetGraphicComp()->Notify();
#else
  if (!_zoomed) {
    _txt1trans = GetText()->GetTransformer();
    _txt2trans = GetText2()->GetTransformer();
    _zoomed = 1;
  }

  float mag = GetViewer()->GetMagnification()*factor;
  if (mag==1.0) {
    if (GetText()->GetTransformer())
      *GetText()->GetTransformer() = _txt1trans;
    if (GetText2()->GetTransformer())
      *GetText2()->GetTransformer() = _txt2trans;
    _zoomed = 0;
    } 

  else {
      
      float zfactor = 1.0/factor* (mag < 1 ? .95 : 1./.6);
      float gcx, gcy;
      GetText()->GetCenter(gcx, gcy);
      GetText()->Scale(zfactor, zfactor, gcx, gcy);
      GetText2()->GetCenter(gcx, gcy);
      GetText2()->Scale(zfactor, zfactor, gcx, gcy);
  }
#endif
#endif
}

/*****************************************************************************/

ConnView::ConnView(ConnComp* comp) : EdgeView(comp) {
}

ConnView::~ConnView() {
}

ClassId ConnView::GetClassId() { return CONN_VIEW; }

boolean ConnView::IsA(ClassId id) {
    return id == CONN_VIEW || EdgeView::IsA(id);
}

Command* ConnView::InterpretManipulator(Manipulator* m) {
    DragManip* dm = (DragManip*) m;
    OverlayEditor* ed = (OverlayEditor*)dm->GetViewer()->GetEditor();
    OverlaysView* views = ed->GetFrame();
    Tool* tool = dm->GetTool();
    Transformer* rel = dm->GetTransformer();
    Command* cmd = nil;
    Viewer* v = GetViewer();
    float mag = dm->GetViewer()->GetMagnification();

    if (tool->IsA(GRAPHIC_COMP_TOOL)) {
        RubberLine* rl = (RubberLine*) dm->GetRubberband();
        Coord x0, y0, x1, y1;
        rl->GetCurrent(x0, y0, x1, y1);

        if (x0 != x1 || y0 != y1) {
            BrushVar* brVar = (BrushVar*) ed->GetState("BrushVar");
            ColorVar* colVar = (ColorVar*) ed->GetState("ColorVar");
	    
	    PipeView* gv0 = nil;
	    PipeView* gv1 = nil;
	    Iterator i;
	    Selection* s0 = views->ViewsContaining(x0, y0);
	    for (s0->Last(i); !s0->Done(i); s0->Prev(i)) {
		if (s0->GetView(i)->IsA(PIPE_VIEW)) {
		    gv0 = (PipeView*)s0->GetView(i);
		    break;
		}
	    }
	    Selection* s1 = views->ViewsContaining(x1, y1);
	    for (s1->Last(i); !s1->Done(i); s1->Prev(i)) {
		if (s1->GetView(i)->IsA(PIPE_VIEW)) {
		    gv1 = (PipeView*)s1->GetView(i);
		    break;
		}
	    }
            ArrowLine* pg = ((EdgeComp*)GetGraphicComp())->GetArrowLine();

            int start_subpipe = -1;
            int end_subpipe = -1;
	    PipeComp* start_subcomp = nil;
	    PipeComp* end_subcomp = nil;
	    if (gv0 && gv0->IsA(INVO_VIEW)) {
   	        PointObj pt0(x0, y0);
	        Graphic* subgr0 = ((Picture*)gv0->GetGraphic())->LastGraphicContaining(pt0);
	        if (subgr0 && subgr0->CompId() == GRAPHIC_COMPS && ((InvoView*)gv0)->dstgraphics()==subgr0) {

		    Graphic* subsubgr0 = subgr0->LastGraphicContaining(pt0);
		    if (subsubgr0 && subsubgr0->CompId() == POLYGON_COMP) {

		      int index0 = ((InvoView*)gv0)->subpipeindex((Picture*)subgr0, (SF_Polygon*)subsubgr0);
		      PipeComp* subpipecomp = ((InvoComp*)gv0->GetGraphicComp())->find_subpipe(false, index0);
		      if (!subpipecomp || (subpipecomp && (subpipecomp->Node() == nil)))
		        return cmd;
 		      start_subpipe = index0;
		      start_subcomp = subpipecomp;

		    }
	        }
	    }

	    if (gv1 && gv1->IsA(INVO_VIEW)) {
   	        PointObj pt1(x1, y1);
	        Graphic* subgr1 = ((Picture*)gv1->GetGraphic())->LastGraphicContaining(pt1);
	        if (subgr1 && subgr1->CompId() == GRAPHIC_COMPS && ((InvoView*)gv1)->srcgraphics()==subgr1) {

		    Graphic* subsubgr1 = subgr1->LastGraphicContaining(pt1);
		    if (subsubgr1 && subsubgr1->CompId() == POLYGON_COMP) {

		      int index1 = ((InvoView*)gv1)->subpipeindex((Picture*)subgr1, (SF_Polygon*)subsubgr1);
		      PipeComp* subpipecomp = ((InvoComp*)gv1->GetGraphicComp())->find_subpipe(true, index1);
		      if (!subpipecomp || (subpipecomp && (subpipecomp->Node() == nil)))
		        return cmd;
		      end_subpipe = index1;
		      end_subcomp = subpipecomp;

		    }
	        }
	    }

	    if (rel) {
		rel->InvTransform(x0, y0);
		rel->InvTransform(x1, y1);
	    }
            ArrowLine* line = new ArrowLine(x0, y0, x1, y1, false, gv1 ? true : false, 1.5, pg);
            if (brVar != nil) 
		line->SetBrush(brVar->GetBrush());
            if (colVar != nil) {
	        line->FillBg(!colVar->GetBgColor()->None());
                line->SetColors(colVar->GetFgColor(), colVar->GetBgColor());
	    }

	    // fprintf(stderr, "start_subpipe %d, end_subpipe %d\n", start_subpipe, end_subpipe);
	    EdgeComp* newedge = NewEdgeComp(line, nil, start_subpipe, end_subpipe);

	    NodeComp* comp0 = start_subcomp 
	      ? start_subcomp : (gv0 ? (NodeComp*)gv0->GetGraphicComp() : nil);
	    NodeComp* comp1 = end_subcomp 
	      ? end_subcomp : (gv1 ? (NodeComp*)gv1->GetGraphicComp() : nil);

	    FullGraphic* stdtext = new FullGraphic(stdgraphic);
	    FontVar* fontVar = (FontVar*) m->GetViewer()->GetEditor()->GetState("FontVar");
	    PSFont* font = (fontVar == nil) ? psstdfont : fontVar->GetFont();
	    stdtext->SetFont(font);

	    if (!comp0) {
	      SF_Ellipse* ell = new SF_Ellipse(x0, y0, 3, 3, stdgraphic);
	      TextGraphic* txt = new TextGraphic("", stdtext);
	      TextGraphic* txt2 = new TextGraphic("", stdtext);
	      comp0 = new ForkComp(ell, txt, txt2);
	    }
	    if (!comp1) {
	      SF_Ellipse* ell = new SF_Ellipse(x1, y1, 3, 3, stdgraphic);
	      TextGraphic* txt = new TextGraphic("", stdtext);
	      TextGraphic* txt2 = new TextGraphic("", stdtext);
	      comp1 = new ForkComp(ell, txt, txt2);
	    }

	    // build up Clipboard for pasting
	    Clipboard* cb = new Clipboard(newedge);
	    if (!gv0) cb->Append(comp0);
	    if (!gv1) cb->Append(comp1);

	    // build up MacroCmd
	    cmd = new MacroCmd(ed);
	    ((MacroCmd*)cmd)->Append(new PasteCmd(ed, cb));
	    ((MacroCmd*)cmd)->Append(new EdgeConnectCmd(ed, newedge, comp0, comp1));
	    
        }
    }
    else {
        cmd = EdgeView::InterpretManipulator(m);
    }
    return cmd;
}

Graphic* ConnView::HighlightGraphic() {
    if (!_cv_gs) {
	Catalog* catalog = unidraw->GetCatalog();
	_cv_gs = new FullGraphic();
	_cv_gs->SetBrush(catalog->FindBrush(0xffff, 2));
	// _cv_gs->SetColors(catalog->FindColor("red"), catalog->FindColor("red"));
    }

    return _cv_gs;
}

/*****************************************************************************/

ArbiterView::ArbiterView(ArbiterComp* comp) : PipeView(comp) {
}

ArbiterView::~ArbiterView() {
}

ClassId ArbiterView::GetClassId() { return ARBITER_VIEW; }

boolean ArbiterView::IsA(ClassId id) {
    return id == ARBITER_VIEW || PipeView::IsA(id);
}

/*****************************************************************************/

ForkView::ForkView(ForkComp* comp) : PipeView(comp) {
}

ForkView::~ForkView() {
}

ClassId ForkView::GetClassId() { return FORK_VIEW; }

boolean ForkView::IsA(ClassId id) {
    return id == FORK_VIEW || PipeView::IsA(id);
}

/*****************************************************************************/

InvoView::InvoView(InvoComp* comp) : PipeView(comp) {
}

InvoView::~InvoView() {
}

ClassId InvoView::GetClassId() { return INVO_VIEW; }

boolean InvoView::IsA(ClassId id) {
    return id == INVO_VIEW || PipeView::IsA(id);
}

Command* InvoView::InterpretManipulator(Manipulator* m) {
    Tool* tool = m->GetTool();
    Command* cmd = nil;

    if (tool->IsA(GRAPHIC_COMP_TOOL)) {

        boolean status = 0;
	char namebuff[BUFSIZ];
	int* srcsizes;
	int nsrcsizes;
	int* dstsizes;
	int ndstsizes;
	
        do {
	  const char* samplestr = "func #inputs #outputs";
	  char* funcstr = 
	    StrEditDialog::post(m->GetViewer()->GetEditor()->GetWindow(),
				status==-1 ? "Input Error:  Re-enter func #inputs #outputs" : "Enter func #inputs #outputs:",
				samplestr);
	  if (!funcstr || strcmp(funcstr, samplestr)==0) return nil;
	  std::strstreambuf sbuf(funcstr, strlen(funcstr));
	  istream ins(&sbuf);
	  
	  status = ParamList::parse_token(ins, namebuff, BUFSIZ, " ");
	  if (ins.eof()) { status = -1; continue; }
	  if (ins.good()&&status==0) status = ParamList::read_ints(ins, &srcsizes, &nsrcsizes, nil, nil);
	  if (ins.eof()) { status = -1; continue; }
	  if (ins.good()&&status==0) status = ParamList::read_ints(ins, &dstsizes, &ndstsizes, nil, nil);
	  status = (status==0 && (ins.good()||ins.eof())) ? 0 : -1;

	} while (status==-1);
	
        Picture* proto_pict = (Picture*)(GetGraphicComp())->GetGraphic();
	Picture* new_pict = new Picture(proto_pict);
	
	DragManip* dm = (DragManip*) m;
	Editor* ed = dm->GetViewer()->GetEditor();
	Transformer* rel = dm->GetTransformer();
	Event initial = dm->GraspEvent();
	Coord xpos = initial.x;
	Coord ypos = initial.y;
	if (rel != nil)
	  rel->InvTransform(xpos, ypos);
	new_pict->SetTransformer(nil);
	new_pict->Translate(xpos, ypos);
	
	FontVar* fontVar = (FontVar*) ed->GetState("FontVar");
	PSFont* font = (fontVar == nil) ? psstdfont : fontVar->GetFont();
	new_pict->SetFont(font);            
	
	BrushVar* brVar = (BrushVar*) ed->GetState("BrushVar");
	PatternVar* patVar = (PatternVar*) ed->GetState("PatternVar");
	ColorVar* colVar = (ColorVar*) ed->GetState("ColorVar");
	if (brVar != nil) new_pict->SetBrush(brVar->GetBrush());
	if (patVar != nil && patVar->GetPattern()->None())
	  new_pict->SetPattern(unidraw->GetCatalog()->ReadPattern("pattern",3));
	else if (patVar != nil)
	  new_pict->SetPattern(patVar->GetPattern());
	if (colVar != nil) {
	  new_pict->FillBg(!colVar->GetBgColor()->None());
	  new_pict->SetColors(colVar->GetFgColor(), colVar->GetBgColor());
	}
	
	InvoComp* invocomp = new InvoComp(new_pict);

	invocomp->funcname(namebuff);
	invocomp->srcsizes(srcsizes, nsrcsizes);
	invocomp->dstsizes(dstsizes, ndstsizes);
	delete srcsizes;
	delete dstsizes;
	invocomp->build_invocation();
	invocomp->height(Math::max(64, 
				   Math::max(invocomp->nsrc(), 
					     invocomp->ndst())
				   *invocomp->triangle_height()));
	invocomp->build_invocation_graphic();

	cmd = new PasteCmd(ed, new Clipboard(invocomp));

    } else if (tool->IsA(MOVE_TOOL)) {
	DragManip* dm = (DragManip*) m;
	Editor* ed = dm->GetViewer()->GetEditor();
	Transformer* rel = dm->GetTransformer();
        SlidingRect* sr = (SlidingRect*) ((RubberGroup*)dm->GetRubberband())->First();
        Coord x0_orig, y0_orig, x1_orig, y1_orig;
	Coord x0_curr, y0_curr, x1_curr, y1_curr;
        float fx_curr, fy_curr, fx_orig, fy_orig;

        sr->GetOriginal(x0_orig, y0_orig, x1_orig, y1_orig);
        sr->GetCurrent(x0_curr, y0_curr, x1_curr, y1_curr);
        if (rel != nil) {
            rel->InvTransform(float(x0_orig), float(y0_orig), fx_orig, fy_orig);
            rel->InvTransform(float(x0_curr), float(y0_curr), fx_curr, fy_curr);
        }
        cmd = new MoveCmd(ed, fx_curr-fx_orig, fy_curr-fy_orig);
    } else if (tool->IsA(RESHAPE_TOOL)) {
      #if 0
        TextManip* tm = (TextManip*) m;
        int size;
        const char* text = tm->GetText(size);
	tm->GetViewer()->Update();

	Coord xpos, ypos;
	tm->GetPosition(xpos, ypos);
	Painter* p = tm->GetPainter();
	Transformer* rel = tm->GetPainter()->GetTransformer();
	int lineHt = tm->GetLineHeight();
	
	FullGraphic* pg = new FullGraphic(stdgraphic);
	FontVar* fontVar = (FontVar*) tm->GetViewer()->GetEditor()->GetState("FontVar");
	PSFont* font = (fontVar == nil) ? psstdfont : fontVar->GetFont();
	pg->SetFont(font);
	TextGraphic* textgr = new TextGraphic(text, lineHt, pg);
	textgr->SetTransformer(nil);
	((NodeComp*)GetGraphicComp())->GetEllipse()->Align(Center, textgr, Center);
	textgr->SetFont((PSFont*) p->GetFont());
//	textgr->SetColors((PSColor*) p->GetFgColor(), nil);
	
	cmd = new NodeTextCmd(tm->GetViewer()->GetEditor(),
			      (NodeComp*)GetGraphicComp(),
			      textgr);
	#endif
    } else {
        cmd = PipeView::InterpretManipulator(m);
    }
    return cmd;
}

void InvoView::Update() {
    Graphic* list = GetGraphic();
    IncurDamage(list);
    *list = *((PipeComp*)GetGraphicComp())->GetGraphic();

    SF_Rect* view_rect = GetRect();
    SF_Rect* comp_rect = ((InvoComp*)GetGraphicComp())->GetRect();
    *(Graphic*) view_rect = *comp_rect;
    Coord x0, y0, x1, y1;
    comp_rect->GetOriginal(x0, y0, x1, y1);
    view_rect->SetOriginal(x0, y0, x1, y1);

    TextGraphic* view_functext = GetFuncText();
    TextGraphic* comp_functext = ((InvoComp*)GetGraphicComp())->GetFuncText();
    *(Graphic*)view_functext = *(Graphic*)comp_functext;
    view_functext->SetFont(comp_functext->GetFont());
    view_functext->SetLineHeight(comp_functext->GetLineHeight());
    view_functext->SetOriginal(comp_functext->GetOriginal());
    if (_textscale != 1.0) {
      float cx, cy;
      view_functext->GetCenter(cx, cy);
      view_functext->Scale(_textscale, _textscale, cx, cy);
      }

    TextGraphic* view_srctext = GetSrcText();
    TextGraphic* comp_srctext = ((InvoComp*)GetGraphicComp())->GetSrcText();
    *(Graphic*)view_srctext = *(Graphic*)comp_srctext;
    view_srctext->SetFont(comp_srctext->GetFont());
    view_srctext->SetLineHeight(comp_srctext->GetLineHeight());
    view_srctext->SetOriginal(comp_srctext->GetOriginal());
    if (_textscale != 1.0) {
      float cx, cy;
      view_srctext->GetCenter(cx, cy);
      view_srctext->Scale(_textscale, _textscale, cx, cy);
      }

    TextGraphic* view_dsttext = GetDstText();
    TextGraphic* comp_dsttext = ((InvoComp*)GetGraphicComp())->GetDstText();
    *(Graphic*)view_dsttext = *(Graphic*)comp_dsttext;
    view_dsttext->SetFont(comp_dsttext->GetFont());
    view_dsttext->SetLineHeight(comp_dsttext->GetLineHeight());
    view_dsttext->SetOriginal(comp_dsttext->GetOriginal());
    if (_textscale != 1.0) {
      float cx, cy;
      view_dsttext->GetCenter(cx, cy);
      view_dsttext->Scale(_textscale, _textscale, cx, cy);
      }

    Picture* view_srctris = GetSrcPict();
    Picture* comp_srctris = ((InvoComp*)GetGraphicComp())->GetSrcPict();
    *(Graphic*)view_srctris = *(Graphic*)comp_srctris;

    Picture* view_dsttris = GetDstPict();
    Picture* comp_dsttris = ((InvoComp*)GetGraphicComp())->GetDstPict();
    *(Graphic*)view_dsttris = *(Graphic*)comp_dsttris;

    Picture* view_sprites = GetSpritePict();
    Picture* comp_sprites = ((InvoComp*)GetGraphicComp())->GetSpritePict();
    *(Graphic*)view_sprites = *(Graphic*)comp_sprites;
    Iterator it, jt;
    view_sprites->First(it);
    comp_sprites->First(jt);
    while(!view_sprites->Done(it)) {
      TextGraphic* view_text = (TextGraphic*)view_sprites->GetGraphic(it);
      TextGraphic* comp_text = (TextGraphic*)comp_sprites->GetGraphic(jt);
      *(Graphic*)view_text = *(Graphic*)comp_text;
      view_text->SetFont(comp_text->GetFont());
      view_text->SetLineHeight(comp_text->GetLineHeight());
      view_text->SetOriginal(comp_text->GetOriginal());
      if (_textscale != 1.0) {
	float cx, cy;
	view_text->GetCenter(cx, cy);
	view_text->Scale(_textscale, _textscale, cx, cy);
      }
      view_sprites->Next(it);
      comp_sprites->Next(jt);
    }

    IncurDamage(list);
    EraseHandles();
}

Manipulator* InvoView::CreateManipulator(Viewer* v, Event& e, Transformer* rel,
					 Tool* tool)
{
    Rubberband* rub = nil;
    Manipulator* m = nil;
    Coord l, r, b, t;
    Editor* ed = v->GetEditor();
    int tabWidth = Math::round(.5*ivinch);

    if (tool->IsA(MOVE_TOOL)) {
	RubberGroup* rubgroup = new RubberGroup(nil,nil);
        v->Constrain(e.x, e.y);
        rub = MakeRubberband(e.x, e.y);
	rubgroup->Append(rub);

	InvoPipesComp* invopipes = ((InvoComp*)GetGraphicComp())->invopipes();
	if (invopipes) {
	  PipesComp* srccomp = invopipes->srccomp();
	  PipesComp* dstcomp = invopipes->dstcomp();
	  Iterator i;
	  
#if !defined(FLAT_ARGLIST)
	  OverlayComp* before = nil;
	  OverlayComp* treenode = srccomp;
	  while (treenode && treenode != srccomp->GetParent()) {
	    OverlayComp* after = treenode->DepthNext(before);
	    before = treenode;
	    treenode = after;
	    TopoNode* node = nil;
	    if (treenode && treenode->IsA(PIPE_COMP))
	      node = ((PipeComp*)treenode)->Node();
	    else
	      continue;
#else
	  Iterator j;
	  for (srccomp->First(j); !srccomp->Done(j); srccomp->Next(j)) {
	    TopoNode* node = ((NodeComp*)srccomp->GetComp(j))->Node();
#endif
	    for (node->first(i); !node->done(i); node->next(i)) {
	      TopoEdge* edge = node->edge(node->elem(i));
	      EdgeView* edgeview = ((EdgeComp*)edge->value())->GetEdgeView(GetViewer());
	      if (!edgeview) continue;
	      int x0, y0, x1, y1;
	      if (edge->end_node() == node)
		edgeview->GetArrowLine()-> GetOriginal(x0, y0, x1, y1);
	      else
		edgeview->GetArrowLine()->GetOriginal(x1, y1, x0, y0);
	      Transformer trans;
	      edgeview->GetArrowLine()->TotalTransformation(trans);
	      trans.Transform(x0, y0);
	      trans.Transform(x1, y1);
	      RubberLine* rubline = new RubberLine(nil, nil, x0-(x1-e.x), y0-(y1-e.y), x1, y1,
						   x1 - e.x, y1 - e.y);
	      rubgroup->Append(rubline);
	    }
	  }
	  
#if !defined(FLAT_ARGLIST)
	  treenode = dstcomp;
	  before = nil;
	  while (treenode && treenode != dstcomp->GetParent()) {
	    OverlayComp* after = treenode->DepthNext(before);
	    before = treenode;
	    treenode = after;
	    TopoNode* node = nil;
	    if (treenode && treenode->IsA(PIPE_COMP))
	      node = ((PipeComp*)treenode)->Node();
	    else
	      continue;
#else
          for (dstcomp->First(j); !dstcomp->Done(j); dstcomp->Next(j)) {
	    TopoNode* node = ((NodeComp*)dstcomp->GetComp(j))->Node();
#endif
	    for (node->first(i); !node->done(i); node->next(i)) {
	      TopoEdge* edge = node->edge(node->elem(i));
	      EdgeView* edgeview = ((EdgeComp*)edge->value())->GetEdgeView(GetViewer());
	      if (!edgeview) continue;
	      int x0, y0, x1, y1;
	      if (edge->end_node() == node)
		edgeview->GetArrowLine()->GetOriginal(x0, y0, x1, y1);
	      else
		edgeview->GetArrowLine()->GetOriginal(x1, y1, x0, y0);
	      Transformer trans;
	      edgeview->GetArrowLine()->TotalTransformation(trans);
	      trans.Transform(x0, y0);
	      trans.Transform(x1, y1);
	      RubberLine* rubline = new RubberLine(nil, nil, x0-(x1-e.x), y0-(y1-e.y), x1, y1,
						   x1 - e.x, y1 - e.y);
	      rubgroup->Append(rubline);
	    }
	  }
	}
	    
        m = new DragManip(
	    v, rubgroup, rel, tool, DragConstraint(HorizOrVert | Gravity)
	);
    } else if (tool->IsA(RESHAPE_TOOL)) {
#if 0
        TextGraphic* textgr = GetText();
        FontVar* fontVar = (FontVar*) v->GetEditor()->GetState("FontVar");
        PSFont* font = (fontVar == nil) ? psstdfont : fontVar->GetFont();
        Painter* painter = new Painter;
        int lineHt = textgr->GetLineHeight();
        Coord xpos, ypos;
        rel = new Transformer;
        const char* text = textgr->GetOriginal();
        int size = strlen(text);
        
        textgr->TotalTransformation(*rel);
	if (size == 0)
	    rel->Transform(0, lineHt/2, xpos, ypos);
	else
	    rel->Transform(0, 0, xpos, ypos);
        painter->SetFont(textgr->GetFont() ? textgr->GetFont() : font);
	painter->SetColors(textgr->GetFgColor(), nil);
        painter->SetTransformer(rel);
        Unref(rel);
	int tabWidth = Math::round(.5*ivinch);

        m = new TextManip(
            v, text, size, xpos, ypos, painter, lineHt, tabWidth, tool
        );
#endif
    } else {
        m = PipeView::CreateManipulator(v, e, rel, tool);
    }
    return m;
}

int InvoView::subpipeindex(Picture* pic, SF_Polygon* polygon) {
    int index = 0;
    SF_Polygon* poly;
    Iterator i;

    for (pic->First(i);!pic->Done(i); pic->Next(i)) {
        poly = (SF_Polygon*)pic->GetGraphic(i);
	if (poly == polygon)
	    return index;
	index++;
    }
    
    return -1;
}



Picture* InvoView::srcgraphics() 
{
  Picture* toppict = (Picture*)GetGraphic();
  Iterator it;
  toppict->First(it);
  for (int i=0; i<4; i++) toppict->Next(it);
  return (Picture*)toppict->GetGraphic(it);
}

Picture* InvoView::dstgraphics() 
{
  Picture* toppict = (Picture*)GetGraphic();
  Iterator it;
  toppict->First(it);
  for (int i=0; i<5; i++) toppict->Next(it);
  return (Picture*)toppict->GetGraphic(it);
}

SF_Rect* InvoView::GetRect() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator i;
  pic->First(i);
  return (SF_Rect*)pic->GetGraphic(i);
}

TextGraphic* InvoView::GetFuncText() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  return (TextGraphic*)pic->GetGraphic(it);
}

TextGraphic* InvoView::GetSrcText() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  return (TextGraphic*)pic->GetGraphic(it);
}

TextGraphic* InvoView::GetDstText() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  return (TextGraphic*)pic->GetGraphic(it);
}

Picture* InvoView::GetSrcPict() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  return (Picture*)pic->GetGraphic(it);
}

Picture* InvoView::GetDstPict() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  return (Picture*)pic->GetGraphic(it);
}

Picture* InvoView::GetSpritePict() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  return (Picture*)pic->GetGraphic(it);
}

Rubberband* InvoView::MakeRubberband(IntCoord x, IntCoord y) {
  Coord l, r, b, t;
  Viewer* v = GetViewer();
  GetRect()->GetBox(l, b, r, t);
  Rubberband* rub = new SlidingRect(nil, nil, l, b, r, t, x, y);
  return rub;
}

/*****************************************************************************/

#if defined(HAVE_ACE)

DistantView::DistantView(DistantComp* comp) : PipeView(comp) {
}

DistantView::~DistantView() {
}

ClassId DistantView::GetClassId() { return DISTANT_VIEW; }

boolean DistantView::IsA(ClassId id) {
    return id == DISTANT_VIEW || PipeView::IsA(id);
}

#endif /* defined(HAVE_ACE) */
