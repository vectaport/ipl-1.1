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

#include <IplServ/eventqueue.h>
#include <IplServ/iplclasses.h>
#include <IplServ/iplcomps.h>
#include <IplServ/iplscripts.h>
#include <IplServ/langfunc.h>
#include <IplServ/spritequeue.h>
#include <GraphUnidraw/graphcmds.h>
#include <ComUnidraw/grfunc.h>
#include <ComUnidraw/unifunc.h>
#include <OverlayUnidraw/leafwalker.h>
#include <OverlayUnidraw/ovcmds.h>
#include <OverlayUnidraw/ovselection.h>
#include <OverlayUnidraw/ovtext.h>
#include <OverlayUnidraw/ovunidraw.h>
#include <OverlayUnidraw/ovviewer.h>
#include <OverlayUnidraw/ovviews.h>
#include <OverlayUnidraw/paramlist.h>
#include <UniIdraw/idarrows.h>
#include <Unidraw/Commands/datas.h>
#include <Unidraw/Commands/dirty.h>
#include <Unidraw/Commands/transforms.h>
#include <Unidraw/Components/text.h>
#include <Unidraw/Graphic/picture.h>
#include <Unidraw/Graphic/polygons.h>
#include <Unidraw/Graphic/ellipses.h>
#include <Unidraw/catalog.h>
#include <Unidraw/iterator.h>
#include <ComTerp/comfunc.h>
#include <ComTerp/comterpserv.h>
#include <ComTerp/debugfunc.h>
#include <TopoFace/toponode.h>
#include <TopoFace/topoedge.h>
#include <Attribute/attrlist.h>
#include <Attribute/attrvalue.h>
#include <InterViews/transformer.h>
#include <Unidraw/statevars.h>
#include <IV-2_6/InterViews/perspective.h>
#include <OS/math.h>
#include <strstream>
#include <fstream.h>
#include <streambuf>
using std::streambuf;

#include <iostream>
#include <math.h>
#include <unistd.h>

using std::cout;
using std::cerr;

#define BLUER_ANIMATION
// #define ALL_BLACK
// #define INVO_TICK  // ticks the guts of an invocation
#define INVO_UNIT // treats invocation as single pipeline stage

PSColor* PipeComp::psblue = nil;
PSColor* PipeComp::psred = nil;
PSColor* PipeComp::psorange = nil;
PSColor* PipeComp::psgreen = nil;
PSColor* PipeComp::pspurple = nil;
boolean PipeComp::_singlestep = 0;
boolean PipeComp::_eventqueueflag = 0;
PipeEventQueue* PipeComp::_eventqueue = nil;
boolean ForkComp::_showforkvals = 0;
int PipeComp::_animateflag = 0;
int PipeComp::_unitaryflag = 1;

#if defined(MANUAL_BREAKPOINT)
int _debug0_func = 0;
InvoComp* _debug0_comp = nil;
#endif

// #define ARBITER_DEBUG
#ifdef ARBITER_DEBUG
ArbiterComp* _arbiter_debug = nil;
#endif


/****************************************************************************/

ParamList* ConnComp::_conn_params = nil;
int ConnComp::_symid = -1;

ConnComp::ConnComp(ArrowLine* g, OverlayComp* parent, int start_subedge,int end_subedge) 
  : EdgeComp(g, parent, start_subedge, end_subedge)
{
  init();
}

ConnComp::ConnComp(istream& in, OverlayComp* parent) 
  : EdgeComp(parent) 
{
  _start_subedge = _end_subedge = -1;
  _edge = new TopoEdge(this);
  _valid = GetParamList()->read_args(in, this);
  init();
}
    
ConnComp::ConnComp(OverlayComp* parent) : EdgeComp(parent) {
  init();
}

void ConnComp::init()
{
  _handout = 0;
}

ConnComp::~ConnComp() {
}

ClassId ConnComp::GetClassId () { return CONN_COMP; }

boolean ConnComp::IsA (ClassId id) {
    return CONN_COMP == id || EdgeComp::IsA(id);
}

ParamList* ConnComp::GetParamList() {
    if (!_conn_params) 
	GrowParamList(_conn_params = new ParamList());
    return _conn_params;
}

void ConnComp::GrowParamList(ParamList* pl) {
    pl->add_param("startsubnode", ParamStruct::keyword, &ParamList::read_int,
		  this, &_start_subedge);
    pl->add_param("endsubnode", ParamStruct::keyword, &ParamList::read_int,
		  this, &_end_subedge);
    EdgeComp::GrowParamList(pl);
    return;
}

void ConnComp::Interpret(Command* cmd) {
  if (cmd->IsA(EDGEUPDATE_CMD)) {
    UpdateArrowLine();
  }
  else
    EdgeComp::Interpret(cmd);
}

void ConnComp::handout(int flag) 
{ 
  _handout = flag; 
  
}

NodeComp* ConnComp::NodeStart() const {
  TopoEdge* edge = Edge();
  if (edge) {
    TopoNode* start = edge->start_node();
    if (start) {
      PipeComp* pipecomp = (PipeComp*)start->value();
      if (!pipecomp->onscreen())
	pipecomp = pipecomp->invocomp();
      return pipecomp;
    }
  }
  return nil;
}

NodeComp* ConnComp::NodeEnd() const {
  TopoEdge* edge = Edge();
  if (edge) {
    TopoNode* end = edge->end_node();
    if (end) {
      PipeComp* pipecomp = (PipeComp*)end->value();
      if (!pipecomp->onscreen())
	pipecomp = pipecomp->invocomp();
      return pipecomp;
    }
  }
  return nil;
}

void ConnComp::build_conn_graphic() {
  if (!Component::use_unidraw()) return;
  Iterator it;
  unidraw->First(it);
  Editor* editor = unidraw->GetEditor(it);
  BrushVar* brVar = (BrushVar*) editor->GetState("BrushVar");
  ColorVar* colVar = (ColorVar*) editor->GetState("ColorVar");

  int unit = 15;
  ArrowLine* aline = new ArrowLine(0, 0, unit, unit, false, true, 1., stdgraphic);
  if (brVar != nil) 
    aline->SetBrush(brVar->GetBrush());
  if (colVar != nil) {
    aline->FillBg(!colVar->GetBgColor()->None());
    aline->SetColors(colVar->GetFgColor(), colVar->GetBgColor());
  }
  aline->ScaleArrows(1.5);
  SetGraphic(aline);

  UpdateArrowLine();
}

void ConnComp::UpdateArrowLine() {
  if (Component::use_unidraw() && GetArrowLine()) {
    int x0, y0, x1, y1;
    GetArrowLine()->GetOriginal(x0, y0, x1, y1);
    GetArrowLine()->SetTransformer(new Transformer());
    
    if (Edge()->start_node()) {
      
      // standalone PipeComp
      if (!((NodeComp*)Edge()->start_node()->value())->GetParent()->IsA(PIPES_COMP)) {
	float fx, fy;
	((NodeComp*)Edge()->start_node()->value())
	  ->GetEllipse()->GetCenter(fx, fy);
	x0 = Math::round(fx);
	y0 = Math::round(fy);
      } 
      
      // PipeComp underneath InvoComp
      else {
	if (((NodeComp*)Edge()->start_node()->value())->IsA(PIPE_COMP)) {
	  PipeComp* pipecomp = (PipeComp*)Edge()->start_node()->value();
	  PipesComp* pipescomp = pipecomp->GetParent()->IsA(PIPES_COMP) 
	    ? (PipesComp*)pipecomp->GetParent() : nil;
	  if (pipescomp) {
	    int srcflag;
	    int id = GetStartSubNode() == -1 ? pipescomp->invocomp()->find_subpipe_index(pipecomp, srcflag) : GetStartSubNode();
	    pipescomp->invocomp()->get_triangle_loc(false /*src*/, id, x0, y0);
	  }
	}
      }
    }
    
    if (Edge()->end_node()) {
      
      // standalone PipeComp
      if (!((NodeComp*)Edge()->end_node()->value())->GetParent()->IsA(PIPES_COMP)) {
	float fx, fy;
	((NodeComp*)Edge()->end_node()->value())
	  ->GetEllipse()->GetCenter(fx, fy);
	x1 = Math::round(fx);
	y1 = Math::round(fy);
      } 
      
      // PipeComp underneath InvoComp
      else {
	if (((NodeComp*)Edge()->end_node()->value())->IsA(PIPE_COMP)) {
	  PipeComp* pipecomp = (PipeComp*)Edge()->end_node()->value();
	  PipesComp* pipescomp = pipecomp->GetParent()->IsA(PIPES_COMP) 
	    ? (PipesComp*)pipecomp->GetParent() : nil;
	  if (pipescomp) {
	    int srcflag;
	    int id = GetEndSubNode() == -1 ? pipescomp->invocomp()->find_subpipe_index(pipecomp, srcflag) : GetEndSubNode();
	    pipescomp->invocomp()->get_triangle_loc(true /*dst*/, id, x1, y1);
	  }
	}
      }
    }
    
    // trim arrowline for ellipses
    Coord nx0, ny0;
    TopoNode* startnode = Edge()->start_node();
    if (startnode) { 
      if ( !((NodeComp*)startnode->value())->IsA(PIPE_COMP) ||
	   ((PipeComp*)startnode->value())->onscreen()) {
	SF_Ellipse* e1 = nil;
	boolean newe = false;
	if (((NodeComp*)Edge()->start_node()->value())->GetClassId() != INVO_COMP)
	  e1 = ((NodeComp*)Edge()->start_node()->value())->GetEllipse();
	else {
	  /* this not really used because INVO_COMP arrowline adjustment done elsewhere */
	  int ex0, ey0, ex1, ey1;
	  ((NodeComp*)Edge()->start_node()->value())->GetGraphic()->
	    GetBox(ex0, ey0, ex1, ey1);
	  e1 = new SF_Ellipse(ex0+(ex1-ex0)/2, ey0+(ey1-ey0)/2,
			      (ex1-ex0)/2, (ey1-ey0)/2);
	  newe = true;
	}
	
	if (clipline(x0, y0, x1, y1, e1, false /* clip x0,y0 */,
		     nx0, ny0)) {
	  x0 = nx0;
	  y0 = ny0;
	}
	if (newe)
	  delete e1;
	
	((NodeComp*)Edge()->start_node()->value())->notify();
      }
    }
    
    Coord nx1, ny1;
    TopoNode* endnode = Edge()->end_node();
    if (endnode) {
      if (!((NodeComp*)endnode->value())->IsA(PIPE_COMP) ||
	  ((PipeComp*)endnode->value())->onscreen()) {
	SF_Ellipse* e2;
	boolean newe = false;
	if (((NodeComp*)Edge()->end_node()->value())->GetClassId() != INVO_COMP )
	  e2 = ((NodeComp*)Edge()->end_node()->value())->GetEllipse();
	else {
	  /* this not really used because INVO_COMP arrowline adjustment done elsewhere */
	  int ex0, ey0, ex1, ey1;
	  ((NodeComp*)Edge()->end_node()->value())->GetGraphic()->
	    GetBox(ex0, ey0, ex1, ey1);
	  e2 = new SF_Ellipse(ex0+(ex1-ex0)/2, ey0+(ey1-ey0)/2,
			      (ex1-ex0)/2, (ey1-ey0)/2);
	  newe = true;
	}
	if (clipline(x0, y0, x1, y1, e2, true /* clip x1, y1 */,
		     nx1, ny1)) {
	  x1 = nx1;
	  y1 = ny1;
	}
	if (newe)
	  delete e2;
	
	((NodeComp*)Edge()->end_node()->value())->notify();
      }
    }
    
    GetArrowLine()->SetOriginal(x0, y0, x1, y1);
    NotifyLater();
  }
}

/****************************************************************************/

ParamList* PipeComp::_pipe_params = nil;
int PipeComp::_symid = -1;

PipeComp::PipeComp(SF_Ellipse* ellipse, TextGraphic* txt, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
    : NodeComp(ellipse, txt, rl, parent)
{
  init(txt, txt2);
}

PipeComp::PipeComp(SF_Ellipse* ellipse, TextGraphic* txt, SF_Ellipse* ellipse2, 
    GraphComp* graph, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
    : NodeComp(ellipse, txt, ellipse2, graph, rl, parent)
{
  init(txt, txt2);
}

PipeComp::PipeComp(Picture* pic, boolean rl, OverlayComp* parent) 
    : NodeComp(pic, rl, parent)
{
  init();
}

PipeComp::PipeComp(GraphComp* graph) 
    : NodeComp(graph)
{
  init();
}

PipeComp::PipeComp(OverlayComp* parent) : NodeComp( parent) 
{
  init();
}

PipeComp::PipeComp(istream& in, OverlayComp* parent) : NodeComp(parent)
{
  init();
  _graph = nil;
  _node = new TopoNode(this);
  Picture *pic = new Picture();
  SetGraphic(pic);
  _valid = GetParamList()->read_args(in, this);

  if (GetGraph())
    GraphGraphic();

  if(Component::use_unidraw() && GetEllipse() && GetText()) {
    GetGraphic()->concatGS(GetEllipse(), GetGraphic(), GetEllipse()); 
    GetGraphic()->concatGS(GetText(), GetGraphic(), GetText()); 
    if (!GetText2()) {
      GetGraphic()->Append(new TextGraphic("", stdgraphic));
      GetText2()->SetTransformer(nil);
      GetEllipse()->Align(TopCenter, GetText2(), BottomCenter);
    }
    if (GetText2())
      GetGraphic()->concatGS(GetText2(), GetGraphic(), GetText2()); 
    _namesym = symbol_add((char*)GetText()->GetOriginal());
  }

}
    
void PipeComp::init(TextGraphic* txt, TextGraphic* txt2)
{
  if (Component::use_unidraw()) {
#ifndef ALL_BLACK
    if (!psblue) psblue = unidraw->GetCatalog()->FindColor("Blue");
    if (!psred) psred = unidraw->GetCatalog()->FindColor("Red");
    if (!psorange) psorange = unidraw->GetCatalog()->FindColor("Orange");
    if (!psgreen) psgreen = unidraw->GetCatalog()->FindColor("Green");
    if (!pspurple) pspurple = unidraw->GetCatalog()->FindColor("Purple");
#else
    if (!psblue) psblue = unidraw->GetCatalog()->FindColor("Black");
    if (!psred) psred = unidraw->GetCatalog()->FindColor("Black");
    if (!psorange) psorange = unidraw->GetCatalog()->FindColor("Black");
    if (!psgreen) psgreen = unidraw->GetCatalog()->FindColor("Black");
    if (!pspurple) pspurple = unidraw->GetCatalog()->FindColor("Black");
#endif
  }

  _buffsize = 1;
  _buff = new AttributeValueList();
  _buff->ref();
  _handsout = 0;
  _text2_update_reserved = 0;

  // extract symid for default txt
  _namesym = txt ? symbol_add((char*)txt->GetOriginal()) : -1;
  init_global();

  // if this is really a PipeComp, makes sure it has a Text2
  if (Component::use_unidraw() && !txt2 && GetEllipse()) {
    txt2 = new TextGraphic("", stdgraphic);
    txt2->SetTransformer(nil);
    GetEllipse()->Align(TopCenter, txt2, BottomCenter);
  }

  if (Component::use_unidraw() && txt2) {
    GetGraphic()->Append(txt2);
    GetGraphic()->concatGS(GetText2(), GetGraphic(), GetText2()); 
    GetEllipse()->Align(1, txt2, 7);
  }

  _current_outconn_color = psblack;
  _dst_notified = 0;
  _src_requested = 0;
  _delay = -1;
  _put_cnt = 0;
  _put_history = nil;
  _already_animated = 0;
  _sprite = nil;
}

PipeComp::~PipeComp() {
  Unref(_buff);
  delete _put_history;
}

ClassId PipeComp::GetClassId () { return PIPE_COMP; }

boolean PipeComp::IsA (ClassId id) {
    return PIPE_COMP == id || NodeComp::IsA(id);
}

void PipeComp::init_global() {
  // use the TextGraphic of this node to create a global variable
  if (GetText()) {
    if (_namesym<0) _namesym = symbol_add((char*)GetText()->GetOriginal());
    ComValue *newval = new ComValue(new OverlayViewRef(this), virtual_classid());
    ComTerpServ* comterp = ((OverlayUnidraw*)unidraw)->comterp();
    comterp->globaltable()->insert(_namesym, newval);
  }
}

int PipeComp::buffsize() {
  return _buffsize;
}

void PipeComp::buffsize(int size) {
  if (buff()->Number()>size && size != -1)
    size = buff()->Number();
  _buffsize = size;
}

AttributeValueList* PipeComp::buff() {
  return _buff;
}

boolean PipeComp::src_vacant() {
  if (!_src_vacant()) return false;
  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe) 
    return parentpipe->src_vacant();
  else
    return true;
}

boolean PipeComp::_src_vacant() {  
  if(sprite()) return 0;
  return 
    (buff() && (buffsize() < 0 || buffsize() > buff()->Number())) ||
    (!buff() && buffsize() != 0);
}

boolean PipeComp::dst_occupied() {
  if (!_dst_occupied()) return false;
  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  boolean parentflag = !parentpipe || parentpipe->dst_occupied();
  return parentflag;
}

boolean PipeComp::_dst_occupied() {
#if defined(NOT_WILD_EXPERIMENT)
 return _handsout==0 && buff() && buff()->Number()>0;
#else
 return buff() && buff()->Number()>0;
#endif
}

boolean PipeComp::src_put(AttributeValue* av) {
  if (src_vacant()) {

    boolean txt2reservation = reserve_text2_update();

    buff()->Append(av);
    if(_put_history) {
      _put_history[_put_cnt%100] = DistantComp::cycle_count();
      _put_cnt++;
    }
    
    boolean updated = update_and_pause();
    if (txt2reservation) unreserve_text2_update();
    if (!updated && !already_animated() && (txt2reservation || animateflag())) {
      update_text2(); 
      already_animated(1);
    }
    if(txt2reservation) already_animated(0);

    return true;
  } else
    return false;
}

boolean PipeComp::dst_get(AttributeValue*& av) {
  if (outconn_exists()) {
    fprintf(stderr, "warning:  dst_get attempted on PipeComp with output ConnComp's\n");
    return false;
  }

  if (dst_occupied()) {
    _dst_get(av);
    return true;
  } else
    return false;
}

void PipeComp::_dst_get(AttributeValue*& av) {
  boolean txt2reservation = reserve_text2_update();
  
  Iterator it;
  buff()->First(it);
  av = buff()->GetAttrVal(it);
  if (av) {
    buff()->Remove(it);
    boolean updated = update_and_pause();
    if (buff()->Number()<buffsize() || buffsize() == -1)
      src_request();
    
    if (txt2reservation) unreserve_text2_update();
    if (!updated && !already_animated() && (txt2reservation || animateflag())) {
      update_text2(); 
      already_animated(1);
    }
    if(txt2reservation) already_animated(0);
  } else
    fprintf(stderr, "PipeComp::_dst_get  unexpected NULL AttributeValue\n");
}

boolean PipeComp::get_buff(int index, AttributeValue*& av) {
  Iterator it;
  buff()->First(it);
  for (int i=0; i<index && !buff()->Done(it); i++) buff()->Next(it);
  if (!buff()->Done(it)) {
    av = buff()->GetAttrVal(it);
    return true;
  } else
    return false;
}

void PipeComp::rdst_to_src(ConnComp* conn, PipeComp* rdst) {
  if (!src_vacant()) return;
  
  AttributeValue* av = rdst_cpy(rdst);

  rdst_clr_handshake(conn, rdst);

  boolean txt2reservation = !rdst->handsout() && rdst->reserve_text2_update();

  /* put value after handshake cleared but before set up for next value */
  boolean flag = src_put(av);

  /* if all handshakes cleared, remove remote value */
  rdst_clr_val(rdst);

  /* if all handshakes cleared, set up for next value */
  rdst_chk(rdst);

  if ((txt2reservation || animateflag()) /* ??? && rdst->handsout()==0 */) {
    if(txt2reservation) rdst->unreserve_text2_update();
    if(!rdst->already_animated()) rdst->update_text2();
    if(animateflag()&&!rdst->already_animated()) rdst->already_animated(1);
    if(txt2reservation) rdst->already_animated(0);
  }

  if (flag) dst_notify();
}

AttributeValue* PipeComp::rdst_cpy_clr(ConnComp* conn, PipeComp* rdst) {

  /* copy value from destination end of other pipe */
  AttributeValue* retval = rdst_cpy(rdst);

  /* clear handshake and remote value */
  rdst_clr_handshake(conn, rdst);
  rdst_clr_val(rdst);

  return retval;
}

AttributeValue* PipeComp::rdst_cpy(PipeComp* rdst) {

  /* copy value from destination end of other pipe */
  Iterator it;
  rdst->buff()->First(it);
  AttributeValue* av = rdst->buff()->GetAttrVal(it);
  AttributeValue* retval = new AttributeValue(av);
  return retval;
}

void PipeComp::rdst_clr_handshake(ConnComp* conn, PipeComp* rdst) { 

  /* clear handshake */
  conn->handout(0);
  rdst->handsout(rdst->handsout()-1);

}
  
void PipeComp::rdst_clr_val(PipeComp* rdst) { 

  /* if all handshakes cleared, set up for next value */
  if (!rdst->handsout()) {
    Iterator it;
    rdst->buff()->First(it);
    AttributeValue* av = rdst->buff()->GetAttrVal(it);
    rdst->buff()->Remove(it);
    rdst->update_and_pause();
    delete av;
  }
  
}

void PipeComp::rdst_chk(PipeComp* rdst) {

  /* if all handshakes cleared, set up for next value */
  if (!rdst->handsout()) {
    if (rdst->buff()->Number()>0)
      rdst->dst_notify();
    if (rdst->buff()->Number()<rdst->buffsize() || rdst->buffsize()==-1)
      rdst->src_request();
  }

}

void PipeComp::src_request() {
  if (_src_requested) return;

  if(!src_vacant()) return;

  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe) 
    parentpipe->src_request();
  else {
    if (!_eventqueueflag)
      _src_request();
    else {
      _src_requested = 1;
      if (!_eventqueue) _eventqueue = new PipeEventQueue();
      _eventqueue->append(new PipeEvent(this, false /* !notify_flag */));
    }
  }
}

void PipeComp::_src_request() {

  Iterator it;
  Node()->first(it);

  /* find single input connector */
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      if (edgecomp->handout()) {
	PipeComp* startnode = (PipeComp*) edge->start_node()->value();
	rdst_to_src(edgecomp, startnode);
      }
      break;
    }
    Node()->next(it);
  }
  /* done finding single input connector */

}

int PipeComp::_src_ready() {

  Iterator it;
  Node()->first(it);

  /* find single input connector */
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      return edgecomp->handout() && src_vacant();
    }
    Node()->next(it);
  }
  /* done finding single input connector */

  return 0;

}

void PipeComp::dst_notify() {
  if (_dst_notified) return;

  if (!dst_occupied()) return;

  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe) 
    parentpipe->dst_notify();
  else {
    if (!_eventqueueflag)
      _dst_notify();
    else {
      _dst_notified = 1;
      if (!_eventqueue) _eventqueue = new PipeEventQueue();
      _eventqueue->append(new PipeEvent(this, true /* notify_flag */));
    }
  }

}

void PipeComp::_dst_notify() {

  Iterator it;

  /* loop through all output connectors to setup handshake */
  Node()->first(it);
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      if (!edgecomp->handout()) {
	edgecomp->handout(true);
	++_handsout;
      }
    }
    Node()->next(it);
  }
  /* done looping through all output connectors to setup handshake */

  /* loop through all output connectors to move value if ready */
  Node()->first(it);
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      PipeComp* srccomp = (PipeComp*)edge->end_node()->value();
      srccomp->rdst_to_src(edgecomp, this);
    }
    Node()->next(it);
  }
  /* done looping through all output connectors to move value if ready */
  

}

int PipeComp::_dst_ready() {

  /* loop through all output connectors to see if a value would move */
  Iterator it;
  Node()->first(it);
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      PipeComp* srccomp = (PipeComp*)edge->end_node()->value();
      if(srccomp->IsA(ARBITER_COMP)) {
	if (!((ArbiterComp*)srccomp)->arbiter_vacant(this)) return false;
      } else if(!srccomp->src_vacant()) return false;
    }
    Node()->next(it);
  }
  /* done looping through all output connectors to see if a value would move */

  return true;

}

ParamList* PipeComp::GetParamList() {
    if (!_pipe_params) 
	GrowParamList(_pipe_params = new ParamList());
    return _pipe_params;
}

void PipeComp::GrowParamList(ParamList* pl) {
    pl->add_param("text2", ParamStruct::keyword, &NodeScript::ReadText,
		  this, &_gr);
    pl->add_param("texttrans2", ParamStruct::keyword, &PipeScript::ReadTextTransform2,
		  this, &_gr);
    NodeComp::GrowParamList(pl);
    return;
}

TextGraphic* PipeComp::GetText2() {
    Picture* pic = (Picture*)GetGraphic();
    Iterator i;
    pic->First(i);
    pic->Next(i);
    pic->Next(i);
    if (_graph)
        pic->Next(i);
    if (pic->Done(i))
	return nil;
    else
	return (TextGraphic*)pic->GetGraphic(i);
}

void PipeComp::ChangeText2(const char* str) {
#if 0
  TextGraphic* tg_new = new TextGraphic(str, stdgraphic);
  SetText2(tg_new);
#else
  GetText2()->SetOriginal(str);
  GetEllipse()->Align(1, GetText2(), 7);
  NotifyLater();
#endif
}

void PipeComp::MoveText2(float dx, float dy) {
  GetText2()->Translate(dx, dy);
  NotifyLater();
}

void PipeComp::MoveText2To(float locx, float locy) {
  float cx, cy;
  GetText2()->GetCenter(cx, cy);
  GetText2()->Translate(locx-cx, locy-cy);
  NotifyLater();
}

void PipeComp::SetText2(TextGraphic* tg_new) {
  TextGraphic* tg_old = (GetText2());
  Transformer t;
  if (tg_old) {
    if (tg_old->GetTransformer())
      t = *tg_old->GetTransformer();
    ((Picture*)GetGraphic())->Remove(tg_old);
    delete tg_old;
  }
  tg_new->SetTransformer(new Transformer(t));
  ((Picture*)GetGraphic())->Append(tg_new);
  GetEllipse()->Align(1, tg_new, 7);
  NotifyLater();
}

void PipeComp::update_text2() {
  if (Component::use_unidraw()) {

    if (GetGraphic()) {
      std::strstreambuf sbuf;
      ostream outs(&sbuf);
      outs << *buff() << '\0';
      char* sbufstr = sbuf.str();

      if(_animateflag && nsrc()) {

	if(strlen(sbufstr)>0) {

	  ChangeText2(sbufstr);

	  /* determine upstream location for start of animation */
	  int order;
	  float old_cx, old_cy;
	  ConnComp* upstream = srcconn(0);
	  ArrowLine* arrowline = upstream->GetArrowLine();
	  arrowline->GetHead(old_cx, old_cy);

	  /* determine downstream location for end of animation */
	  float new_cx, new_cy;
	  arrowline->GetTail(new_cx, new_cy);

	  /* recolor as needed */
	  ((Picture*)GetGraphic())->SetColors(nil, nil);
	  GetText2()->SetColors(psblack, pswhite);
	  GetEllipse()->SetColors(psblue, pswhite);
	  GetText()->SetColors(psblue, pswhite);
	  
	  /* (get ready to) step through the delta in a 100 pieces */
	  int nsteps = 100;
	  float dx = new_cx-old_cx;
	  float dy = new_cy-old_cy;
	  MoveText2To(old_cx, old_cy);
	  
	  /* step now if not using eventqueue */
	  if(!_eventqueueflag) {
	    for(int i=0; i<nsteps; i++) {
	      float percent = (float)(i+1)/nsteps;
	      float curx = old_cx+ dx * percent;
	      float cury = old_cy+ dy * percent;
	      MoveText2To(curx, cury);
	      unidraw->Update(true);
	      usleep(10000);
	    }
	  } 
	  
	/* otherwise save the interpolating info */
	  else {
	    SpriteQueue* spritequeue = SpriteQueue::instance();
	    sprite(new Sprite(this, old_cx, old_cy, new_cx-old_cx, new_cy-old_cy, false));
	    spritequeue->Append(sprite());
	  }

	} 

	/* setup to animate acknowledge */
	else {

	  if(eventqueueflag() && animateflag()==1) {

	    /* determine downstream and upstream locations */
	    /* for start and end of acknowledge animation */
	    float dnl, dnb, dnr, dnt;
	    GetEllipse()->GetBounds(dnl, dnb, dnr, dnt);
	    float old_cx = (dnl+dnr)/2;
	    float old_cy = (dnb+dnt)/2;

	    float upl, upb, upr, upt;
	    int order;
	    float new_cx, new_cy;
	    PipeComp* upstream = remote_srccomp(0, order, false);
	    if (!upstream->IsA(INVO_COMP)) {
	      upstream->GetEllipse()->GetBounds(upl, upb, upr, upt);
	      new_cx = (upl+upr)/2;
	      new_cy = upb;
	    } 
	    else {
	      int ncx, ncy;
	      ((InvoComp*)upstream)->get_triangle_loc(0, order, ncx, ncy);
	      new_cx = ncx+10;
	      new_cy = ncy;
	    }
	      
	    
	    /* trim a little off each end */
	    float dx = new_cx - old_cx;
	    float dy = new_cy - old_cy;
	    float trim = 24.0;
	    float dist = sqrt(dx*dx + dy*dy);
	    float trimy = dy*trim/dist;
	    float trimx = dx*trimy/dy;
	    old_cx += trimx;
	    old_cy += trimy;
	    new_cx -= trimx;
	    new_cy -= trimy;

	    /* (get ready to) step through the delta in a 100 pieces */
	    SpriteQueue* spritequeue = SpriteQueue::instance();
	    sprite(new Sprite(this, old_cx, old_cy, new_cx-old_cx, new_cy-old_cy, true /* acknowledgeflag */));
	    spritequeue->Append(sprite());
	    ChangeText2("@");
	    
	    ((Picture*)GetGraphic())->SetColors(nil, nil);
	    GetEllipse()->SetColors(eventqueueflag() ? psred : psblue, pswhite);
	    GetText()->SetColors(eventqueueflag() ? psred : psblue, pswhite);
	    GetText2()->SetColors(psgreen, pswhite);
	    NotifyLater();
	  } else 
	    ChangeText2("");
	}


      } else
	ChangeText2(sbufstr);
      
      if (!sprite()) {
	if (strlen(sbufstr)) {
	  if(_put_history) {
	    float pr= put_rate();
	    GetGraphic()->SetPattern(pssolid);
	    if(pr<.25) {
	      GetGraphic()->SetColors(psred, pswhite);
	      GetEllipse()->SetColors(psred, psred);
	    }
	    else if(pr<.49999) {
	      GetGraphic()->SetColors(psorange, pswhite);
	      GetEllipse()->SetColors(psorange, psorange);
	    }
	    else {
	      GetGraphic()->SetColors(psgreen, pswhite);
	      GetEllipse()->SetColors(psgreen, psgreen);
	    }
	  }
	  else 
	    GetGraphic()->SetColors(psred, pswhite);
	  color_outconn(psred);
	}
	else {
	  if (!animateflag() || !eventqueueflag()) {
	    if (!_put_history)
	      GetGraphic()->SetColors(psblue, pswhite);
	    color_outconn(psblue);
	  }
	}
	NotifyLater();
	unidraw->Update();
      }
      delete sbufstr;
      // fprintf(stderr, "text2 update --> %s\n", sbuf.str());
    } else {
      if (GetParent() && GetParent()->IsA(PIPES_COMP)) {
	PipesComp* parentpipes = (PipesComp*)GetParent();
	parentpipes->update_parent_text(nil, this);
      }
    }
  }
}

int PipeComp::update_and_pause() {
  if (!_singlestep) return 0;
  update_text2();
  unidraw->Update(true);
  if (_singlestep>1) return 1;

  ComTerpServ* comterp = ((OverlayUnidraw*)unidraw)->comterp();
  static UnidrawPauseFunc* _pausefunc = nil;
  if (!_pausefunc) {
    Iterator it;
    unidraw->First(it);
    _pausefunc = new UnidrawPauseFunc(comterp, unidraw->GetEditor(it));
  }

  _pausefunc->exec(0, 0);  
  comterp->pop_stack();
  return 1;
}

void PipeComp::Interpret(Command* cmd) {
    if (cmd->IsA(MOVE_CMD)) {
        float dx, dy;
        ((MoveCmd*) cmd)->GetMovement(dx, dy);
	GetGraphic()->Translate(dx, dy);
        NotifyLater();

	Iterator i;
	TopoNode* node = Node();
	Editor* ed = cmd->GetEditor();
	for (node->first(i); !node->done(i); node->next(i)) {
	    TopoEdge* edge = node->edge(node->elem(i));
	    EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
	    eucmd.Execute();
	}
    } else if (cmd->IsA(REORIENT_TEXT_CMD)) {
      Transformer tpic;
      if (GetGraphic()->GetTransformer()) tpic = *GetGraphic()->GetTransformer();
      tpic.Invert();
      float a00, a01, a10, a11, a20, a21;
      tpic.matrix(a00, a01, a10, a11, a20, a21);
      Transformer tname(a00, a01, a10, a11, 0.0, 0.0);
      GetText()->SetTransformer(new Transformer(tname));
      GetEllipse()->Align(Center, GetText(), Center);
      GetText2()->SetTransformer(new Transformer(tname));
      GetEllipse()->Align(TopCenter, GetText2(), BottomCenter);
      DirtyCmd dcmd(cmd->GetEditor());
      dcmd.Execute();
      NotifyLater();
      unidraw->Update();
    } else if (cmd->IsA(ROTATE_CMD)) {
    } else if (cmd->IsA(NODETEXT_CMD)) {
	NodeTextCmd* ntcmd = (NodeTextCmd*)cmd;
	TextGraphic* tg = ntcmd->Graphic();
	SetText(tg);
	_namesym = -1;
	init_global();
	_reqlabel = 1;
	Notify();
	unidraw->Update();
    }
    else
	NodeComp::Interpret(cmd);
}

void PipeComp::Uninterpret(Command* cmd) {
    if (cmd->IsA(MOVE_CMD)) {
        float dx, dy;
        ((MoveCmd*) cmd)->GetMovement(dx, dy);
	GetGraphic()->Translate(-dx, -dy);
        NotifyLater();

	Iterator i;
	TopoNode* node = Node();
	Editor* ed = cmd->GetEditor();
	for (node->first(i); !node->done(i); node->next(i)) {
	    TopoEdge* edge = node->edge(node->elem(i));
	    EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
	    eucmd.Execute();
	}
    }
    else if (cmd->IsA(ROTATE_CMD)) {
    }
    else
	NodeComp::Uninterpret(cmd);
}

Component* PipeComp::Copy() {
    PipeComp* comp = (PipeComp*) NewNodeComp((SF_Ellipse*)GetEllipse()->Copy(), 
            (TextGraphic*)GetText()->Copy());
    comp->RequireLabel(RequireLabel());
    if (GetText2())
      comp->SetText2((TextGraphic*)GetText2()->Copy());
    comp->GetGraphic()->SetTransformer(new Transformer(GetGraphic()->GetTransformer()));
    return comp;
}

boolean PipeComp::onscreen() {
  return GetParent() ? !GetParent()->IsA(PIPES_COMP) : 1;
}

InvoComp* PipeComp::invocomp() {
  if (GetParent() && GetParent()->IsA(PIPES_COMP))
    return ((PipesComp*)GetParent())->invocomp();
  else
    return nil;
}


boolean PipeComp::inconn_exists() {
  Iterator it;

  Node()->first(it);
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() == Node()) {
      return true;
    }
    Node()->next(it);
  }
  return false;
}

boolean PipeComp::outconn_exists() {
  Iterator it;

  Node()->first(it);
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      return true;
    }
    Node()->next(it);
  }
  return false;
}
  
void PipeComp::color_outconn(PSColor* color) {
  Iterator it;

  Node()->first(it);
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      ConnComp* conncomp = ((ConnComp*)edge->value());
      if (conncomp->GetGraphic()) {
	conncomp->GetGraphic()->SetColors(color, pswhite);
	conncomp->NotifyLater();
      }
      TopoNode* nextnode = edge->end_node();
      PipeComp* nextcomp = nextnode ? (PipeComp*)nextnode->value() : nil;
      if (nextcomp && nextcomp->IsA(FORK_COMP)) nextcomp->color_outconn(color);
    }
    Node()->next(it);
  }
  _current_outconn_color = color;
  return;
}

int PipeComp::nsrc() {
  int nin, nout;
#if 0
  PipesComp* pipesparent = (PipesComp*)(GetParent() && GetParent()->IsA(PIPES_COMP) ? GetParent() : nil);
  PipeComp* pipecomp = nil;
  if (pipesparent)
    pipecomp = pipesparent->invocomp();
  else
    pipecomp = this;
  pipecomp->nedges(nin, nout);
#else
  nedges(nin, nout);
#endif
  return nin;
}

int PipeComp::ndst() {
  int nin, nout;
#if 0
  PipesComp* pipesparent = (PipesComp*)(GetParent() && GetParent()->IsA(PIPES_COMP) ? GetParent() : nil);
  PipeComp* pipecomp = nil;
  if (pipesparent)
    pipecomp = pipesparent->invocomp();
  else
    pipecomp = this;
  pipecomp->nedges(nin, nout);
#else
  nedges(nin, nout);
#endif
  return nout;
}

int PipeComp::delay() {
  static int delaysym = symbol_add("delay");
  if (_delay<0) {
    AttributeValue* delayval = FindValue(delaysym);
    _delay = delayval ? delayval->int_val() : 0;
  }
  return _delay;
}

PipeComp* PipeComp::remote_srccomp(int index, int& order, boolean lowlevel) {

  Iterator it;
  Node()->first(it);
  int count = 0;

  /* find nth input connector */
  boolean found = false;
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() == Node()) {
      if (index==count) {
	PipeComp* startnode = (PipeComp*) edge->start_node()->value();
	order = startnode->EdgeOutOrder((EdgeComp*)edge->value());
	if (startnode->invocomp()) 
	  return lowlevel ? startnode : startnode->invocomp();
	else if (startnode->IsA(FORK_COMP)) return startnode->remote_srccomp(0, order);
	else return startnode;
      }
      count++;
    }
    Node()->next(it);
  }
  /* done finding nth input connector */

  order = -1;
  return nil;

}

PipeComp* PipeComp::remote_dstcomp(int index, int& order, boolean lowlevel) {

  Iterator it;
  Node()->first(it);
  int count = 0;

  /* find nth input connector */
  boolean found = false;
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      if (index==count) {
	ConnComp* edgecomp = (ConnComp*) edge->value();
	PipeComp* endnode = (PipeComp*) edge->end_node()->value();
	if (endnode->invocomp())
	  return lowlevel ? endnode : endnode->invocomp();
	else if (endnode->IsA(FORK_COMP)) return endnode->remote_dstcomp(index, order);
	else return endnode;
      } 
      count++;
    }
    Node()->next(it);
  }
  /* done finding nth input connector */

  order = -1;
  return nil;

}

ConnComp* PipeComp::srcconn(int index) {

  Iterator it;
  Node()->first(it);
  int count = 0;

  /* find nth input connector */
  boolean found = false;
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() == Node()) {
      if (index==count) {
	return (ConnComp*) edge->value();
      }
      count++;
    }
    Node()->next(it);
  }
  /* done finding nth input connector */

  return nil;

}

ConnComp* PipeComp::dstconn(int index) {

  Iterator it;
  Node()->first(it);
  int count = 0;

  /* find nth input connector */
  boolean found = false;
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      if (index==count) {
	return (ConnComp*) edge->value();
      } 
      count++;
    }
    Node()->next(it);
  }
  /* done finding nth input connector */

  return nil;

}

Picture* PipeComp::init_picture() {
  Picture* new_pict = new Picture();

  // calculate center of current canvas
  Iterator it;
  unidraw->First(it);
  Editor* editor = unidraw->GetEditor(it);
  Viewer* viewer = editor->GetViewer();
  Perspective* p = viewer->GetPerspective();
  Coord xpos = p->curwidth/2;
  Coord ypos = p->curheight/2;

  Transformer* rel = viewer->ComputeGravityRel();
  if (rel != nil)
    rel->InvTransform(xpos, ypos);
  new_pict->SetTransformer(nil);
  new_pict->Translate(xpos, ypos);

  FontVar* fontVar = (FontVar*) editor->GetState("FontVar");
  PSFont* font = (fontVar == nil) ? psstdfont : fontVar->GetFont();
  new_pict->SetFont(font);            
  
  BrushVar* brVar = (BrushVar*) editor->GetState("BrushVar");
  PatternVar* patVar = (PatternVar*) editor->GetState("PatternVar");
  ColorVar* colVar = (ColorVar*) editor->GetState("ColorVar");
  if (brVar != nil) new_pict->SetBrush(brVar->GetBrush());
  if (patVar != nil && patVar->GetPattern()->None())
    new_pict->SetPattern(unidraw->GetCatalog()->ReadPattern("pattern",3));
  else if (patVar != nil)
    new_pict->SetPattern(patVar->GetPattern());
  if (colVar != nil) {
    new_pict->FillBg(!colVar->GetBgColor()->None());
    new_pict->SetColors(colVar->GetFgColor(), colVar->GetBgColor());
  }

  SetGraphic(new_pict);
  return new_pict;
}

void PipeComp::build_pipe_graphic() {
  _reqlabel = 1;

  Iterator it;
  unidraw->First(it);
  Editor* editor = unidraw->GetEditor(it);

  Picture* pict = (Picture*)GetGraphic();
  if (!pict) pict = init_picture();
  Transformer* savetrans = pict->GetTransformer() ? new Transformer(*pict->GetTransformer()) : new Transformer();
  Ref(savetrans);
  pict->SetTransformer(nil);

  TextGraphic* textgr;
  SF_Ellipse* ellipse;
  FontVar* fontVar = (FontVar*) editor->GetState("FontVar");
  PSFont* font = (fontVar == nil) ? psstdfont : fontVar->GetFont();
  textgr = new TextGraphic(symbol_pntr(_namesym), stdgraphic);
  textgr->SetFont(font);            
  textgr->SetTransformer(new Transformer(savetrans));
  
  ellipse = new SF_Ellipse(0, 0, 35, 20, stdgraphic);
  ellipse->SetTransformer(new Transformer(savetrans));
  BrushVar* brVar = (BrushVar*) editor->GetState("BrushVar");
  PatternVar* patVar = (PatternVar*) editor->GetState("PatternVar");
  ColorVar* colVar = (ColorVar*) editor->GetState("ColorVar");
  if (brVar != nil) ellipse->SetBrush(brVar->GetBrush());
  if (patVar != nil && patVar->GetPattern()->None())
    ellipse->SetPattern(unidraw->GetCatalog()->ReadPattern("pattern",3));
  else if (patVar != nil)
    ellipse->SetPattern(patVar->GetPattern());
  if (colVar != nil) {
    ellipse->FillBg(!colVar->GetBgColor()->None());
    ellipse->SetColors(colVar->GetFgColor(), colVar->GetBgColor());
  }
  
  ellipse->Align(Center, textgr, Center);

  pict->Append(ellipse, textgr);
  pict->FillBg(ellipse->BgFilled() && !ellipse->GetBgColor()->None());
  pict->SetColors(ellipse->GetFgColor(), ellipse->GetBgColor());
  pict->SetPattern(ellipse->GetPattern());
  pict->SetBrush(ellipse->GetBrush());
  pict->SetFont(textgr->GetFont());
  
  pict->Append(new TextGraphic("", stdgraphic));
  GetText2()->SetTransformer(new Transformer(savetrans));
  GetEllipse()->Align(TopCenter, GetText2(), BottomCenter);

  pict->concatGS(GetEllipse(), GetGraphic(), GetEllipse()); 
  pict->concatGS(GetText(), GetGraphic(), GetText()); 
  pict->concatGS(GetText2(), GetGraphic(), GetText2()); 

  Unref(savetrans);
}

float PipeComp::put_rate() {
  if(!_put_history) {
    _put_history= new int[100];
    for(int i=0; i<100; i++)
      _put_history[i]=0;
  }
  int cycles_right_now = _put_history[_put_cnt%100 ? (_put_cnt%100)-1 : 99];
  int cycles_hundred_puts_ago = _put_history[_put_cnt%100];
  float pr = cycles_right_now!=cycles_hundred_puts_ago ? float(min(100,_put_cnt))/(cycles_right_now-cycles_hundred_puts_ago) : 0; // percent of max
  return pr;
}

/****************************************************************************/

ParamList* ArbiterComp::_arbiter_params = nil;
int ArbiterComp::_symid = -1;

ArbiterComp::ArbiterComp(SF_Ellipse* ellipse, TextGraphic* txt, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
    : PipeComp(ellipse, txt, txt2, rl, parent)
{
  init();
}

ArbiterComp::ArbiterComp(SF_Ellipse* ellipse, TextGraphic* txt, SF_Ellipse* ellipse2, 
    GraphComp* graph, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
    : PipeComp(ellipse, txt, ellipse2, graph, txt2, rl, parent)
{
  init();
}

ArbiterComp::ArbiterComp(Picture* pic, boolean rl, OverlayComp* parent) 
    : PipeComp(pic, rl, parent)
{
  init();
}

ArbiterComp::ArbiterComp(GraphComp* graph) 
    : PipeComp(graph)
{
  init();
}

ArbiterComp::ArbiterComp(OverlayComp* parent) : PipeComp( parent) 
{
  init();
}

ArbiterComp::ArbiterComp(istream& in, OverlayComp* parent) : PipeComp(in, parent)
{
  init();
}
    
void ArbiterComp::init() {
  buffsize(2);
  _lastinput = 0;
}

ArbiterComp::~ArbiterComp() {
}

ClassId ArbiterComp::GetClassId () { return ARBITER_COMP; }

boolean ArbiterComp::IsA (ClassId id) {
    return ARBITER_COMP == id || PipeComp::IsA(id);
}

void ArbiterComp::rdst_to_src(ConnComp* conn, PipeComp* rdst) {
  if (!src_vacant()) return;

#ifdef ARBITER_DEBUG
  if (this==_arbiter_debug)
    fprintf(stderr, "ArbiterComp::rdst_to_src:  at start _lastinput %d\n", _lastinput);
#endif

  boolean txt2reservation = false;

  /* determine ordinal of dst from src viewpoint */
  Iterator it;
  Node()->first(it);
  int rdstid = 0;
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
#ifdef ARBITER_DEBUG
    if (this==_arbiter_debug) {
      fprintf(stderr, "ArbiterComp::rdst_to_src:  rdstid %d\n", rdstid);
      fprintf(stderr, "ArbiterComp::rdst_to_src:  edge->start_node() 0x%x\n", edge->start_node());
      fprintf(stderr, "ArbiterComp::rdst_to_src:  rdst->Node() 0x%x\n", rdst->Node());
    }
#endif
    if (edge->end_node() != Node()) {
      Node()->next(it);
      continue;
    }
    if (edge->start_node() == rdst->Node()) 
      break;
    rdstid++;
    Node()->next(it);
  }
#ifdef ARBITER_DEBUG
  if (this==_arbiter_debug)
    fprintf(stderr, "ArbiterComp::rdst_to_src:  final rdstid %d\n", rdstid);
#endif

  /* buff has no entries, take anything, but remember what you took */

  if (buff()->Number()==0) {
#ifdef ARBITER_DEBUG
  if (this==_arbiter_debug)
    fprintf(stderr, "ArbiterComp::rdst_to_src:  buffer length zero\n");
#endif
    _lastinput = rdstid;
    txt2reservation = reserve_text2_update();
    PipeComp::rdst_to_src(conn, rdst);
#ifdef ARBITER_DEBUG
      if (this==_arbiter_debug)
	fprintf(stderr, "ArbiterComp::rdst_to_src:  afterwards buffer length %d\n", buff()->Number());
#endif
  }  

  /* buff has one entry, take only the opposite */
  else if(buff()->Number()==1) {
    if (_lastinput != rdstid) {
#ifdef ARBITER_DEBUG
      if (this==_arbiter_debug)
	fprintf(stderr, "ArbiterComp::rdst_to_src:  buffer length one\n");
#endif
      _lastinput = !_lastinput;
      txt2reservation = reserve_text2_update();
      PipeComp::rdst_to_src(conn, rdst);
#ifdef ARBITER_DEBUG
      if (this==_arbiter_debug)
	fprintf(stderr, "ArbiterComp::rdst_to_src:  afterwards buffer length %d\n", buff()->Number());
#endif
    }
  }

  if (txt2reservation || animateflag()) {
    if(txt2reservation) unreserve_text2_update();
    if(!already_animated()) update_text2();
    if(animateflag()&&!already_animated()) already_animated(1);
    if(txt2reservation) already_animated(0);
  }

#ifdef ARBITER_DEBUG
  if (this==_arbiter_debug)
    fprintf(stderr, "ArbiterComp::rdst_to_src:  at end _lastinput %d\n", _lastinput);
#endif
}

void ArbiterComp::src_request() {
  if(_src_requested) return;

  if(!src_vacant()) return;

  if (!_eventqueueflag)
    _src_request();
  else {
    _src_requested = 1;
    if (!_eventqueue) _eventqueue = new PipeEventQueue();
      _eventqueue->append(new PipeEvent(this, false /* !notify_flag */));
  }
}

void ArbiterComp::_src_request() {
#ifdef ARBITER_DEBUG
  if (this==_arbiter_debug)
    fprintf(stderr, "ArbiterComp::_src_request:  at start _lastinput %d\n", _lastinput);
#endif

  PipeComp* startnode0 = nil;
  PipeComp* startnode1 = nil;
  ConnComp* edgecomp0 = nil;
  ConnComp* edgecomp1 = nil;
  Iterator it;
  Node()->first(it);
  
  /* find two input pipes */
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      if (startnode0) {
	startnode1 = (PipeComp*) edge->start_node()->value();
	edgecomp1 = edgecomp;
      } else {
	startnode0 = (PipeComp*) edge->start_node()->value();
	edgecomp0 = edgecomp;
      }
    }
    Node()->next(it);
  }
  /* done finding two input pipes */

  /* no input ready */
  if ((!edgecomp0 || !edgecomp0->handout()) && (!edgecomp1 || !edgecomp1->handout())) {
#ifdef ARBITER_DEBUG
    if (this==_arbiter_debug) {
      fprintf(stderr, "ArbiterComp::_src_request:  no input ready\n");
      fprintf(stderr, "ArbiterComp::_src_request:  at end _lastinput %d\n", _lastinput);
    }
#endif
    return;
  }

  /* both inputs ready */
  if (edgecomp0 && edgecomp0->handout() && edgecomp1 && edgecomp1->handout() && buff()->Number()<2) {
#ifdef ARBITER_DEBUG
    if (this==_arbiter_debug)
      fprintf(stderr, "ArbiterComp::_src_request:  both inputs ready\n");
#endif
    _lastinput = !_lastinput;
  }

  /* zeroth input ready */
  else if (edgecomp0 && edgecomp0->handout()) {
#ifdef ARBITER_DEBUG
    if (this==_arbiter_debug)
      fprintf(stderr, "ArbiterComp::_src_request:  zeroth input ready\n");
#endif
    if (_lastinput == 0 && buff()->Number()==1 ||
	buff()->Number()==2) {
#ifdef ARBITER_DEBUG
      if (this==_arbiter_debug) {
	fprintf(stderr, "ArbiterComp::_src_request:  not gonna take the zeroth\n");
	fprintf(stderr, "ArbiterComp::_src_request:  at end _lastinput %d\n", _lastinput);
      }
#endif
      return;
    }
    else
      _lastinput = 0;
  } 

  /* oneth input ready */
  else {
#ifdef ARBITER_DEBUG
    if (this==_arbiter_debug)
      fprintf(stderr, "ArbiterComp::_src_request:  oneth input ready\n");
#endif
    if (_lastinput == 1 && buff()->Number()==1 ||
	buff()->Number()==2)
      {
#ifdef ARBITER_DEBUG
	if (this==_arbiter_debug) {
	  fprintf(stderr, "ArbiterComp::_src_request:  not gonna take the oneth\n");
	  fprintf(stderr, "ArbiterComp::_src_request:  at end _lastinput %d\n", _lastinput);
	}
#endif
	return;
      }
    else
      _lastinput = 1;
  }

  boolean txt2reservation = reserve_text2_update();

  PipeComp::rdst_to_src((_lastinput ? edgecomp1 : edgecomp0), (_lastinput ? startnode1 : startnode0));

  if (txt2reservation || animateflag()) {
    if(txt2reservation) unreserve_text2_update();
    if(!already_animated()) update_text2();
    if(animateflag()&&!already_animated()) already_animated(1);
    if(txt2reservation) already_animated(0);
  }

#ifdef ARBITER_DEBUG
  if (this==_arbiter_debug) {
    fprintf(stderr, "ArbiterComp::_src_request:  buffer size %d\n", buff()->Number());
    fprintf(stderr, "ArbiterComp::_src_request:  at end _lastinput %d\n", _lastinput);
  }
#endif
}

int ArbiterComp::_src_ready() {
  PipeComp* startnode0 = nil;
  PipeComp* startnode1 = nil;
  ConnComp* edgecomp0 = nil;
  ConnComp* edgecomp1 = nil;
  Iterator it;
  Node()->first(it);
  
  /* find two input pipes */
  while (!Node()->done(it)) {
    
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      if (startnode0) {
	startnode1 = (PipeComp*) edge->start_node()->value();
	edgecomp1 = edgecomp;
      } else {
	startnode0 = (PipeComp*) edge->start_node()->value();
	edgecomp0 = edgecomp;
      }
    }
    Node()->next(it);
  }
  /* done finding two input pipes */

  if ((!edgecomp0 || !edgecomp0->handout()) && (!edgecomp1 || !edgecomp1->handout()))
    return 0;
  else
    return src_vacant();

}

boolean ArbiterComp::arbiter_vacant(PipeComp* rdst) {

  /* determine ordinal of dst from src viewpoint */
  Iterator it;
  Node()->first(it);
  int rdstid = 0;
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->end_node() != Node()) {
      Node()->next(it);
      continue;
    }
    if (edge->start_node() == rdst->Node()) 
      break;
    rdstid++;
    Node()->next(it);
  }

  /* buff has no entries, ready to take anything */
  if (buff()->Number()==0) 
    return 1;

  /* buff has one entry, take only the opposite */
  else if(buff()->Number()==1 && _lastinput != rdstid)
    return 1;

  else
    return 0;

}

void ArbiterComp::dst_notify() {
  if (_dst_notified) return;

  if (!dst_occupied()) return;

  if (!_eventqueueflag)
    _dst_notify();
  else {
    _dst_notified = 1;
    if (!_eventqueue) _eventqueue = new PipeEventQueue();
    _eventqueue->append(new PipeEvent(this, true /* notify_flag */));
  }
}

void ArbiterComp::_dst_notify() {
  if (_handsout>0) 
    return;
  else 
    PipeComp::_dst_notify();
}

int ArbiterComp::_dst_ready() {
  if (_handsout>0) 
    return 0;
  else 
    return PipeComp::_dst_ready();
}

ParamList* ArbiterComp::GetParamList() {
    if (!_arbiter_params) 
	GrowParamList(_arbiter_params = new ParamList());
    return _arbiter_params;
}

void ArbiterComp::GrowParamList(ParamList* pl) {
    PipeComp::GrowParamList(pl);
    return;
}

/****************************************************************************/

ParamList* ForkComp::_fork_params = nil;
int ForkComp::_symid = -1;

ForkComp::ForkComp(SF_Ellipse* ellipse, TextGraphic* txt, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
    : PipeComp(ellipse, txt, txt2, rl, parent)
{
  init();
}

ForkComp::ForkComp(SF_Ellipse* ellipse, TextGraphic* txt, SF_Ellipse* ellipse2, 
    GraphComp* graph, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
    : PipeComp(ellipse, txt, ellipse2, graph, txt2, rl, parent)
{
  init();
}

ForkComp::ForkComp(Picture* pic, boolean rl, OverlayComp* parent) 
    : PipeComp(pic, rl, parent)
{
  init();
}

ForkComp::ForkComp(GraphComp* graph) 
    : PipeComp(graph)
{
  init();
}

ForkComp::ForkComp(OverlayComp* parent) : PipeComp( parent) 
{
  init();
}

ForkComp::ForkComp(istream& in, OverlayComp* parent) : PipeComp(in, parent)
{
  init();
}
    
void ForkComp::init() {
  _forking = false;
  if (Component::use_unidraw() && GetGraphic()) GetGraphic()->SetPattern(pssolid);
  _prepped = 0;
  _upflag = 0;
  _distnamesym = 0;
  _conn = nil;
  if (Component::use_unidraw() && GetText()){
    GetGraphic()->concatGS(GetText(), GetGraphic(), GetText()); 
    GetEllipse()->Align(7, GetText(), 1);
  }

}

ForkComp::~ForkComp() {
  if (_conn) {
    _conn->Edge()->remove_nodes();
    delete _conn;
  }
}

ClassId ForkComp::GetClassId () { return FORK_COMP; }

boolean ForkComp::IsA (ClassId id) {
    return FORK_COMP == id || PipeComp::IsA(id);
}

ParamList* ForkComp::GetParamList() {
    if (!_fork_params) 
	GrowParamList(_fork_params = new ParamList());
    return _fork_params;
}

void ForkComp::GrowParamList(ParamList* pl) {
    PipeComp::GrowParamList(pl);
    return;
}

void ForkComp::rdst_to_src(ConnComp* conn, PipeComp* rdst) {
  if (!src_vacant()) return;

  boolean nothing_new = false;
  boolean srcput_flag = false;
  // clear the handshake the fork has been secretly duplicating
  if (_forking) {
    rdst_clr_handshake(conn, rdst);
    rdst_clr_val(rdst);
    if (rdst->handsout()==0) src_request();
    else nothing_new = true;
    _forking = false;
  }

  // make a copy of the new value without clearing 
  if (!nothing_new && rdst->dst_occupied()) {
    AttributeValue* av = rdst_cpy(rdst);
    _forking = true;
    srcput_flag = src_put(av);
  }

    /* if all handshakes cleared, set up for next value */
  boolean txt2reservation = !rdst->handsout() && rdst->reserve_text2_update();
  rdst_chk(rdst);
  if (txt2reservation || animateflag()) {
    if(txt2reservation) rdst->unreserve_text2_update();
    if(!rdst->already_animated()) rdst->update_text2();
    if(animateflag()&&!rdst->already_animated()) rdst->already_animated(1);
    if(txt2reservation) rdst->already_animated(0);
  }

  if (srcput_flag) dst_notify();
}

void ForkComp::color_outconn(PSColor* color) {
  GetGraphic()->SetColors(color, pswhite);
  NotifyLater();
  PipeComp::color_outconn(color);
}
  
int ForkComp::update_and_pause() {
  return 1;
}

void ForkComp::update_text2() {
  if (_showforkvals) PipeComp::update_text2();
}

void ForkComp::Interpret(Command* cmd) {
  if (cmd->IsA(SQUARE_CMD)) {
    TopoNode* node = Node();
    Iterator i;
    boolean horiz = false, vert = false;

    // first do incoming connections
    for (node->first(i); !node->done(i); node->next(i)) {
      TopoEdge* edge = node->edge(node->elem(i));
      if (edge->end_node()==node) {
	ConnComp* conncomp = (ConnComp*)edge->value();
	if (!conncomp || !conncomp->IsA(CONN_COMP)) continue;
	
	int dx, dy;
	// do iteratively, because the remote endpoint can change
	do {
	  int x0, y0, x1, y1;
	  conncomp->GetArrowLine()->GetOriginal(x0, y0, x1, y1);
	  dx= x0-x1;
	  dy= y0-y1;
	  if (abs(dx)>abs(dy)) {
	    dx=0;
	    horiz = true;
	  }
	  else {
	    dy = 0;
	    vert = true;
	  }
	  if (dx !=0 || dy!=0) {
	    GetGraphic()->Translate(dx, dy);
	    NotifyLater();
	    cmd->Store(this, new MoveData(dx, dy));
	    EdgeUpdateCmd eucmd(cmd->GetEditor(), (EdgeComp*)edge->value());
	    eucmd.Execute();
	  }
	} while (dx!=0 || dy!=0);

      }
    }

    // then all outgoing connections that won't re-do an incoming straightening
    for (node->first(i); !node->done(i); node->next(i)) {
      TopoEdge* edge = node->edge(node->elem(i));
      if (edge->start_node()==node) {
	ConnComp* conncomp = (ConnComp*)edge->value();
	if (!conncomp || !conncomp->IsA(CONN_COMP)) continue;

	int dx, dy;
	// do iteratively, because the remote endpoint can change
	do {
	  int x0, y0, x1, y1;
	  conncomp->GetArrowLine()->GetOriginal(x0, y0, x1, y1);
	  dx= x1-x0;
	  dy= y1-y0;
	  if (abs(dx)>abs(dy)) {
	    dx=0;
	    if (horiz) dy=0;
	  }
	  else {
	    dy = 0;
	    if (vert) dx=0;
	  }
	  if (dx !=0 || dy !=0 ) {
	    GetGraphic()->Translate(dx, dy);
	    NotifyLater();
	    MoveData* md = (MoveData*) cmd->Recall(this);
	    if (md) {
	      md->_dx += dx;
	      md->_dy += dy;
	    } else 
	      cmd->Store(this, new MoveData(dx, dy));
	    EdgeUpdateCmd eucmd(cmd->GetEditor(), (EdgeComp*)edge->value());
	    eucmd.Execute();
	  }
	} while (dx!=0 || dy!=0);
      }
    }

    // straighten up all edges after nodes have moved
    for (node->first(i); !node->done(i); node->next(i)) {
      TopoEdge* edge = node->edge(node->elem(i));
      EdgeUpdateCmd eucmd(cmd->GetEditor(), (EdgeComp*)edge->value());
      eucmd.Execute();
    }
    
  } else if (cmd->IsA(REORIENT_TEXT_CMD)) {
    Transformer tpic;
    if (GetGraphic()->GetTransformer()) tpic = *GetGraphic()->GetTransformer();
    tpic.Invert();
    float a00, a01, a10, a11, a20, a21;
    tpic.matrix(a00, a01, a10, a11, a20, a21);
    Transformer tname(a00, a01, a10, a11, 0.0, 0.0);
    GetText()->SetTransformer(new Transformer(tname));
    GetEllipse()->Align(BottomCenter, GetText(), TopCenter);
    GetText2()->SetTransformer(new Transformer(tname));
    GetEllipse()->Align(TopCenter, GetText2(), BottomCenter);
    DirtyCmd dcmd(cmd->GetEditor());
    dcmd.Execute();
    NotifyLater();
    unidraw->Update();
  }

  else
    PipeComp::Interpret(cmd);
}

void ForkComp::Uninterpret(Command* cmd) {
  if (cmd->IsA(SQUARE_CMD)) {
    MoveData* md = (MoveData*) cmd->Recall(this);
    
    if (md != nil) {
      GetGraphic()->Translate(-md->_dx, -md->_dy);
      NotifyLater();
      TopoNode* node = Node();
      Iterator i;
      for (node->first(i); !node->done(i); node->next(i)) {
	TopoEdge* edge = node->edge(node->elem(i));
	EdgeUpdateCmd eucmd(cmd->GetEditor(), (EdgeComp*)edge->value());
	eucmd.Execute();
      }
    }
    
  }
  else
    PipeComp::Uninterpret(cmd);
}

void ForkComp::src_request() {

  /* simplified version of PipeComp::src_request that ignores eventqueue */
  if (_src_requested) return;
  if(!src_vacant()) return;
  _src_request();

}

void ForkComp::dst_notify() {

  /* simplified version of PipeComp::dst_notify that ignores eventqueue */
  if (_dst_notified) return;
  if (!dst_occupied()) return;
  _dst_notify();

}

void ForkComp::_src_request() {

  if (!_prepped) {
    _prepped = true;
    prep();
  }

  PipeComp::_src_request();
}

void ForkComp::_dst_notify() {

  if (!_prepped) {
    _prepped = true;
    prep();
  }

  PipeComp::_dst_notify();
}
 
#ifdef TRUE_BLUE_BUBBLES
// experiment with forward reaching src_vacant
// failed because src_request needs it to go vacant
boolean ForkComp::src_vacant() {

  if (PipeComp::src_vacant()) return 1;  // change since first experiment

  /* loop through all output connectors to see if all values would move */
  Iterator it;
  Node()->first(it);
  while (!Node()->done(it)) {
    TopoEdge* edge = Node()->get_edge(it);
    if (edge->start_node() == Node()) {
      ConnComp* edgecomp = (ConnComp*) edge->value();
      PipeComp* srccomp = (PipeComp*)edge->end_node()->value();
      if(!srccomp->src_vacant()) return false;
    }
    Node()->next(it);
  }
  /* done looping through all output connectors to see if all values would move */

  return true;

}
#endif

void ForkComp::prep() {
  static int upflagsym = symbol_add("upflag");
  AttributeValue* upflagval = FindValue(upflagsym);
  if (!upflagval) return;
  _upflag = upflagval->is_true();
  if (_upflag && nsrc()>0 || !_upflag && ndst()>0) return;

  static int namesym = symbol_add("name");
  AttributeValue* nameval = FindValue(namesym);
  if (nameval) {
    void* forkptr = nil;
    ForkComp* forkcomp = nil;
    ComTerpServ* comterp = ((OverlayUnidraw*)unidraw)->comterp();
    comterp->localtable()->find(forkptr, nameval->symbol_val());
    if(!forkptr) 
      comterp->globaltable()->find(forkptr, nameval->symbol_val());
    if (forkptr) {
      ComValue* forkval = (ComValue*)forkptr;
      forkcomp = (ForkComp*)forkval->geta(ForkComp::class_symid());
      if (forkcomp) {
	TopoNode *upnode = _upflag ? forkcomp->Node() : this->Node(); 
	PipeComp *upcomp = _upflag ? forkcomp : this;
	TopoNode *dnnode = _upflag ? this->Node() : forkcomp->Node();
	PipeComp *dncomp = _upflag ? this : forkcomp;
	_conn = new ConnComp();
	_conn->Edge()->attach_nodes(upnode, dnnode);
      }
    }
    return;
  }
}

/****************************************************************************/

ParamList* PipesComp::_pipes_params = nil;
int PipesComp::_symid = -1;

PipesComp::PipesComp (OverlayComp* parent) 
    : GraphComp((Graphic*)nil, parent) {
  init();
}

PipesComp::PipesComp (istream& in, const char* pathname, OverlayComp* parent) : GraphComp(pathname, parent) {
  init();
  _valid = GetParamList()->read_args(in, this);
}

ParamList* PipesComp::GetParamList() {
    if (!_pipes_params) 
	GrowParamList(_pipes_params = new ParamList());
    return _pipes_params;
}

void PipesComp::GrowParamList(ParamList* pl) {
    GraphComp::GrowParamList(pl); 
}

PipesComp::~PipesComp() {
}

void PipesComp::init() {
  _complete = false;
  _exclusive = false;
  _dst_notified = 0;
  _src_requested = 0;
  _ackwait = 0;
}

ClassId PipesComp::GetClassId () { return PIPES_COMP; }

boolean PipesComp::IsA (ClassId id) {
    return PIPES_COMP == id || GraphComp::IsA(id);
}

boolean PipesComp::src_vacant() {
  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe && !parentpipe->IsA(INVOPIPES_COMP) /* && !parentpipe->exclusive() */) 
    return parentpipe->src_vacant();
  else
    return _src_vacant();
}

boolean PipesComp::_src_vacant() {
  // if this composite pipe is not complete, some subpipe is waiting for data
  // testing fullness refreshes _complete
  if (!complete()) return true;

  // toggle complete only when all subpipes have been emptied
  Iterator it;
  First(it);
  while (!Done(it)) {
    PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
    if (comp && !comp->_src_vacant()) return false;
    PipesComp* comps = GetComp(it)->IsA(PIPES_COMP) ? (PipesComp*)GetComp(it) : nil;
    if (comps && !comps->_src_vacant()) return false;
    Next(it);
  }
  
  // no subpipe still complete
  if(!_ackwait) {
    _complete = false;
    return true;
  } else
    return false;
}

boolean PipesComp::dst_occupied() {
  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe && !parentpipe->IsA(INVOPIPES_COMP) && !parentpipe->exclusive()) 
    return parentpipe->dst_occupied();
  else
    return _dst_occupied();
}

boolean PipesComp::_dst_occupied() {
  Iterator it;
  First(it);

  // not complete, check for completion by testing if every subpipe is complete
  if (!_complete) {

#if defined(COMPLETION_BLOCK)
    // return false if this is the source side of an InvoComp
    // and a completion block is on.
    if (invocomp() && invocomp()->completion_block()) {
      PipesComp* parent = this;
      while (1) {
	if (parent->GetParent() != (GraphicComp*)invocomp()->invopipes()) 
	  parent = (PipesComp*)parent->GetParent();
	else
	  break;
      }
      if (parent==invocomp()->invopipes()->srccomp())
	return false;
    }
#endif

    // check for ALU style completeness first
    if (invocomp() && (invocomp()->invopipes()->srccomp()==this ||
		       invocomp()->invopipes()->dstcomp()==this)) {
      boolean srcflag = invocomp()->invopipes()->srccomp()==this;
      if (invocomp()->alustyle(srcflag))
	return _complete = invocomp()->alucomplete(srcflag);
    }

    while (!Done(it)) {
      PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
      if (comp) {
	if (!comp->_dst_occupied() && !exclusive()) 
	  return false;
	if (comp->_dst_occupied() && exclusive()) 
	  return _complete = true;
      }
      PipesComp* comps = GetComp(it)->IsA(PIPES_COMP) ? (PipesComp*)GetComp(it) : nil;
      if (comps) {
	if (!comps->_dst_occupied() && !exclusive()) 
	  return false;
	if (comps->_dst_occupied() && exclusive()) 
	  return _complete = true;
      }
      Next(it);
    }
    if (!exclusive()) 
      return _complete = true;
    else
      return false;

  }

  // complete, check for readiness to disable completeness by seeing if every subpipe is empty
  else {
    while (!Done(it)) {
      PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
      if (comp && comp->_dst_occupied()) return true;
      PipesComp* comps = GetComp(it)->IsA(PIPES_COMP) ? (PipesComp*)GetComp(it) : nil;
      if (comps && comps->_dst_occupied()) return true;
      Next(it);
    }
    // fprintf(stderr, "_complete disabled in PipesComp::_dst_occupied\n");
    if (!_ackwait)
      return _complete = false;
    else
      return true;
  }
}


void PipesComp::src_request() {
  if (_src_requested) return;

  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe) {
    if (!parentpipe->IsA(INVOPIPES_COMP)) 
      parentpipe->src_request();
    else {
      PipesComp* srccomp = ((InvoPipesComp*)parentpipe)->srccomp();
      if (this == srccomp) {
	if (!PipeComp::_eventqueueflag)
	  _src_request();
	else {
	  _src_requested = 1;
	  if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
	  PipeComp::_eventqueue->append(new PipeEvent(this, false /* !notify_flag */));
	}
      }
#ifdef INVO_UNIT
      else if (PipeComp::unitaryflag() && srccomp && srccomp->ackwait()) {
	srccomp->ackwait(0);
	if(srccomp->src_vacant())
	  srccomp->src_request();
      }
#endif
      else if (srccomp && srccomp->dst_occupied()) {
#ifndef INVO_TICK
	if(!PipeComp::animateflag() || !PipeComp::eventqueueflag()) {
#endif
#if 0
	  printf("ready to fire upstream invocation %s\n", 
		 symbol_pntr(((InvoPipesComp*)parentpipe)->invocomp()->func()->funcid()));
#endif
	  ((InvoPipesComp*)parentpipe)->invocomp()->invofunc();
#ifndef INVO_TICK
	}
	else {
	  _src_requested = 1;
	  if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
	  PipeComp::_eventqueue->append(new PipeEvent(this, false /* notify_flag */));
	}
#endif
      } 
    }
  }
  else {
    if (!PipeComp::_eventqueueflag)
      _src_request();
    else {
      _src_requested = 1;
      if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
      PipeComp::_eventqueue->append(new PipeEvent(this, false /* !notify_flag */));
    }
  }
}

void PipesComp::_src_request() {
#ifdef INVO_TICK
  /* if animated */
  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe && parentpipe->IsA(INVOPIPES_COMP) && this==((InvoPipesComp*)parentpipe)->dstcomp()) {
#if 0
    printf("ready to fire upstream invocation %s\n", 
	   symbol_pntr(((InvoPipesComp*)parentpipe)->invocomp()->func()->funcid()));
#endif
    ((InvoPipesComp*)parentpipe)->invocomp()->invofunc();
    return;
  }
#endif

  if (_complete) return;
  Iterator it;
  First(it);
  while (!Done(it)) {
    PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
    if (comp) comp->_src_request();
    PipesComp* comps = GetComp(it)->IsA(PIPES_COMP) ? (PipesComp*)GetComp(it) : nil;
    if (comps) comps->_src_request();
    Next(it);
  }
}

int PipesComp::_src_ready() {
  if (_complete) return 0;
  Iterator it;
  First(it);
  while (!Done(it)) {
    PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
    if (comp) if (comp->_src_ready()) return 1;
    PipesComp* comps = GetComp(it)->IsA(PIPES_COMP) ? (PipesComp*)GetComp(it) : nil;
    if (comps) if (comp->_src_ready()) return 1;
    Next(it);
  }
  return 0;
}

void PipesComp::dst_notify() {
  if (_dst_notified) return;

  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe) {
    if (!parentpipe->IsA(INVOPIPES_COMP))
      parentpipe->dst_notify();
    else {
      PipesComp* dstcomp = ((InvoPipesComp*)parentpipe)->dstcomp();
      if (this == dstcomp) {
	if (!PipeComp::_eventqueueflag)
	  _dst_notify();
	else {
	  _dst_notified = 1;
	  if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
	  PipeComp::_eventqueue->append(new PipeEvent(this, true /* notify_flag */));
	}
      }
      else if (dstcomp && dstcomp->src_vacant()) {
#ifndef INVO_TICK
	if (!PipeComp::animateflag() || !PipeComp::eventqueueflag()) {
#endif
#if 0
	  printf("ready to fire downstream invocation %s\n", 
		 symbol_pntr(((InvoPipesComp*)parentpipe)->invocomp()->func()->funcid()));
#endif
	  ((InvoPipesComp*)parentpipe)->invocomp()->invofunc();
#ifndef INVO_TICK
	} 
	else {
	  _dst_notified = 1;
	  if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
	  PipeComp::_eventqueue->append(new PipeEvent(this, true /* notify_flag */));
	}
#endif	
      }
    }
  }
  else {
    if (!PipeComp::_eventqueueflag)
      _dst_notify();
    else {
      _dst_notified = 1;
      if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
      PipeComp::_eventqueue->append(new PipeEvent(this, true /* notify_flag */));
    }
  }
}

void PipesComp::_dst_notify() {
#ifndef INVO_TICK
  /* if animated */
  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe && parentpipe->IsA(INVOPIPES_COMP) && this==((InvoPipesComp*)parentpipe)->srccomp()) {
#if 0
    printf("ready to fire downstream invocation %s\n", 
	   symbol_pntr(((InvoPipesComp*)parentpipe)->invocomp()->func()->funcid()));
#endif
    ((InvoPipesComp*)parentpipe)->invocomp()->invofunc();
    return;
  }
#endif

  if (!_complete) return;

  // !exclusive()
  if (!exclusive()) {
    Iterator it;
    First(it);
    while (!Done(it)) {
      PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
      if (comp) comp->_dst_notify();
      Next(it);
    }
  } 

  // exclusive()
  else {
    Iterator it;
    First(it);
    while (!Done(it)) {
      PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
      if (comp && comp->_dst_occupied()) comp->_dst_notify();
      PipesComp* comps = GetComp(it)->IsA(PIPES_COMP) ? (PipesComp*)GetComp(it) : nil;
      if (comps && comps->_dst_occupied()) comps->_dst_notify();
      Next(it);
    }
  }
}

int PipesComp::_dst_ready() {
  /* if animated */
  PipesComp* parentpipe = (GetParent() && GetParent()->IsA(PIPES_COMP)) ? (PipesComp*)GetParent() : nil;
  if (parentpipe && parentpipe->IsA(INVOPIPES_COMP) && this==((InvoPipesComp*)parentpipe)->srccomp()) {
    return 1;
  }

  if (!_complete) return 0;

  // !exclusive()
  if (!exclusive()) {
    Iterator it;
    First(it);
    while (!Done(it)) {
      PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
      if (comp) if(comp->_dst_ready()) return 1;
      Next(it);
    }
  } 

  // exclusive()
  else {
    Iterator it;
    First(it);
    while (!Done(it)) {
      PipeComp* comp = GetComp(it)->IsA(PIPE_COMP) ? (PipeComp*)GetComp(it) : nil;
      if (comp && comp->_dst_occupied()) 
	if(comp->_dst_ready()) return 1;
      PipesComp* comps = GetComp(it)->IsA(PIPES_COMP) ? (PipesComp*)GetComp(it) : nil;
      if (comps && comps->_dst_occupied()) 
	if(comps->_dst_ready()) return 1;
      Next(it);
    }
  }

  return 0;
}

ostream& operator<< (ostream& out, const PipesComp& pc) {

  PipesComp* pipescomp = (PipesComp*)&pc;
#if defined(FLAT_ARGLIST)
  Iterator i;
  for (pipescomp->First(i); !pipescomp->Done(i);) {
    PipeComp* pipecomp = (PipeComp*)pipescomp->GetComp(i);
    out << *pipecomp->buff();
    pipescomp->Next(i);
    if (!pipescomp->Done(i)) out << ",";
  }
#else
  boolean first = true;
  OverlayComp* before = nil;
  OverlayComp* treenode = pipescomp;
  while (treenode && treenode != pipescomp->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (treenode && treenode->IsA(PIPE_COMP)) {
      if (!first) 
	out << ",";
      else
	first = false;
      out << *((PipeComp*)treenode)->buff();
    }
  }
#endif
  return out;
}

void PipesComp::update_parent_text(PipesComp* child, PipeComp* grandchild) {
  if (GetParent() && GetParent()->IsA(PIPES_COMP)) {
    PipesComp* pipescomp = (PipesComp*)GetParent();
    pipescomp->update_parent_text(this, grandchild);
  }
}

InvoComp* PipesComp::invocomp() {
  if (!GetParent() || !GetParent()->IsA(PIPES_COMP)) return nil;
  if (GetParent()->IsA(INVOPIPES_COMP)) 
    return ((InvoPipesComp*)GetParent())->invocomp();
  return ((PipesComp*)GetParent())->invocomp();
}

int PipesComp::numleaf() {
  OverlayComp* before = nil;
  OverlayComp* treenode = this;
  int count = 0;
  while (treenode && treenode != this->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (treenode && treenode->IsA(PIPE_COMP))
      count++;
  }
  return count;
}
  
/****************************************************************************/

int InvoPipesComp::_symid = -1;

InvoPipesComp::InvoPipesComp (OverlayComp* parent) 
    : PipesComp(parent) {
  init();
}

void InvoPipesComp::init() { 
  _invocomp = nil;
  _exclusive = true;
}

InvoPipesComp::~InvoPipesComp() {
}

ClassId InvoPipesComp::GetClassId () { return INVOPIPES_COMP; }

boolean InvoPipesComp::IsA (ClassId id) {
    return INVOPIPES_COMP == id || PipesComp::IsA(id);
}

PipesComp* InvoPipesComp::srccomp() {
  Iterator it;
  First(it);
  if (!Done(it)) 
    return (PipesComp*)GetComp(it);
  else
    return nil;
}

PipesComp* InvoPipesComp::dstcomp() {
  Iterator it;
  First(it);
  Next(it);
  if (!Done(it)) 
    return (PipesComp*)GetComp(it);
  else
    return nil;
}

PipeComp* InvoPipesComp::find_subpipe(boolean srcflag, int index) {
#if defined(FLAT_ARGLIST)
  PipesComp* subpipescomp = srcflag ? srccomp() : dstcomp();
  Iterator it;
  subpipescomp->First(it);
  for (int i=0; i<index; i++) subpipescomp->Next(it);
  return (PipeComp*)subpipescomp->GetComp(it);
#else
  PipesComp* subpipescomp = srcflag ? srccomp() : dstcomp();
  OverlayComp* before = nil;
  OverlayComp* treenode = subpipescomp;
  int count = 0;
  while (treenode && treenode != subpipescomp->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (treenode && treenode->IsA(PIPE_COMP)) {
      if (count==index) break;
      count++;
    }
  }
  if(treenode==subpipescomp->GetParent()) treenode=nil;
  return treenode ? (PipeComp*)treenode : nil;
#endif
}
  
void InvoPipesComp::update_parent_text(PipesComp* child, PipeComp* grandchild) {
  if(child == srccomp())
    invocomp()->update_srctext(grandchild);
  else
    invocomp()->update_dsttext(grandchild);
}

/****************************************************************************/

implementTable(FuncCountTable,int,int)
FuncCountTable* InvoComp::_func_count_table = nil;

ParamList* InvoComp::_invo_params = nil;
int InvoComp::_symid = -1;
int InvoComp::_trix[3] = {0, 0, 13};
int InvoComp::_triy[3] = {0, 15, 7};

InvoComp::InvoComp(Picture* pic, boolean reqlabel, OverlayComp* parent) 
    : PipeComp(pic, reqlabel, parent)
{
  init();
}

InvoComp::InvoComp(GraphComp* graph) 
    : PipeComp(graph)
{
  init();
}

InvoComp::InvoComp(OverlayComp* parent) : PipeComp( parent) 
{
  init();
}

InvoComp::InvoComp(istream& in, OverlayComp* parent) : PipeComp(parent)
{
  init();
  _graph = nil;
  _node = new TopoNode(this);
  Picture *pic = new Picture();
  SetGraphic(pic);
  _valid = GetParamList()->read_args(in, this);
  build_invocation();
  build_invocation_graphic();
}
    
void InvoComp::init() { 
  _func = nil;
  _funcname = nil;
  _nsrcsizes = 0;
  _srcsizes = nil;
  _ndstsizes = 0;
  _dstsizes = nil;
  _srctext_update_reserved = 0;
  _dsttext_update_reserved = 0;
#if defined(COMPLETION_BLOCK)
  _completion_block = 0;
#else
  _invofunc_block = 0;
#endif
  _func_block = 0;
  _next_in_line = 0;
  _width = _height = 64;
  _funcnum = 0;
  if(!_func_count_table) _func_count_table = new FuncCountTable(100);
}

InvoComp::~InvoComp() {
  delete _funcname;
  delete _srcsizes;
  delete _dstsizes;
}

ClassId InvoComp::GetClassId () { return INVO_COMP; }

boolean InvoComp::IsA (ClassId id) {
    return INVO_COMP == id || PipeComp::IsA(id);
}

int InvoComp::nsrc() {
  int cnt=0;
  for (int i=0; i<_nsrcsizes; i++) cnt+= _srcsizes[i];
  return cnt;
}

void InvoComp::nsrc(int n) {
  if (_srcsizes) delete _srcsizes;
  _srcsizes = new int[1];
  _srcsizes[0] = n;
  _nsrcsizes = 1;
}

int InvoComp::ndst() {
  int cnt=0;
  for (int i=0; i<_ndstsizes; i++) cnt+= _dstsizes[i];
  return cnt;
}

void InvoComp::ndst(int n) {
  if (_dstsizes) delete _dstsizes;
  _dstsizes = new int[1];
  _dstsizes[0] = n;
  _ndstsizes = 1;
}

void InvoComp::srcsizes(int* nsrcs, int nnsrcs) {
  if (_srcsizes) delete _srcsizes;
  _srcsizes = new int[nnsrcs];
  _nsrcsizes = nnsrcs;
  for (int i=0; i<nnsrcs; i++) _srcsizes[i] = nsrcs[i];
  if(nnsrcs>1 && invopipes() && invopipes()->srccomp()) invopipes()->srccomp()->exclusive(1);
}

void InvoComp::dstsizes(int* ndsts, int nndsts) {
  if (_dstsizes) delete _dstsizes;
  _dstsizes = new int[nndsts];
  _ndstsizes = nndsts;
  for (int i=0; i<nndsts; i++) _dstsizes[i] = ndsts[i];
  if(nndsts>1 && invopipes() && invopipes()->dstcomp()) invopipes()->dstcomp()->exclusive(1);
}

void InvoComp::build_invocation()
{
    ComTerp* comterp = NULL;
    if (Component::use_unidraw())
	comterp = ((OverlayUnidraw*)unidraw)->comterp();
    else
	comterp = OverlayComp::comterp();
    if (!comterp) {
	fprintf(stderr, "no comterp found for InvoComp::build_invocation\n");
	return;
    }

    // handle special case funcs with unique internal tables
    int funcid = symbol_add((char *)_funcname);
    ComValue funcv(funcid, ComValue::SymbolType);
    ComValue funcv2(comterp->lookup_symval(funcv));
    IplIdrawComp* idrawcomp = (IplIdrawComp*)funcv2.geta(IplIdrawComp::class_symid());
    if (!idrawcomp) 
      func((ComFunc*)funcv2.obj_val());
    else {
      InvoDefFuncFunc* deffunc = new InvoDefFuncFunc(comterp);
      deffunc->defcomp(idrawcomp);
      func(deffunc);
    }

  build_pipes();
}

void InvoComp::build_pipes() {

  if (!invopipes()) {
    invopipes(new InvoPipesComp());
    invopipes()->invocomp(this);
    invopipes()->Append(new PipesComp(invopipes())); // srccomp
    invopipes()->Append(new PipesComp(invopipes())); // dstcomp
  }

  // handle nested argument lists
  Iterator it;
  invopipes()->srccomp()->First(it);
  if (invopipes()->srccomp()->Done(it)) {
    if (nsrcsizes()==1) {
      for (int i=0; i<nsrc(); i++)
	invopipes()->srccomp()->Append(new PipeComp((OverlayComp*)invopipes()->srccomp()));
    } else if(nsrcsizes()>1) {
      invopipes()->srccomp()->exclusive(true);
      for (int j=0; j<nsrcsizes(); j++) {
	PipesComp* pipescomp = new PipesComp((OverlayComp*)invopipes()->srccomp());
	invopipes()->srccomp()->Append(pipescomp);
	for (int i=0; i<srcsizes()[j]; i++)
	  pipescomp->Append(new PipeComp((OverlayComp*)invopipes()->srccomp()));
      }
    }
  }

  // #define FORK_DST_EXPERIMENT

  invopipes()->dstcomp()->First(it);
  if (invopipes()->dstcomp()->Done(it)) {
    if (ndstsizes()==1) {
      for (int i=0; i<ndst(); i++) {
#if defined(FORK_DST_EXPERIMENT)
	ForkComp* pipecomp = new ForkComp((OverlayComp*)invopipes()->dstcomp());
#else
	PipeComp* pipecomp = new PipeComp((OverlayComp*)invopipes()->dstcomp());
#endif
	// if (funcid==IAD_sym) pipecomp->buffsize(1024);
	invopipes()->dstcomp()->Append(pipecomp);
      }
    } else if (ndstsizes()>1) {
      invopipes()->dstcomp()->exclusive(true);
      for (int j=0; j<ndstsizes(); j++) {
	PipesComp* pipescomp = new PipesComp((OverlayComp*)invopipes()->dstcomp());
	invopipes()->dstcomp()->Append(pipescomp);
	for (int i=0; i<dstsizes()[j]; i++) {
#if defined(FORK_DST_EXPERIMENT)
	  ForkComp* pipecomp = new ForkComp((OverlayComp*)invopipes()->dstcomp());
#else
	  PipeComp* pipecomp = new PipeComp((OverlayComp*)invopipes()->dstcomp());
#endif
	  // if (funcid==IAD_sym) pipecomp->buffsize(1024);
	  pipescomp->Append(pipecomp);
	}
      }
    }
  }
}

void InvoComp::build_invocation_graphic()
{

  if (!Component::use_unidraw()) return;

  TextGraphic* name_textgr;
  TextGraphic* src_textgr;
  TextGraphic* dst_textgr;
  Picture* src_triangles;
  Picture* dst_triangles;
  Picture* sprites;

  SF_Rect* invo_rectgr;
  Picture* pict = (Picture*)GetGraphic();
  if (!pict) pict = init_picture();
  Transformer* savetrans = pict->GetTransformer() ? new Transformer(*pict->GetTransformer()) : new Transformer();
  Ref(savetrans);
  pict->SetTransformer(nil);

  if (!funcname() && func())
    _funcname = strnew(symbol_pntr(func()->funcid()));
  else if (!funcname())
    _funcname = strnew(symbol_pntr(namesym()));
  name_textgr = new TextGraphic(funcname() ? funcname() : "", pict);
  name_textgr->SetTransformer(new Transformer(_nametrans));

  #if 0
  // reset height based on number of pipes
  _height = Math::max(nsrc(), ndst())*triangle_height();
  #endif
  
  int width2 = _width/2; int height2 = _height/2;
  SF_Rect* invo_rect = new SF_Rect(0, 0, width2*2, height2*2, pict);
  invo_rect->SetTransformer(nil);
  invo_rect->Translate(-width2, -height2);
  src_textgr = new TextGraphic("", pict);
  src_textgr->SetTransformer(new Transformer(_nametrans));
  dst_textgr = new TextGraphic("", pict);
  dst_textgr->SetTransformer(new Transformer(_nametrans));

  int *trix, *triy;
  triangle_points(trix, triy);
  int srcdy = nsrc()+nsrcsizes() ? height2*2/(nsrc()+nsrcsizes()) : 0;
  src_triangles = new Picture(pict);
  int cnt=0;
  for (int i=0; i<nsrcsizes(); i++) {
    for (int j=0; j<srcsizes()[i]; j++) {
      SF_Polygon* triangle = new SF_Polygon(trix, triy, 3, pict);
      triangle->SetTransformer(nil);
      triangle->Translate(0, height2*2-srcdy*cnt+srcdy/2);
      src_triangles->Append(triangle);
      cnt++;
    }
    cnt++;
  }
  
  int dstdy = ndst()+ndstsizes() ? height2*2/(ndst()+ndstsizes()) : 0;
  dst_triangles = new Picture(pict);
  for (int i=0; i<ndstsizes(); i++) {
    for (int j=0; j<dstsizes()[i]; j++) {
      SF_Polygon* triangle = new SF_Polygon(trix, triy, 3, pict);
      triangle->SetTransformer(nil);
      triangle->Translate(0, height2*2-dstdy*cnt+dstdy/2);
      dst_triangles->Append(triangle);
      cnt++;
    }
    cnt++;
  }

  sprites = new Picture(pict);
  for (int i=0; i<nsrc(); i++)
    sprites->Append(new TextGraphic("", sprites));
  
  pict->Append(invo_rect, name_textgr, src_textgr, dst_textgr);
  pict->Append(src_triangles, dst_triangles);
  pict->Append(sprites);
  
  invo_rect->Align(CenterLeft, src_triangles, CenterRight);
  invo_rect->Align(CenterRight, dst_triangles, CenterLeft);

  invo_rect->Align(Center, name_textgr, Center);
  invo_rect->Align(TopLeft, src_textgr, BottomCenter);
  invo_rect->Align(BottomRight, dst_textgr, TopCenter);

  invo_rect->Align(BottomCenter, sprites, Center);

  pict->SetTransformer(savetrans);
  Unref(savetrans);

  NotifyLater();
}

void InvoComp::invofunc() {
#if !defined(COMPLETION_BLOCK)
  if (invofunc_block()) return;
#endif

#if 1
  // test to see if all destination pipes are empty
  LeafWalker walker(invopipes()->dstcomp());
  PipeComp* leaf;
  while (leaf = (PipeComp*)walker.NextLeaf()) {
    if (leaf->_dst_occupied()) {
      fprintf(stderr, "invofunc:  invofunc called when leaf->_dst_occupied() == %d\n", leaf->_dst_occupied());
      fprintf(stderr, "invofunc:  invofunc called when dstcomp()->src_vacant() == %d\n", invopipes()->dstcomp()->src_vacant());
      fprintf(stderr, "invofunc:  invofunc called when dstcomp()->_src_vacant() == %d\n", invopipes()->dstcomp()->_src_vacant());
      fprintf(stderr, "invofunc:  unexpected leaf node in dstcomp with data present\n");

      // debug like this
      _singlestep = 1;
      return;
    }
  }
#endif

#if defined(MANUAL_BREAKPOINT)
  if (func()->funcid()==_debug0_func && this==_debug0_comp) {
    fprintf(stderr, "manual conditional breakpoint reached:  %d, %lx\n", _debug0_func, (long unsigned)_debug0_comp);
  }
#endif

  int narg = 0;
  AttributeValue *tmpv = nil;
  #if defined(FLAT_ARGLIST)
  PipesComp* srccomp = invopipes()->srccomp();
  #else
  PipesComp* srccomp = next_complete();
  if (!srccomp) return;
  #endif

  // block premature completion from incoming values
#if defined(COMPLETION_BLOCK)
  enable_completion_block();
#else
  enable_invofunc_block();
#endif

  // get values out of incoming pipes and push on stack
  if (srccomp) {

#ifdef INVO_UNIT
    if (PipeComp::unitaryflag()) 
      invopipes()->srccomp()->ackwait(1);
#endif

    AttributeValueList * al = new AttributeValueList();
    Iterator it;
    srccomp->First(it);
    while (!srccomp->Done(it)) {
      PipeComp* srcpipe = (PipeComp*)srccomp->GetComp(it);

      if(srcpipe->_dst_occupied()) {
	if (srcpipe->dst_get(tmpv))
	  al->Append(tmpv);
	else
	  al->Append(new AttributeValue(ComValue::blankval()));
      }
      else
	al->Append(new AttributeValue(ComValue::blankval()));
      srccomp->Next(it);
      narg++;
    }

    // do this after all src's have been pulled
    Iterator jt;
    for (al->First(jt); !al->Done(jt); al->Next(jt))
      func()->comterp()->push_stack(*al->GetAttrVal(jt));
    delete al;
  }

  // execute primitive
  func()->context(this);
  func()->exec(narg, 0);  

  // pop result off stack and put into outgoing pipe(s)
  if (invopipes()->dstcomp()) {
    PipesComp* dstcomp = invopipes()->dstcomp();
    ComValue topval(func()->comterp()->pop_stack());
    AttributeValue resultv(topval);
    boolean putflag = false;
    if (resultv.is_blank()) {
      // no output generated
    }
    else if (resultv.is_array()) {
      AttributeValueList *al = resultv.array_val();
      ALIterator at;
      al->First(at);
      LeafWalker walker(dstcomp);
      OverlayComp* treenode = dstcomp;
      while ((treenode = walker.NextLeaf()) && !al->Done(at)) {

	// skip blank outputs
	if (al->GetAttrVal(at)->is_blank()) {
	  al->Next(at);
	  continue;
	}

	PipeComp* dstpipe = (PipeComp*)treenode;
	tmpv = new AttributeValue(al->GetAttrVal(at));
	boolean flag = dstpipe->src_put(tmpv);
	putflag = putflag || flag;
	al->Next(at);
      }
      #if 0
      if (!dstcomp->Done(it) || !al->Done(at))
	fprintf(stderr, "mismatch in generated output and number of output pipes\n");
      #endif
    } else {
      Iterator it;
      dstcomp->First(it);
      if (!dstcomp->Done(it)) {
	if (!resultv.is_blank()) {
	  PipeComp* dstpipe = (PipeComp*)dstcomp->GetComp(it);
	  tmpv = new AttributeValue(resultv);
	  putflag = dstpipe->src_put(tmpv);
	  dstcomp->Next(it);
	  if (!dstcomp->Done(it)) 
	    fprintf(stderr, "single output generated but multiple pipes present\n");
	}
      } else
	fprintf(stderr, "output generated but no output pipes\n");

    }

    // notify as necessary 
    if (putflag) {

      // alustyle completion:  individually notify subpipes
      if (alustyle(false /* !srcflag */) && alucomplete(false /* !srcflag */)) {
	LeafWalker walker(dstcomp);
	OverlayComp* treenode;
	while (treenode = walker.NextLeaf()) {
	  PipeComp* pipecomp = (PipeComp*)treenode;
	  if (pipecomp->_dst_occupied()) {
	    if (!_eventqueueflag)
	      pipecomp->_dst_notify();
	    else {
	      pipecomp->dst_notified(1);
	      if (!_eventqueue) _eventqueue = new PipeEventQueue();
	      _eventqueue->append(new PipeEvent(pipecomp, true /* notify_flag */));
	    }
	  }
	}
      }
      
      // regular completion:  aggregately notify subpipes
      else if (dstcomp->dst_occupied()) // maybe completion testing can be completely disabled on output
	dstcomp->dst_notify();

    } else {
      static const int SINK_sym = symbol_add("SINK");
      static const int IAD_sym = symbol_add("IAD");
      if (func()->funcid()==SINK_sym || func()->funcid()==IAD_sym)  {
	update_dsttext();
	if(invopipes()->srccomp()->ackwait()) {
	  invopipes()->srccomp()->ackwait(0);
	  invopipes()->srccomp()->src_request();
	}
      }
    }
  }


#if defined(COMPLETION_BLOCK)
  disable_completion_block();
#else
  disable_invofunc_block();
#endif

  // test to see if ready for next firing
  if (!invopipes()->srccomp()->ackwait() && invopipes()->srccomp()->dst_occupied() && invopipes()->dstcomp()->src_vacant()) invofunc();
}

ParamList* InvoComp::GetParamList() {
    if (!_invo_params) 
	GrowParamList(_invo_params = new ParamList());
    return _invo_params;
}

void InvoComp::GrowParamList(ParamList* pl) {
    pl->add_param("width", ParamStruct::keyword, &ParamList::read_int,
		  this, &_width);
    pl->add_param("height", ParamStruct::keyword, &ParamList::read_int,
		  this, &_height);
    pl->add_param("nsrc", ParamStruct::keyword, &ParamList::read_ints,
		  this, &_srcsizes, &_nsrcsizes);
    pl->add_param("ndst", ParamStruct::keyword, &ParamList::read_ints,
		  this, &_dstsizes, &_ndstsizes);
    pl->add_param("funcname", ParamStruct::keyword, &ParamList::read_string,
		  this, &_funcname);
    pl->add_param("rotate", ParamStruct::keyword, &InvoScript::ReadTextRotate,
		  this, &_nametrans);
    pl->add_param("nametrans", ParamStruct::keyword, &InvoScript::ReadNameTransform,
		  this, &_nametrans);
    NodeComp::GrowParamList(pl);
    return;
}

PipeComp* InvoComp::find_subpipe(boolean srcflag, int index) {
  if (!invopipes()) return nil;
  return invopipes()->find_subpipe(srcflag, index);
}

Component* InvoComp::Copy() {
    InvoComp* comp = CopyOnly();
    comp->build_invocation();
    return comp;
}

InvoComp* InvoComp::CopyOnly() {
    InvoComp* comp = nil;
    comp = GetGraphic() ? new InvoComp((Picture*)GetGraphic()->Copy()) : new InvoComp();
    if (attrlist()) comp->SetAttributeList(new AttributeList(attrlist()));
    comp->funcname(funcname());
    comp->namesym(namesym());
    comp->srcsizes(srcsizes(), nsrcsizes());
    comp->dstsizes(dstsizes(), ndstsizes());
    comp->nametrans(nametrans()); 
    comp->width(width());
    comp->height(height());
    return comp;
}

void InvoComp::update_srctext(PipeComp* pipe) {
  if (Component::use_unidraw() && GetGraphic()) {

    if (!animateflag() || !eventqueueflag()) {
      std::strstreambuf sbuf;
      ostream outs(&sbuf);
      if (subbuff_count(true)>0)
	outs << "{" << *invopipes()->srccomp() << "}";
      outs << '\0';
      char* sbufstr = sbuf.str();
      ChangeSrcText(sbufstr);
#if !defined(BLUER_ANIMATION)
      if (strlen(sbufstr))
#else
      if (invopipes()->srccomp()->complete())
#endif
	  GetGraphic()->SetColors(psred, pswhite);
#if 0
      else
	  GetGraphic()->SetColors(psblue, pswhite);
#endif
      NotifyLater();
      unidraw->Update();
      // fprintf(stderr, "src update --> %s\n", sbuf.str());
      delete sbufstr;
    } 
    
    else {

      /* upstream location */
      float old_cx, old_cy;
      int srcflag;
      int index = find_subpipe_index(pipe, srcflag);
      int order;
      PipeComp* upstream = remote_srccomp(index, order, false);
      if (upstream->IsA(INVO_COMP)) {
	int ncx, ncy;
	((InvoComp*)upstream)->get_triangle_loc(0, order, ncx, ncy);
	old_cx = ncx+10;
	old_cy = ncy;
      } else
	upstream->GetGraphic()->GetCenter(old_cx, old_cy);
      

      /* downstream location */
      float new_cx, new_cy;
#if 0
      GetGraphic()->GetCenter(new_cx, new_cy);
#else
      int ncx, ncy;
      get_triangle_loc(1, index, ncx, ncy);
      new_cx = ncx;
      new_cy = ncy;
#endif

      /* set up graphic */
      std::strstreambuf sbuf;
      ostream outs(&sbuf);
      outs << *pipe->buff() << '\0';

      char* sbufstr = sbufstr;
      if (strlen(sbufstr)!=0) {
	ChangeSpriteText(index, sbufstr);
	sprite(new Sprite(pipe, old_cx, old_cy, new_cx-old_cx, new_cy-old_cy, 0, this));
      } 
      
      else {
	ChangeSpriteText(index, "@");
	Iterator it;
	Picture* sp = GetSpritePict();
	sp->First(it);
	for(int i=0; i<index; i++) sp->Next(it);
	NotifyLater();
	sprite(new Sprite(pipe, new_cx, new_cy, old_cx-new_cx, old_cy-new_cy, 1, this));
      }
      delete sbufstr;
      SpriteQueue* spritequeue = SpriteQueue::instance();
      spritequeue->Append(sprite());
    }
    
  }
}

void InvoComp::update_dsttext(PipeComp* pipe) {
  static const int SINK_sym = symbol_add("SINK");

  if (Component::use_unidraw() && GetGraphic()) {
    
    std::strstreambuf sbuf;
    ostream outs(&sbuf);
    if (func()->funcid()==SINK_sym) {
      outs << *buff();
    } else {
      if (subbuff_count(false)>0)
	if (!animateflag() || !eventqueueflag())
	  outs << "{" << *invopipes()->dstcomp() << "}";
	else
	  outs << *invopipes()->dstcomp();
    }
    outs << '\0';
    char* sbufstr = sbuf.str();
    ChangeDstText(sbufstr);
    if(animateflag()&&eventqueueflag()) {
      int srcflag;
      int index = find_subpipe_index(pipe, srcflag);
      int ncx, ncy;
      get_triangle_loc(0, index, ncx, ncy);
      MoveDstTextTo((float)ncx+10, (float)ncy);
    }
    if (!strlen(sbufstr) || func()->funcid()==SINK_sym) {

#if !defined BLUER_ANIMATION
      boolean new_input = false;

      // check to see if any input has already arrived
#if defined(FLAT_ARGLIST)
      Iterator it;
      PipesComp* srcpipes = invopipes()->srccomp();
      for(srcpipes->First(it); !srcpipes->Done(it); srcpipes->Next(it)) {
	PipeComp* pipecomp = (PipeComp*)srcpipes->GetComp(it);
#else
      OverlayComp* before = nil;
      OverlayComp* treenode = srcpipes;
      while (treenode && treenode != srcpipes->GetParent()) {
	OverlayComp* after = treenode->DepthNext(before);
	before = treenode;
	treenode = after;
	if (!treenode->IsA(PIPE_COMP)) continue;
	PipeComp* pipecomp = (PipeComp*)treenode;
#endif
	if (pipecomp->buff()->Number()>0) {
	  new_input = true;
	  break;
	}
      }

      if (!new_input) GetGraphic()->SetColors(psblue, pswhite);
      if (current_outconn_color() != psblack) color_outconn(psblue);
#else
      if (!invopipes()->srccomp()->complete()) 
	GetGraphic()->SetColors(psblue, pswhite);
      color_outconn(psblue);
#endif
    }
    else {
      if (invopipes()->dstcomp()->dst_occupied()) {
	PipesComp* dstpipes = invopipes()->dstcomp();
#if defined(FLAT_ARGLIST)
	Iterator it;
	for(dstpipes->First(it); !dstpipes->Done(it); dstpipes->Next(it)) {
	  PipeComp* pipecomp = (PipeComp*)dstpipes->GetComp(it);
#else
        OverlayComp* before = nil;
	OverlayComp* treenode = dstpipes;
	while (treenode && treenode != dstpipes->GetParent()) {
	  OverlayComp* after = treenode->DepthNext(before);
	  before = treenode;
	  treenode = after;
	  if (!treenode->IsA(PIPE_COMP)) continue;
	  PipeComp* pipecomp = (PipeComp*)treenode;
#endif
	  if (pipecomp->buff()->Number()>0)
	    if (pipecomp->_dst_occupied()) pipecomp->color_outconn(psred);
	  else 
	    pipecomp->color_outconn(psblue);
	}
      }
    }
    delete sbufstr;
    NotifyLater();
    unidraw->Update();
    // fprintf(stderr, "dst update --> %s\n", sbuf.str());
  }
}

void InvoComp::ChangeSrcText(const char* str) {
#if 0
  TextGraphic* tg_new = new TextGraphic(str, stdgraphic);
  SetSrcText(tg_new);
#else
  GetSrcText()->SetOriginal(str);
  Picture* pict = (Picture*)GetGraphic();
  Transformer * t = pict->GetTransformer();
  Alignment rect_align;
  Alignment text_align;
  boolean rot90 = t->Rotated90();
  if (!t->xflipped() && !t->yflipped()) {
    rect_align = rot90 ? BottomLeft : TopLeft;
    text_align = rot90 ? CenterRight : BottomCenter;
  } else if (!t->yflipped()) {
    rect_align = rot90 ? BottomRight : TopRight;
    text_align = rot90 ? CenterLeft : BottomCenter;
  } else if (!t->xflipped()) {
    rect_align = rot90 ? TopLeft : BottomLeft;
    text_align = rot90 ? CenterRight : TopCenter;
  } else {
    rect_align = rot90 ? TopRight : BottomRight;
    text_align = rot90 ? CenterLeft : TopCenter;
  }
  GetRect()->Align(rect_align, GetSrcText(), text_align);
  NotifyLater();
#endif
}

void InvoComp::SetSrcText(TextGraphic* tg_new) {
  TextGraphic* tg_old = (GetSrcText());
  if (tg_old) {
    ((Picture*)GetGraphic())->Remove(tg_old);
    delete tg_old;
  }
  Picture* pict = (Picture*)GetGraphic();
  Iterator it;
  pict->First(it); 
  pict->Next(it);
  pict->InsertAfter(it, tg_new);

  tg_new->SetTransformer(new Transformer(nametrans()));

  Transformer * t = pict->GetTransformer();
  Alignment rect_align;
  Alignment text_align;
  boolean rot90 = t->Rotated90();
  if (!t->xflipped() && !t->yflipped()) {
    rect_align = rot90 ? BottomLeft : TopLeft;
    text_align = rot90 ? CenterRight : BottomCenter;
  } else if (!t->yflipped()) {
    rect_align = rot90 ? BottomRight : TopRight;
    text_align = rot90 ? CenterLeft : BottomCenter;
  } else if (!t->xflipped()) {
    rect_align = rot90 ? TopLeft : BottomLeft;
    text_align = rot90 ? CenterRight : TopCenter;
  } else {
    rect_align = rot90 ? TopRight : BottomRight;
    text_align = rot90 ? CenterLeft : TopCenter;
  }
  GetRect()->Align(rect_align, tg_new, text_align);

  NotifyLater();
}

void InvoComp::ChangeDstText(const char* str) {
#if 0
  TextGraphic* tg_new = new TextGraphic(str, stdgraphic);
  SetDstText(tg_new);
#else
  GetDstText()->SetOriginal(str);
  Picture* pict = (Picture*)GetGraphic();
  Transformer * t = pict->GetTransformer();
  Alignment rect_align;
  Alignment text_align;
  boolean rot90 = t->Rotated90();
  if (!t->xflipped() && !t->yflipped()) {
    rect_align = rot90 ? TopRight : BottomRight;
    text_align = rot90 ? CenterLeft : TopCenter;
  } else if (!t->yflipped()) {
    rect_align = rot90 ? TopLeft : BottomLeft;
    text_align = rot90 ? CenterRight : TopCenter;
  } else if (!t->xflipped()) {
    rect_align = rot90 ? BottomRight : TopRight;
    text_align = rot90 ? CenterLeft : BottomCenter;
  } else {
    rect_align = rot90 ? BottomLeft : TopLeft;
    text_align = rot90 ? CenterRight : BottomCenter;
  }
  GetRect()->Align(rect_align, GetDstText(), text_align);
  NotifyLater();
#endif
}

void InvoComp::SetDstText(TextGraphic* tg_new) {
  TextGraphic* tg_old = (GetDstText());
  if (tg_old) {
    ((Picture*)GetGraphic())->Remove(tg_old);
    delete tg_old;
  }
  Picture* pict = (Picture*)GetGraphic();
  Iterator it;
  pict->First(it); 
  pict->Next(it);
  pict->Next(it);
  pict->InsertAfter(it, tg_new);

  tg_new->SetTransformer(new Transformer(nametrans()));

  Transformer * t = pict->GetTransformer();
  Alignment rect_align;
  Alignment text_align;
  boolean rot90 = t->Rotated90();
  if (!t->xflipped() && !t->yflipped()) {
    rect_align = rot90 ? TopRight : BottomRight;
    text_align = rot90 ? CenterLeft : TopCenter;
  } else if (!t->yflipped()) {
    rect_align = rot90 ? TopLeft : BottomLeft;
    text_align = rot90 ? CenterRight : TopCenter;
  } else if (!t->xflipped()) {
    rect_align = rot90 ? BottomRight : TopRight;
    text_align = rot90 ? CenterLeft : BottomCenter;
  } else {
    rect_align = rot90 ? BottomLeft : TopLeft;
    text_align = rot90 ? CenterRight : BottomCenter;
  }
  GetRect()->Align(rect_align, tg_new, text_align);
  NotifyLater();
}

void InvoComp::MoveDstTextTo(float locx, float locy) {
  float cx, cy;
  GetDstText()->GetCenter(cx, cy);
  GetDstText()->Translate(locx-cx, locy-cy);
  NotifyLater();
}

SF_Rect* InvoComp::GetRect() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  return (SF_Rect*)pic->GetGraphic(it);
}

TextGraphic* InvoComp::GetFuncText() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  return (TextGraphic*)pic->GetGraphic(it);
}


void InvoComp::SetFuncText(TextGraphic* tg_new) {
  TextGraphic* tg_old = (GetFuncText());
  if (tg_old) {
    ((Picture*)GetGraphic())->Remove(tg_old);
    delete tg_old;
  }
  Picture* pict = (Picture*)GetGraphic();
  Iterator it;
  pict->First(it); 
  pict->Next(it);
  pict->InsertAfter(it, tg_new);

  GetRect()->Align(Center, tg_new, Center);
  NotifyLater();
}


void InvoComp::ChangeFuncText(const char* str) {
  TextGraphic* tg_new = new TextGraphic(str, stdgraphic);
  SetFuncText(tg_new);
}

TextGraphic* InvoComp::GetSrcText() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  return (TextGraphic*)pic->GetGraphic(it);
}

TextGraphic* InvoComp::GetDstText() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  return (TextGraphic*)pic->GetGraphic(it);
}

Picture* InvoComp::GetSrcPict() {
  Picture* pic = (Picture*)GetGraphic();
  Iterator it;
  pic->First(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  pic->Next(it);
  return (Picture*)pic->GetGraphic(it);
}

Picture* InvoComp::GetDstPict() {
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

Picture* InvoComp::GetSpritePict() {
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

void InvoComp::ChangeSpriteText(int index, const char* text) {
  Picture* sprites = GetSpritePict();
  Iterator it;
  sprites->First(it);
  for(int i=0; i<index; i++) sprites->Next(it);
  TextGraphic* tg_old = (TextGraphic*) sprites->GetGraphic(it);
  tg_old->SetOriginal(text);
#if 0
  Transformer t;
  if (tg_old) {
    if (tg_old->GetTransformer())
      t = *tg_old->GetTransformer();
    sprites->Remove(tg_old);
    delete tg_old;
  }
  TextGraphic* tg_new = new TextGraphic(text, stdgraphic);
  tg_new->SetTransformer(new Transformer(t));
  sprites->Append(tg_new);
  GetGraphic()->Align(1, tg_new, 7);
  tg_old->Translate(100,100);
#endif
  NotifyLater();
}

void InvoComp::get_triangle_loc(int srcflag, int pipeindex, int& x, int& y) {
  Picture* trianglepict = srcflag ? GetSrcPict() : GetDstPict();
  Iterator it;
  trianglepict->First(it);
  int i=0;
  while (i<pipeindex && !trianglepict->Done(it)) {
    trianglepict->Next(it);
    i++;
  }
  SF_Polygon* triangle = (SF_Polygon*)trianglepict->GetGraphic(it);
  if (srcflag) {
    Coord x0, y0, x1, y1;
    triangle->GetPoint(0, x0, y0);
    triangle->GetPoint(1, x1, y1);
    x = (x0+x1)/2;
    y = (y0+y1)/2;
  } else 
    triangle->GetPoint(2, x, y);

}

int InvoComp::find_subpipe_index(PipeComp* comp, int& srcflag) {
#if defined(FLAT_ARGLIST)
  PipesComp* srcpipes = invopipes()->srccomp();
  int index = 0;
  Iterator it;
  srcpipes->First(it);
  while (srcpipes->GetComp(it) != comp && !srcpipes->Done(it)) {
    index++;
    srcpipes->Next(it);
  }
  if (!srcpipes->Done(it) && srcpipes->GetComp(it)) {
    srcflag = 1;
    return index;
  }

  PipesComp* dstpipes = invopipes()->dstcomp();
  index = 0;
  dstpipes->First(it);
  while (dstpipes->GetComp(it) != comp && !dstpipes->Done(it)) {
    index++;
    dstpipes->Next(it);
  }
  if (!dstpipes->Done(it) && dstpipes->GetComp(it)) {
    srcflag = 0;
    return index;
  }
#else
  PipesComp* srcpipes = invopipes()->srccomp();
  int index = 0;
  OverlayComp* before = nil;
  OverlayComp* treenode = srcpipes;
  while (treenode && treenode != srcpipes->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (treenode==comp) {
      srcflag = true;
      return index;
    }
    if (treenode && treenode->IsA(PIPE_COMP)) index++;
  }

  PipesComp* dstpipes = invopipes()->dstcomp();
  before = nil;
  treenode = dstpipes;
  index = 0;
  while (treenode && treenode != dstpipes->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (treenode==comp) {
      srcflag = false;
      return index;
    }
    if (treenode && treenode->IsA(PIPE_COMP)) index++;
  }
#endif

  return -1;
}

int InvoComp::subbuff_count(boolean srcflag) {
  PipesComp* pipescomp = srcflag ? invopipes()->srccomp() : invopipes()->dstcomp();
  int count = 0;
#if defined(FLAT_ARGLIST)
  Iterator it;
  for (pipescomp->First(it); !pipescomp->Done(it); pipescomp->Next(it)) {
    PipeComp* pipecomp = (PipeComp*)pipescomp->GetComp(it);
    if (!pipecomp->IsA(PIPE_COMP)) {
      fprintf(stderr, "time to handle nested PipeComps case\n");
      return -1;
    }
    count += pipecomp->buff()->Number();
  }
#else
  OverlayComp* before = nil;
  OverlayComp* treenode = pipescomp;
  while (treenode && treenode != pipescomp->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (treenode && treenode->IsA(PIPE_COMP))
      count += ((PipeComp*)treenode)->buff()->Number();
  }
#endif
  return count;
}

void InvoComp::Interpret(Command* cmd) {
    if (cmd->IsA(MOVE_CMD)) {
        float dx, dy;
        ((MoveCmd*) cmd)->GetMovement(dx, dy);
	GetGraphic()->Translate(dx, dy);
        NotifyLater();

	Editor* ed = cmd->GetEditor();
	Iterator i;
	if (invopipes()) {
	  PipesComp* srccomp = invopipes()->srccomp();
	  PipesComp* dstcomp = invopipes()->dstcomp();

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
	    TopoNode* node = ((PipeComp*)srccomp->GetComp(j))->Node();
#endif
	    for (node->first(i); !node->done(i); node->next(i)) {
	      TopoEdge* edge = node->edge(node->elem(i));
	      EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
	      eucmd.Execute();
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
	    TopoNode* node = ((PipeComp*)dstcomp->GetComp(j))->Node();
#endif
	    for (node->first(i); !node->done(i); node->next(i)) {
	      TopoEdge* edge = node->edge(node->elem(i));
	      EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
	      eucmd.Execute();
	    }
	  }
	}
    }

    else if (cmd->IsA(ROTATE_CMD)) {

        // counter-rotate the text
        float angle, cx, cy;
	angle = ((RotateCmd*) cmd)->GetRotation();
#if 1
	_nametrans.rotate(-angle);
#else
        _rotate += angle;
#endif

	TextGraphic* functext = GetFuncText();
	functext->GetCenter(cx, cy);
	functext->Rotate(-angle, cx, cy);
	TextGraphic* srctext = GetSrcText();
	srctext->GetCenter(cx, cy);
	srctext->Rotate(-angle, cx, cy);
	TextGraphic* dsttext = GetDstText();
	dsttext->GetCenter(cx, cy);
	dsttext->Rotate(-angle, cx, cy);
        NodeComp::Interpret(cmd);

	update_conns(cmd->GetEditor());

    } else if (cmd->IsA(SCALE_CMD)) {

        float sx, sy;
        ScaleCmd* scaleCmd = (ScaleCmd*) cmd;
        scaleCmd->GetScaling(sx, sy);
	TextGraphic* functext = GetFuncText();
	// handle flip vertical
	if (sx==1.0 && sy==-1.0) {
	  if (functext->GetTransformer()) 
	    functext->GetTransformer()->flipy();
	  else
	    functext->SetTransformer(new Transformer(1,0,0,-1,0,0));
	}

	// handle flip horizontal
	else if (sx==-1.0 && sy==1.0) {
	  if (functext->GetTransformer()) 
	    functext->GetTransformer()->flipx();
	  else
	    functext->SetTransformer(new Transformer(-1,0,0,1,0,0));
	}
	GetRect()->Align(Center, functext, Center);

	nametrans(*functext->GetTransformer());

	NodeComp::Interpret(cmd);

	update_conns(cmd->GetEditor());

    } else if (cmd->IsA(REORIENT_TEXT_CMD)) {
      Transformer tpic;
      if (GetGraphic()->GetTransformer()) tpic = *GetGraphic()->GetTransformer();
      tpic.Invert();
      float a00, a01, a10, a11, a20, a21;
      tpic.matrix(a00, a01, a10, a11, a20, a21);
      Transformer tname(a00, a01, a10, a11, 0.0, 0.0);
      nametrans(tname);
      GetFuncText()->SetTransformer(new Transformer(nametrans()));
      GetRect()->Align(Center, GetFuncText(), Center);
      DirtyCmd dcmd(cmd->GetEditor());
      dcmd.Execute();
      NotifyLater();
      unidraw->Update();
    } else
	NodeComp::Interpret(cmd);
}

void InvoComp::Uninterpret(Command* cmd) {
    if (cmd->IsA(MOVE_CMD)) {
        float dx, dy;
        ((MoveCmd*) cmd)->GetMovement(dx, dy);
	GetGraphic()->Translate(-dx, -dy);
        NotifyLater();

	Editor* ed = cmd->GetEditor();
	Iterator i;
	PipesComp* srccomp = invopipes()->srccomp();
	PipesComp* dstcomp = invopipes()->dstcomp();

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
	  TopoNode* node = ((PipeComp*)srccomp->GetComp(j))->Node();
#endif
	  for (node->first(i); !node->done(i); node->next(i)) {
	    TopoEdge* edge = node->edge(node->elem(i));
	    EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
	    eucmd.Execute();
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
	  TopoNode* node = ((PipeComp*)dstcomp->GetComp(j))->Node();
#endif
	  for (node->first(i); !node->done(i); node->next(i)) {
	    TopoEdge* edge = node->edge(node->elem(i));
	    EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
	    eucmd.Execute();
	  }
	}

    }
    else if (cmd->IsA(ROTATE_CMD)) {

        // counter-rotate the text
        float angle, cx, cy;
	angle = ((RotateCmd*) cmd)->GetRotation();
	TextGraphic* functext = GetFuncText();
	functext->GetCenter(cx, cy);
	functext->Rotate(angle, cx, cy);
	TextGraphic* srctext = GetSrcText();
	srctext->GetCenter(cx, cy);
	srctext->Rotate(angle, cx, cy);
	TextGraphic* dsttext = GetDstText();
	dsttext->GetCenter(cx, cy);
	dsttext->Rotate(angle, cx, cy);
        NodeComp::Interpret(cmd);

	update_conns(cmd->GetEditor());

    } else if (cmd->IsA(SCALE_CMD)) {

        float sx, sy;
        ScaleCmd* scaleCmd = (ScaleCmd*) cmd;
        scaleCmd->GetScaling(sx, sy);
	TextGraphic* functext = GetFuncText();
	// handle flip vertical
	if (sx==1.0 && sy==-1.0) 
	  functext->GetTransformer()->flipy();
	// handle flip horizontal
	else if (sx==-1.0 && sy==1.0)
	  functext->GetTransformer()->flipx();

	GetRect()->Align(Center, functext, Center);

	nametrans(*functext->GetTransformer());

	NodeComp::Uninterpret(cmd);

	update_conns(cmd->GetEditor());

    }
    else
	NodeComp::Uninterpret(cmd);
}

void InvoComp::color_outconn(PSColor* color) {
  PipesComp* dstpipes = invopipes()->dstcomp();
  if (!dstpipes) return;
#if defined(FLAT_ARGLIST)
  Iterator it;
  for(dstpipes->First(it); !dstpipes->Done(it); dstpipes->Next(it)) {
    PipeComp* pipecomp = (PipeComp*)dstpipes->GetComp(it);
#else
  OverlayComp* before = nil;
  OverlayComp* treenode = dstpipes;
  while (treenode && treenode != dstpipes->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (!treenode || !treenode->IsA(PIPE_COMP)) continue;
    PipeComp* pipecomp = (PipeComp*)treenode;
#endif
    if (pipecomp->current_outconn_color()==psblack && color==psblue) continue;
    pipecomp->color_outconn(color);
  }
}

void InvoComp::update_conns(Editor* ed) {
#if defined(FLAT_ARGLIST)
  Iterator i, j, k;
  for(invopipes()->First(k); !invopipes()->Done(k); invopipes()->Next(k)) {
    PipesComp* pipes = (PipesComp*)invopipes()->GetComp(k);
    for (pipes->First(j); !pipes->Done(j); pipes->Next(j)) {
      TopoNode* node = ((PipeComp*)pipes->GetComp(j))->Node();
      for (node->first(i); !node->done(i); node->next(i)) {
	TopoEdge* edge = node->edge(node->elem(i));
	EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
	eucmd.Execute();
      }
    }
  }
#else
  OverlayComp* before = nil;
  OverlayComp* treenode = invopipes();
  while (treenode && treenode != invopipes()->GetParent()) {
    OverlayComp* after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (!treenode || !treenode->IsA(PIPE_COMP)) continue;
    TopoNode* node = ((PipeComp*)treenode)->Node();
    Iterator i;
    for (node->first(i); !node->done(i); node->next(i)) {
      TopoEdge* edge = node->edge(node->elem(i));
      EdgeUpdateCmd eucmd(ed, (EdgeComp*)edge->value());
      eucmd.Execute();
    }
  }
#endif
}
 
PipesComp* InvoComp::next_complete() {
  PipesComp* srccomp = invopipes()->srccomp();
  int srccomp_numleaf = srccomp->numleaf();
  int count = 0;
  OverlayComp* before = nil;
  OverlayComp* treenode = srccomp;
  OverlayComp* after;
  
  // find the next subpipe to start checking for completion
  do {
    after = treenode->DepthNext(before);
    before = treenode;
    treenode = after;
    if (treenode && treenode->IsA(PIPE_COMP))
      count++;
  } while (treenode && treenode != srccomp->GetParent() && count-1 != next_in_line());
  count--;
  
  OverlayComp* firstnode = treenode;
  do {

    // find the first exclusive ancestor
    PipesComp* ancestor = (PipesComp*)treenode->GetParent();
    while (ancestor->GetParent() && !((PipesComp*)ancestor->GetParent())->exclusive()) ancestor = (PipesComp*)ancestor->GetParent();
    
    // wrap around 
    int ancestor_numleaf = ancestor->numleaf();
    if (count + ancestor_numleaf >= srccomp_numleaf) {
      treenode = srccomp;
      before = nil;
      do {
	
	after = treenode->DepthNext(before);
	before = treenode;
	treenode = after;
      } while (!treenode->IsA(PIPE_COMP));
      count = 0;
    }

    // or not wrap around
    else {
      for (int i=0; i<ancestor_numleaf; i++) {
	do {
	  after = treenode->DepthNext(before);
	  before = treenode;
	  treenode = after;
	} while (!treenode->IsA(PIPE_COMP));
	count++;
      }
    }

    if (ancestor->dst_occupied()) {
      next_in_line(count);
      return ancestor;
    }
    
  } while (firstnode != treenode);

  return nil;

}

PipeComp* InvoComp::remote_srccomp(int index, int& order, boolean lowlevel) {
  LeafWalker walker(invopipes()->srccomp());
  PipeComp* leaf;
  int count=0;
  while (leaf = (PipeComp*)walker.NextLeaf()) {
    if (count==index)
      return leaf->remote_srccomp(0, order, lowlevel);
    count++;
  }
  order = -1;
  return nil;
}

PipeComp* InvoComp::remote_dstcomp(int index, int& order, boolean lowlevel) {
  LeafWalker walker(invopipes()->dstcomp());
  PipeComp* leaf;
  int count=0;
  while (leaf = (PipeComp*)walker.NextLeaf()) {
    if (count==index)
      return leaf->remote_dstcomp(0, order, lowlevel);
    count++;
  }
  order = -1;
  return nil;
}

 PipeComp* InvoComp::remote_dstcomp_fanout(int index, int fanout, int& order, boolean lowlevel) {
  LeafWalker walker(invopipes()->dstcomp());
  PipeComp* leaf;
  int count=0;
  while (leaf = (PipeComp*)walker.NextLeaf()) {
    if (count==index)
      return leaf->remote_dstcomp(fanout, order, lowlevel);
    count++;
  }
  order = -1;
  return nil;
}

int InvoComp::remote_dstcomps(int index, PipeComp** pipecomps, int* orders, int maxpipecomps, boolean lowlevel) {
  LeafWalker walker(invopipes()->dstcomp());
  PipeComp* leaf;
  int count=0;
  while (leaf = (PipeComp*)walker.NextLeaf()) {
    if (count==index) {
      int npipecomps = 0;
      int order;
      do {
	pipecomps[npipecomps] = 
	  leaf->remote_dstcomp(npipecomps, orders[npipecomps], lowlevel);
      } while(pipecomps[npipecomps++]!=nil && npipecomps<maxpipecomps);
      return npipecomps;
    }
    count++;
  }
  return 0;
}

void InvoComp::func(ComFunc* funcptr) {
  _func = funcptr;
  if (_func) {
    int count;
    if(!_func_count_table->find_and_remove(count, _func->funcid())) count=0;
    _funcnum = count;
    count++;
    _func_count_table->insert(_func->funcid(), count); 
  }
}


const char* InvoComp::funcname() { 
  if (!_funcname && func()) 
    _funcname = strnew(symbol_pntr(func()->funcid())); 
  return _funcname; 
}

/****************************************************************************/

ParamList* IplIdrawComp::_ipl_idraw_params = nil;
int IplIdrawComp::_symid = -1;

IplIdrawComp::IplIdrawComp (const char* pathname, OverlayComp* parent) : DrawIdrawComp(pathname, parent) 
{ 
}

ClassId IplIdrawComp::GetClassId () { return IPL_IDRAW_COMP; }

boolean IplIdrawComp::IsA (ClassId id) {
    return IPL_IDRAW_COMP == id || DrawIdrawComp::IsA(id);
}

IplIdrawComp::IplIdrawComp (istream& in, const char* pathname, OverlayComp* parent) : DrawIdrawComp(pathname, parent) {
    _valid = GetParamList()->read_args(in, this);
}

ParamList* IplIdrawComp::GetParamList() {
    if (!_ipl_idraw_params) 
	GrowParamList(_ipl_idraw_params = new ParamList());
    return _ipl_idraw_params;
}

void IplIdrawComp::GrowParamList(ParamList* pl) {
    DrawIdrawComp::GrowParamList(pl); 
}

Component* IplIdrawComp::Copy () {
    return new IplIdrawComp((const char*)nil, nil);
}

/****************************************************************************/

int DistantComp::_symid = -1;
AttributeValueList* DistantComp::_distant_list = nil;
int DistantComp::_first_cycle = 0;
int DistantComp::_cycle_count = 0;
ComValue* DistantComp::_statval = nil;
int DistantComp::_nostats = 0;

DistantComp::DistantComp(SF_Ellipse* ellipse, TextGraphic* txt, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
  : PipeComp(ellipse, txt, txt2, rl, parent)
{
  init();
}

DistantComp::DistantComp(SF_Ellipse* ellipse, TextGraphic* txt, SF_Ellipse* ellipse2, 
    GraphComp* graph, TextGraphic* txt2, boolean rl, OverlayComp* parent) 
  : PipeComp(ellipse, txt, ellipse2, graph, txt2, rl, parent)
{
  init();
}

DistantComp::DistantComp(Picture* pic, boolean rl, OverlayComp* parent) 
    : PipeComp(pic, rl, parent)
{
  init();
}

DistantComp::DistantComp(GraphComp* graph) 
    : PipeComp(graph)
{
  init();
}

DistantComp::DistantComp(OverlayComp* parent) : PipeComp( parent) 
{
  init();
}

DistantComp::DistantComp(istream& in, OverlayComp* parent) : PipeComp(in, parent)
{
  init();
}
    
void DistantComp::init()
{
  _prepped = 0;
  _upflag = 0;
  _distant_request = 1;
  _cycle_ready = 0;
  #ifdef HAVE_ACE
  _addr = nil;
  _socket = nil;
  _conn = nil;
  #endif
  _disabled = -1;
  _distnamesym = 0;

  if(!_distant_list) {
    _distant_list = new AttributeValueList();
    ComValue* newval = new ComValue(_distant_list);
    int newsym = symbol_add("distant_list");
    ComTerpServ* comterp = ((OverlayUnidraw*)unidraw)->comterp();
    comterp->globaltable()->insert(newsym, newval);
  }
  AttributeValue* av = new AttributeValue(new OverlayViewRef(this), DistantComp::class_symid());
  _distant_list->Append(av);

}

DistantComp::~DistantComp() {
  #ifdef HAVE_ACE
  if (_socket && _socket->close () == -1)
    fprintf(stderr, "error closing socket in DistantComp destructor\n");
  delete _conn;
  delete _socket;
  delete _addr;
  #endif
  
  Iterator it;
  if(_distant_list) {
    _distant_list->First(it);
    while(!_distant_list->Done(it)) {
      AttributeValue* av = _distant_list->GetAttrVal(it);
      if (av->is_null()) continue;
      OverlayView* view = (OverlayView*) av->obj_val();
      if (view->GetOverlayComp()==this) {
	_distant_list->Remove(it);
	#ifndef RESOURCE_COMPVIEW
	delete view;  
        #endif
	delete av;
	break;
      }
      _distant_list->Next(it);
    }
  }
}
 

ClassId DistantComp::GetClassId () { return DISTANT_COMP; }

boolean DistantComp::IsA (ClassId id) {
    return DISTANT_COMP == id || PipeComp::IsA(id);
}

void DistantComp::_src_request() {

#ifdef HAVE_ACE
  if (!_prepped) {
    prep();
    _prepped = true;
  }

  if (!_upflag) {
    PipeComp::_src_request();
    return;
  }

  if (distant_request()) return;
  distant_request(true);

  std::strstreambuf sbuf;
  ostream outs(&sbuf);
  outs << "request(" << symbol_pntr(_distnamesym) << ")\n" << '\0';

  char* sbufstr = sbuf.str();
  if(write(_socket->get_handle(), sbufstr, strlen(sbuf.str()))<0)
    fprintf(stderr, "unexpected error (%s) writing to socket in DistantComp:  %s", strerror(errno), sbuf.str());
  cout << "sending:  " << sbufstr;
  delete sbufstr;

#else
  fprintf(stderr, "DistantComp needs to be built with ACE\n");
#endif
  

}

int DistantComp::_src_ready() {

  if (!_upflag)
    return PipeComp::_src_ready();
  else 
    return 1;
}

void DistantComp::_dst_notify() {

#ifdef HAVE_ACE
  if (!_prepped) {
    prep();
    _prepped = true;
  }

  if (_upflag) {
    PipeComp::_dst_notify();
    return;
  }

  if (!distant_request()) return;
  distant_request(false);

  AttributeValue* av;
  _dst_get(av);

  if (av) {

    std::strstreambuf sbuf;
    ostream outs(&sbuf);
    outs << "srcput(" << symbol_pntr(_distnamesym) << " " << *av << ")\n" << '\0';
    char* sbufstr = sbuf.str();
    delete av;
    
    if(_socket->get_handle()) {
      if(write(_socket->get_handle(), sbufstr, strlen(sbufstr))<0)
      fprintf(stderr, "unexpected error (%s) writing to socket in DistantComp:  %s\n", strerror(errno), sbufstr);
      cout << "sending:  " << sbufstr;
    }
    delete sbufstr;
  } else
    fprintf(stderr, "DistantComp::_dst_notify  unexpected NULL AttributeValue\n");

#else
  fprintf(stderr, "DistantComp needs to be built with ACE\n");
#endif
  
  return;
  
}
 
int DistantComp::_dst_ready() {
  if (_upflag)
    return PipeComp::_dst_ready();
  else 
    return distant_request();
}
 
boolean DistantComp::disabled() {
  if (_disabled==-1) {
    static int upflagsym = symbol_add("upflag");
    AttributeValue* upflagval = FindValue(upflagsym);
    _disabled = !upflagval;
  }
  return _disabled;
}

void DistantComp::prep() {
 #ifdef HAVE_ACE
  static int upflagsym = symbol_add("upflag");
  AttributeValue* upflagval = FindValue(upflagsym);
  _upflag = upflagval && upflagval->is_true();
  
  static int hostsym = symbol_add("host");
  static int portsym = symbol_add("port");
  static int namesym = symbol_add("name");
  AttributeValue* hostval = FindValue(hostsym);
  AttributeValue* portval = FindValue(portsym);
  AttributeValue* nameval = FindValue(namesym);
  if (hostval&&portval&&nameval) {
    _distnamesym = nameval->symbol_val();
    
    _addr = new ACE_INET_Addr(portval->int_val(), hostval->string_ptr());
    _socket = new ACE_SOCK_Stream;
    _conn = new ACE_SOCK_Connector;
    if (_conn->connect (*_socket, *_addr) == -1) {
      fprintf(stderr, "unable to connect to socket in DistantComp\n");
	  return;
    } else {
      _socket->enable(ACE_NONBLOCK);
      if(write(_socket->get_handle(), "mute\n", 5)<0)
	fprintf(stderr, "unexpected error (%s) writing mute command to socket in DistantComp\n", strerror(errno));
    }
    return;
  }
  
  fprintf(stderr, "expected failure in prepping DistantComp\n");
#else
  fprintf(stderr, "DistantComp needs to be built with ACE\n");
#endif
  

}

Component* DistantComp::Copy() {
    DistantComp* comp = (DistantComp*) NewNodeComp((SF_Ellipse*)GetEllipse()->Copy(), 
            (TextGraphic*)GetText()->Copy());
    if (GetText2())
      comp->SetText2((TextGraphic*)GetText2()->Copy());
    comp->GetGraphic()->SetTransformer(new Transformer(GetGraphic()->GetTransformer()));
    return comp;
}

void DistantComp::distant_request(int flag) {
  _distant_request = flag;
  if (flag && _dst_occupied())
    dst_notify();
}

int DistantComp::upflag() {
  if (_prepped) 
    return _upflag;
  else {
    static int upflagsym = symbol_add("upflag");
    AttributeValue* upflagval = FindValue(upflagsym);
    return _upflag = upflagval && upflagval->is_true();
  }
}

int DistantComp::cycle() {
#ifdef HAVE_ACE
  if (PipeComp::_eventqueueflag>=2) {
    unidraw->Update(true);

    ComTerpServ* comterp = ((OverlayUnidraw*)unidraw)->comterp();
    static UnidrawPauseFunc* _pausefunc = nil;
    if (!_pausefunc) {
      Iterator it;
      unidraw->First(it);
      _pausefunc = new UnidrawPauseFunc(comterp, unidraw->GetEditor(it));
    }

    _pausefunc->exec(0, 0);  
    comterp->pop_stack();
  }

  if (!_first_cycle) {
    _first_cycle = 1;
    if (PipeComp::_eventqueueflag==0) PipeComp::_eventqueueflag=1;
  }

  _cycle_count++;

#define SHOW_STATS
#if defined(SHOW_STATS)   // slight unexplained memory loss herein.
        // this should be done without paste/delete
  if (!DistantComp::nostats()) {
    ComTerpServ* comterp = ((OverlayUnidraw*)unidraw)->comterp();
    Iterator jt;
    unidraw->First(jt);
    
    if (_statval) {
      DeleteFunc delfunc(comterp, unidraw->GetEditor(jt));
      comterp->push_stack(*_statval);
      delfunc.exec(1,0);
      delete _statval;
      _statval = nil;
    }
    
#if 1
    CreateTextFunc func(comterp, unidraw->GetEditor(jt));
    AttributeValueList* al = new AttributeValueList();
    al->Append(new AttributeValue(0, AttributeValue::IntType));
    al->Append(new AttributeValue(0, AttributeValue::IntType));
    ComValue pointv(al);
    comterp->push_stack(pointv);
    char buffer[32];
#if 0
    snprintf(buffer, 32, "%d", _cycle_count);
#else
    if (!_eventqueue) _eventqueue = new PipeEventQueue();
    snprintf(buffer, 32, "%d (%d)", (int)ceil(PipeComp::_eventqueue->lasttick()/2.0), _cycle_count);
#endif
    ComValue textv(buffer);
    comterp->push_stack(textv);
    func.exec(2, 0);  
#else
    CreateRectFunc func(comterp, unidraw->GetEditor(jt));
    AttributeValueList* al = new AttributeValueList();
    al->Append(new AttributeValue(0, AttributeValue::IntType));
    al->Append(new AttributeValue(0, AttributeValue::IntType));
    al->Append(new AttributeValue(25, AttributeValue::IntType));
    al->Append(new AttributeValue(100, AttributeValue::IntType));
    ComValue pointsv(al);
    comterp->push_stack(pointsv);
    func.exec(1, 0);  
#endif
    _statval = new ComValue(comterp->pop_stack());

    Selection* sel = unidraw->GetEditor(jt)->GetViewer()->GetSelection();
    sel->Clear();
  }
#endif /* defined(SHOW_STATS) */
  

  /* loop through all DistantComp's, sending out a ready signal */
  Iterator it;
  for (_distant_list->First(it); !_distant_list->Done(it); _distant_list->Next(it)) {
    AttributeValue* av = _distant_list->GetAttrVal(it);
    if (av->is_null()) continue;
    DistantComp* comp = (DistantComp*)((OverlayView*)av->obj_val())->GetOverlayComp();

    if (!comp->_prepped) {
      comp->prep();
      comp->_prepped = true;
    }
    
    if(!comp->disabled()) {
      std::strstreambuf sbuf;
      ostream outs(&sbuf);
      outs << "ready(" << symbol_pntr(comp->_distnamesym) << ")\n" << '\0';
      char* sbufstr = sbuf.str();
      
      if(write(comp->_socket->get_handle(), sbufstr, strlen(sbufstr))<0)
	fprintf(stderr, "unexpected error (%s) writing to socket in DistantComp:  %s", strerror(errno), sbufstr);
      delete sbufstr;
    }
  }
  /* done looping through all DistantComp's, sending out a ready signal */

#if 0
  cout << ".";
#endif

#else
  fprintf(stderr, "DistantComp needs to be built with ACE\n");
#endif
  return 1;
}

int DistantComp::ready() {
  
#if 0
  /* initialize if necessary */
  if (!_first_cycle)
    cycle();
#endif

  static int hostsym = symbol_add("host");
  cycle_ready(1);

  Iterator it;
  for (_distant_list->First(it); !_distant_list->Done(it); _distant_list->Next(it)) {
    AttributeValue* av = _distant_list->GetAttrVal(it);
    if (av->is_null()) continue;
    DistantComp* comp = (DistantComp*)((OverlayView*)av->obj_val())->GetOverlayComp();
    if (comp->disabled()) continue;
    if (!comp->cycle_ready()) return 0;
  }

  /* reset cycle ready flag */
  for (_distant_list->First(it); !_distant_list->Done(it); _distant_list->Next(it)) {
    AttributeValue* av = _distant_list->GetAttrVal(it);
    if (av->is_null()) continue;
    DistantComp* comp = (DistantComp*)((OverlayView*)av->obj_val())->GetOverlayComp();
    comp->cycle_ready(0);
  }
  
  if(PipeComp::_eventqueue) PipeComp::_eventqueue->tick();
  return cycle();
}
