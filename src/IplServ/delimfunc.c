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

#include <IplServ/delimfunc.h>
#include <IplServ/iplclasses.h>
#include <IplServ/iplcomps.h>
#include <IplServ/iplfunc.h>
#include <OverlayUnidraw/ovunidraw.h>
#include <ComTerp/strmfunc.h>
#include <Unidraw/clipboard.h>
#include <Unidraw/Commands/edit.h>
#include <TopoFace/topoedge.h>
#include <TopoFace/toponode.h>

/*****************************************************************************/

ParensFunc::ParensFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void ParensFunc::execute() {
  static int parens_symid = symbol_add("()");
  static int parensplus_symid = symbol_add("+()");
  ComValue nameval(stack_arg(nargs()-1, true));
  boolean firstflag = !nameval.is_symbol() || nameval.symbol_val()!=parens_symid && nameval.symbol_val()!=parensplus_symid;
  boolean onstackflag = nameval.symbol_val()==parensplus_symid;
  ComValue invov;
  if (onstackflag) invov = stack_arg(0);
  int num=nargs()-1-onstackflag;
  ComValue *insidevals = new ComValue[num];
  for(int i=0; i<num; i++)   
    insidevals[i] = stack_arg(i+onstackflag);
  reset_stack();

  /* debug output */
  if (IplFunc::debug()) { 
    PipeComp* comp = nil;
    if (num>0) {
      fprintf(stderr, "--> ParensFunc for %s around ", nameval.symbol_ptr());
      for(int i=0; i<num; i++) {
	if(insidevals[i].object_compview()) {
	  
	  PipeComp* pipecomp=(PipeComp*)insidevals[i].geta(PipeComp::class_symid());
	  if (pipecomp)
	    fprintf(stderr, "%s<>", symbol_pntr(pipecomp->namesym()));
	  
	  ConnComp* conncomp=(ConnComp*)insidevals[i].geta(ConnComp::class_symid());
	  if (conncomp) {
	    PipeComp* dstcomp = (PipeComp*)conncomp->Edge()->start_node()->value();
	    fprintf(stderr, "$%s", symbol_pntr(dstcomp->namesym()));
	  }
	  
	  PipesComp* pipescomp=(PipesComp*)insidevals[i].geta(PipesComp::class_symid());
	  if (pipescomp)
	    fprintf(stderr, "{}");
	  
	  if(i+1<num) fprintf(stderr, ",");
	}
      }
      if (!firstflag) {
	InvoComp* invocomp= (InvoComp*)invov.geta(InvoComp::class_symid());
	if (invocomp) 
	  fprintf(stderr, " for invocation %s", invocomp->funcname() ? invocomp->funcname() : "unknown");
      }
      fprintf(stderr, "\n");
    }
    
    else
     fprintf(stderr, "--> ParensFunc for %s\n", nameval.symbol_ptr());
  }
    
  /* construct invocation if needed */
  if (firstflag) {
    InvoComp* invocomp = new InvoComp();
    
    ComValue funcv(lookup_symval(nameval));
    invocomp->func((ComFunc*)funcv.obj_val());
    if(!invocomp->func()) invocomp->namesym(nameval.symbol_val());
    invocomp->nsrc(num);
    invocomp->build_pipes();

    /* hook up to input pipes */
    if (num>0) {
      for(int i=0; i<num; i++) {
	if(insidevals[i].object_compview()) {
	  
	  ConnComp* conncomp=(ConnComp*)insidevals[i].geta(ConnComp::class_symid());
	  if (conncomp) {
	    PipeComp* subpipe = invocomp->find_subpipe(true, i);
	    push_stack(ComValue::nullval());  // don't need to supply other end
	    ComValue srcv(new OverlayViewRef(subpipe), PipeComp::class_symid());
	    push_stack(srcv);
	    push_stack(insidevals[i]);
	    ConnectDstToSrcFunc func(comterp());
	    func.exec(3,0);
	    comterp()->pop_stack();
	  } else {

	    /* replace srccomp pipe */
	    PipesComp* pipescomp=(PipesComp*)insidevals[i].geta(PipesComp::class_symid());
	    if (pipescomp) {
	      Iterator it;
	      PipesComp* srccomp = invocomp->invopipes()->srccomp();
	      PipesComp* invopipes = invocomp->invopipes();
	      invopipes->First(it);
	      invopipes->Remove(it);
	      invopipes->InsertBefore(it, pipescomp);
	      delete srccomp;
	      int nnsrcs = pipescomp->Count();
	      int nsrcs[nnsrcs];
	      for(int i=0; i<nnsrcs; i++) nsrcs[i] = 1;
	      invocomp->srcsizes(nsrcs, nnsrcs);
	    }
	  }
	}
      }
    }

    ComValue result(new OverlayViewRef(invocomp), InvoComp::class_symid());
    push_stack(result);

  }
  else {
    InvoComp* invocomp= (InvoComp*)invov.geta(InvoComp::class_symid());
    if (!invocomp) {
      delete [] insidevals;
      return;
    }

    /* if _ndstsizes is already set, this isn't a fresh InvoComp (because of stream overdriving), and needs to be copied */
    if (invocomp->ndstsizes()!=0) {
      invocomp = (InvoComp*)invocomp->CopyOnly();
      ComValue newinvov(new OverlayViewRef(invocomp), invov.class_symid());
      invov = newinvov;
    }
    
    invocomp->ndst(num);
    invocomp->build_pipes();

    push_stack(invov);

    /* replace output pipes */
    if (num>0) {
      for(int i=0; i<num; i++) {
	if(insidevals[i].object_compview()) {
	  
	  PipeComp* pipecomp=(PipeComp*)insidevals[i].geta(PipeComp::class_symid());
	  if (pipecomp) {
	    PipeComp* subpipe = invocomp->find_subpipe(false, i);
	    PipesComp* pipescomp = (PipesComp*)subpipe->GetParent();
	    Iterator it;
	    pipescomp->First(it);
	    while(!pipescomp->Done(it)) {
	      if (pipescomp->GetComp(it)==subpipe) {
		pipescomp->Remove(it);
		pipescomp->InsertBefore(it, pipecomp);
		delete subpipe;
		break;
	      }
	      pipescomp->Next(it);
	    }

	  } else {

	    /* replace dstcomp pipe */
	    PipesComp* pipescomp=(PipesComp*)insidevals[i].geta(PipesComp::class_symid());
	    if (pipescomp) {
	      Iterator it;
	      PipesComp* dstcomp = invocomp->invopipes()->dstcomp();
	      PipesComp* invopipes = invocomp->invopipes();
	      invopipes->First(it);
	      invopipes->Next(it);
	      invopipes->Remove(it);
	      invopipes->InsertBefore(it, pipescomp);
	      delete dstcomp;
	      int nndsts = pipescomp->Count();
	      int ndsts[nndsts];
	      for(int i=0; i<nndsts; i++) ndsts[i] = 1;
	      invocomp->dstsizes(ndsts, nndsts);
	    }
	  }
	}
      }
    }

    /* build invocation graphic and paste it */
    if (unidraw) {
      Iterator it;
      unidraw->First(it);
      Editor* editor = unidraw->GetEditor(it);
      invocomp->build_invocation_graphic();
      PasteCmd* cmd = new PasteCmd(editor, new Clipboard(invocomp));
      unidraw->ExecuteCmd(cmd);

      /* loop through all connectors, and paste those that are ready */
      for(int i=0; i<invocomp->nsrc(); i++) {
	PipeComp* subpipe = invocomp->find_subpipe(true, i);
	/* find all input connectors */
	Iterator it;
	subpipe->Node()->first(it);
	while (!subpipe->Node()->done(it)) {
	  
	  TopoEdge* edge = subpipe->Node()->get_edge(it);
	  if (edge->end_node() == subpipe->Node()) {
	    ConnComp* edgecomp = (ConnComp*) edge->value();
	    if (!edgecomp->GetGraphic() && edgecomp->NodeStart() && edgecomp->NodeStart()->GetGraphic()) {
	      edgecomp->build_conn_graphic();
	      PasteCmd* cmd = new PasteCmd(editor, new Clipboard(edgecomp));
	      unidraw->ExecuteCmd(cmd);
	    }
	  }
	  subpipe->Node()->next(it);
	}
      }
      for(int i=0; i<invocomp->ndst(); i++) {
	PipeComp* subpipe = invocomp->find_subpipe(false, i);
	
	/* find all output connectors */
	Iterator it;
	subpipe->Node()->first(it);
	while (!subpipe->Node()->done(it)) {
	  TopoEdge* edge = subpipe->Node()->get_edge(it);
	  if (edge->start_node() == subpipe->Node()) {
	    ConnComp* edgecomp = (ConnComp*) edge->value();
	    if (!edgecomp->GetGraphic() && edgecomp->NodeEnd() && edgecomp->NodeEnd()->GetGraphic()) {
	      edgecomp->build_conn_graphic();
	      PasteCmd* cmd = new PasteCmd(editor, new Clipboard(edgecomp));
	      unidraw->ExecuteCmd(cmd);
	    }
	  }
	  subpipe->Node()->next(it);
	}
      }
      /* done looping through all connectors, and pasting those that are ready */

    }
  }

  delete [] insidevals;
  return;
}

/*****************************************************************************/

BracketsFunc::BracketsFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void BracketsFunc::execute() {
  ComValue nameval(stack_arg(nargs()-1, true));
  reset_stack();
  fprintf(stderr, "--> BracketsFunc for %s\n", nameval.symbol_ptr());
  push_stack(nameval);
}

/*****************************************************************************/

BracesFunc::BracesFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void BracesFunc::execute() {
  int num=nargs()-1;
  ComValue*  insidevals = new ComValue[num];
  for(int i=0; i<num; i++)   
    insidevals[i] = stack_arg(i);
  ComValue nameval(stack_arg(num, true));
  reset_stack();

  PipesComp* pipescomp = new PipesComp();
  ComValue retval(new OverlayViewRef(pipescomp), PipesComp::class_symid());
  push_stack(retval);
  if (num==0) {
    delete insidevals;
    return;
  }

  boolean upstream_flag = insidevals[0].geta(ConnComp::class_symid()) != nil;

  if(IplFunc::debug()) fprintf(stderr, "--> BracesFunc for %s around ", nameval.symbol_ptr());

  if (upstream_flag) {
    for(int i=0; i<num; i++) {
      ConnComp* conncomp = (ConnComp*)insidevals[i].geta(ConnComp::class_symid());
      if (conncomp) {
	PipeComp* startcomp = (PipeComp*)conncomp->Edge()->start_node()->value();
	PipeComp* endcomp = new PipeComp();
	conncomp->Edge()->attach_nodes(startcomp->Node(), endcomp->Node());
	startcomp->dst_notify();
	PipesComp* subpipescomp = new PipesComp();
	subpipescomp->Append(endcomp);
	pipescomp->Append(subpipescomp);
	if(IplFunc::debug()) fprintf(stderr, "$%s ", symbol_pntr(startcomp->namesym()));
      }
    }

  } else {
    for(int i=0; i<num; i++) {
      PipeComp* pipecomp = (PipeComp*)insidevals[i].geta(PipeComp::class_symid());
      if (pipecomp) {
	PipesComp* subpipescomp = new PipesComp();
	subpipescomp->Append(pipecomp);
	pipescomp->Append(pipecomp);
	if(IplFunc::debug()) fprintf(stderr, "%s<> ", symbol_pntr(pipecomp->namesym()));
      }
    }
  }
  if(IplFunc::debug()) fprintf(stderr, "\n");

  delete [] insidevals;
  return;
  
}

/*****************************************************************************/

AngleBracketsFunc::AngleBracketsFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void AngleBracketsFunc::execute() {
  static int abrackets_symid = symbol_add("<>");
  static int abracketsplus_symid = symbol_add("+<>");
  ComValue namev(stack_arg(nargs()-1, true));
  boolean namefirst = !namev.is_symbol() || namev.symbol_val()==abrackets_symid || namev.symbol_val()==abracketsplus_symid;
  if (namefirst)  namev = stack_arg(0, true);
  ComValue connv(nargs()>namefirst ? stack_arg(namefirst, true) : ComValue::nullval());
  reset_stack();

  if (namev.symbol_val()==abrackets_symid) {
    fprintf(stderr, "unexpected missing angle bracket name\n");
    return;
  }

  if(IplFunc::debug()) {
    fprintf(stderr, "--> AngleBracketsFunc for %s", namev.symbol_ptr());
    if(connv.is_known()) {
      ConnComp* conncomp=(ConnComp*)connv.geta(ConnComp::class_symid());
      if (conncomp) {
	PipeComp* dstcomp = (PipeComp*)conncomp->Edge()->start_node()->value();
	fprintf(stderr, " around $%s", symbol_pntr(dstcomp->namesym()));
      }
    }
    fprintf(stderr, "\n");
  }
  

#if 0
  // handle stream case
  if (noname_flag && comterp()->stack_height()>0 && comterp()->stack_top().is_stream()) {
    ComValue streamv(comterp()->stack_top());
    do {
      NextFunc::execute_impl(comterp(), streamv);
      if (comterp()->stack_top().is_known()) {
	cerr << comterp()->stack_top().string_ptr() << "<>\n"; cerr.flush(); 
	comterp()->pop_stack();
      }
    } while (comterp()->stack_top().is_known());
    return;
  }
#endif

  void* srcpipeptr = nil;
  ComValue* srcpipeval = nil;
  comterp()->globaltable()->find(srcpipeptr, namev.symbol_val());
  PipeComp* srccomp = nil;
  if (!srcpipeptr) {
    srccomp = new PipeComp();
    srccomp->namesym(namev.symbol_val());
    srcpipeval = new ComValue(new OverlayViewRef(srccomp), PipeComp::class_symid());
    comterp()->globaltable()->insert(namev.symbol_val(), (void*)srcpipeval);
  } else {
    srcpipeval = (ComValue*)srcpipeptr;
    srccomp = (PipeComp*)srcpipeval->geta(PipeComp::class_symid());
    srccomp->namesym(namev.symbol_val());
#ifdef HAVE_ACE
    if (!srccomp) srccomp = (PipeComp*) srcpipeval->geta(DistantComp::class_symid());
#endif
    if (!srccomp) { 
      fprintf(stderr, "value \"%s\" already exists\n", symbol_pntr(namev.symbol_val()));
      push_stack(ComValue::nullval());
      return;
    }
  }

  if (connv.is_object()) {
    ConnComp* conncomp = (ConnComp*)connv.geta(ConnComp::class_symid());
    if (!conncomp) {
      fprintf(stderr, "no connector found for destination in source\n");
      push_stack(ComValue::nullval());
      return;
    }
    push_stack(ComValue::nullval());  // don't need to supply other end
    push_stack(*srcpipeval);
    push_stack(connv);
    ConnectDstToSrcFunc func(comterp());
    func.exec(3,0);
    comterp()->pop_stack();
  }

  push_stack(*srcpipeval);
  
  if (unidraw) {
 
    Iterator it;
    unidraw->First(it);
    Editor* editor = unidraw->GetEditor(it);
 
    if (connv.is_object() || comterp()->top_expr()) {

      if (!srccomp->GetGraphic()) {
	srccomp->build_pipe_graphic();
	PasteCmd* cmd = new PasteCmd(editor, new Clipboard(srccomp));
	unidraw->ExecuteCmd(cmd);
      }
      
      /* draw an incoming edge if it is ready */
      ConnComp* conncomp = (ConnComp*)connv.geta(ConnComp::class_symid());
      if (conncomp && conncomp->NodeStart() && conncomp->NodeStart()->GetGraphic()) {
	conncomp->build_conn_graphic();
	PasteCmd* cmd = new PasteCmd(editor, new Clipboard(conncomp));
	unidraw->ExecuteCmd(cmd);
      }

      /* loop through all output connectors draw if ready */
      srccomp->Node()->first(it);
      while (!srccomp->Node()->done(it)) {
	TopoEdge* edge = srccomp->Node()->get_edge(it);
	if (edge->start_node() == srccomp->Node()) {
	  ConnComp* conncomp = (ConnComp*) edge->value();
	  if (conncomp->NodeEnd() && conncomp->NodeEnd()->GetGraphic()
	      && !conncomp->GetGraphic()) {
	    conncomp->build_conn_graphic();
	    PasteCmd* cmd = new PasteCmd(editor, new Clipboard(conncomp));
	    unidraw->ExecuteCmd(cmd);
	  }
	}
	srccomp->Node()->next(it);
      }
      /* done looping through all output connectors to draw if ready */
    }
    
    
  }
  
}

/*****************************************************************************/

DoubleAngleBracketsFunc::DoubleAngleBracketsFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DoubleAngleBracketsFunc::execute() {
  reset_stack();
  printf("-->DoubleAngleBracketsFunc\n");
}

