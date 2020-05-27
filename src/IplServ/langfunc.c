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
#include <IplServ/iplfunc.h>
#include <IplServ/langfunc.h>
#include <TopoFace/topoedge.h>
#include <Unidraw/catalog.h>
#include <Unidraw/iterator.h>
#include <Unidraw/ulist.h>
#include <Unidraw/unidraw.h>
#include <ComTerp/assignfunc.h>
#include <Attribute/attrlist.h>

/*****************************************************************************/

DestinationFunc::DestinationFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DestinationFunc::execute() {
  ComValue namev(stack_arg(0, true));
  reset_stack();

  void* pipeptr = nil;
  PipeComp* pipecomp = nil;
  comterp()->globaltable()->find(pipeptr, namev.symbol_val());
  if (!pipeptr) {
    pipecomp = new PipeComp();
    pipecomp->namesym(namev.symbol_val());
    ComValue* pipeval = new ComValue(new OverlayViewRef(pipecomp), PipeComp::class_symid());
    comterp()->globaltable()->insert(namev.symbol_val(), (void*)pipeval);

  } else {
    ComValue* pipeval = (ComValue*)pipeptr;
    pipecomp = (PipeComp*)pipeval->geta(PipeComp::class_symid());
#ifdef HAVE_ACE
    if (!pipecomp) pipecomp = (PipeComp*) pipeval->geta(DistantComp::class_symid());
#endif
    if (!pipecomp) {
      fprintf(stderr, "value of \"%s\" not pipe\n", symbol_pntr(namev.symbol_val()));
      push_stack(ComValue::nullval());
      return;
    }

  }

  ConnComp* conncomp = new ConnComp();
  conncomp->Edge()->attach_nodes(pipecomp->Node(), nil);
  pipecomp->dst_notify();
  ComValue connval(new OverlayViewRef(conncomp), ConnComp::class_symid());
  push_stack(connval);

#if 0  /* never drawn first, because it relies on start and end nodes to be drawn first */
  if (unidraw) {
    Iterator it;
    unidraw->First(it);
    ConnViewFunc func(comterp(), unidraw->GetEditor(it));
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
  }
#endif
}

/*****************************************************************************/

Destination2Func::Destination2Func(ComTerp* comterp) : ComFunc(comterp) {
}

void Destination2Func::execute() {
  ComValue name1v(stack_arg(0, true));
  ComValue name2v(stack_arg(1, true));
  reset_stack();

  char buffer[BUFSIZ];
  snprintf(buffer, BUFSIZ, "%s$%s", name1v.symbol_ptr(), name2v.symbol_ptr());
  ComValue retval(buffer);
  push_stack(retval);
  if (IplFunc::debug()) fprintf(stderr, "--> Destination2Func for %s\n", buffer);
}

/*****************************************************************************/

SymSpaceFunc::SymSpaceFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SymSpaceFunc::execute() {
  ComValue instrmv(stack_arg_post_eval(0));
  reset_stack();

  if (instrmv.is_stream()) {
    /* symbol space stream copy */
    AttributeValueList* in_avl = instrmv.stream_list();
    AttributeValueList* copy_avl = new AttributeValueList(in_avl);
    ComValue copystrmv(instrmv.stream_func(), copy_avl);
    copystrmv.stream_mode(instrmv.stream_mode());

    /* create new stream */
    static SymSpaceNextFunc* nextfunc = nil;
    if (!nextfunc) {
      nextfunc = new SymSpaceNextFunc(comterp());
      nextfunc->funcid(symbol_add("symspacenext"));
    }

    AttributeValueList* out_avl = new AttributeValueList();
    out_avl->Append(new ComValue(copystrmv));
    ComValue outstrmv(nextfunc, out_avl);
    outstrmv.stream_mode(1);
    push_stack(outstrmv);
  }

}

/*****************************************************************************/

SymSpaceNextFunc::SymSpaceNextFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SymSpaceNextFunc::execute() {
  ComValue namev(stack_arg(0, true));
  reset_stack();

  char buffer[BUFSIZ];
  if (namev.is_int()) 
    snprintf(buffer, BUFSIZ, "%d", namev.int_val());
  else
    snprintf(buffer, BUFSIZ, "%s", namev.symbol_ptr());
  ComValue retval(buffer);
  retval.type(ComValue::SymbolType);
  push_stack(retval);
  if (IplFunc::debug()) fprintf(stderr, "--> SymSpaceFunc for %s\n", buffer);
}

/*****************************************************************************/

SymSpace2Func::SymSpace2Func(ComTerp* comterp) : ComFunc(comterp) {
}

void SymSpace2Func::execute() {
  ComValue symv(stack_arg(0, true));
  if (symv.type() != ComValue::SymbolType) {
    symv.assignval(stack_arg_post_eval(0, true /* no symbol or attribute lookup */));
  }
  ComValue instrmv(stack_arg_post_eval(1));
  reset_stack();

  if (instrmv.is_stream()) {
    /* symbol space stream copy */
    AttributeValueList* in_avl = instrmv.stream_list();
    AttributeValueList* copy_avl = new AttributeValueList(in_avl);
    ComValue copystrmv(instrmv.stream_func(), copy_avl);
    copystrmv.stream_mode(instrmv.stream_mode());

    /* create new stream */
    static SymSpace2NextFunc* nextfunc = nil;
    if (!nextfunc) {
      nextfunc = new SymSpace2NextFunc(comterp());
      nextfunc->funcid(symbol_add("symspace2next"));
    }

    AttributeValueList* out_avl = new AttributeValueList();
    out_avl->Append(new ComValue(symv));
    out_avl->Append(new ComValue(copystrmv));
    ComValue outstrmv(nextfunc, out_avl);
    outstrmv.stream_mode(1);
    push_stack(outstrmv);
  }

}

/*****************************************************************************/

SymSpace2NextFunc::SymSpace2NextFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SymSpace2NextFunc::execute() {
  ComValue name1v(stack_arg(0, true));
  ComValue name2v(stack_arg(1, true));
  reset_stack();

  char buffer[BUFSIZ];
  if (name2v.is_int()) 
    snprintf(buffer, BUFSIZ, "%s%d", name1v.symbol_ptr(), name2v.int_val());
  else
    snprintf(buffer, BUFSIZ, "%s%s", name1v.symbol_ptr(), name2v.symbol_ptr());
  ComValue retval(buffer);
  retval.type(ComValue::SymbolType);
  push_stack(retval);
  if (IplFunc::debug()) fprintf(stderr, "--> SymSpace2Func for %s\n", buffer);
}

/*****************************************************************************/

DestSymSpaceFunc::DestSymSpaceFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DestSymSpaceFunc::execute() {
  ComValue instrmv(stack_arg_post_eval(0));
  reset_stack();

  if (instrmv.is_stream()) {
    /* symbol space stream copy */
    AttributeValueList* in_avl = instrmv.stream_list();
    AttributeValueList* copy_avl = new AttributeValueList(in_avl);
    ComValue copystrmv(instrmv.stream_func(), copy_avl);
    copystrmv.stream_mode(instrmv.stream_mode());

    /* create new stream */
    static DestSymSpaceNextFunc* nextfunc = nil;
    if (!nextfunc) {
      nextfunc = new DestSymSpaceNextFunc(comterp());
      nextfunc->funcid(symbol_add("destsymspacenext"));
    }

    AttributeValueList* out_avl = new AttributeValueList();
    out_avl->Append(new ComValue(copystrmv));
    ComValue outstrmv(nextfunc, out_avl);
    outstrmv.stream_mode(1);
    push_stack(outstrmv);
  }

}

/*****************************************************************************/

DestSymSpaceNextFunc::DestSymSpaceNextFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DestSymSpaceNextFunc::execute() {
  ComValue namev(stack_arg(0, true));
  reset_stack();

  char buffer[BUFSIZ];
  if (namev.is_int()) 
    snprintf(buffer, BUFSIZ, "%d", namev.int_val());
  else
    snprintf(buffer, BUFSIZ, "%s", namev.symbol_ptr());
  ComValue retval(buffer);
  retval.type(ComValue::SymbolType);
  push_stack(retval);
  if (IplFunc::debug()) fprintf(stderr, "--> DestSymSpaceFunc for %s\n", buffer);
  DestinationFunc destfunc(comterp());
  destfunc.exec(1, 0);
}

/*****************************************************************************/

DestSymSpace2Func::DestSymSpace2Func(ComTerp* comterp) : ComFunc(comterp) {
}

void DestSymSpace2Func::execute() {
  ComValue symv(stack_arg(0, true));
  if (symv.type() != ComValue::SymbolType) {
    symv.assignval(stack_arg_post_eval(0, true /* no symbol or attribute lookup */));
  }
  ComValue instrmv(stack_arg_post_eval(1));
  reset_stack();

  if (instrmv.is_stream()) {
    /* symbol space stream copy */
    AttributeValueList* in_avl = instrmv.stream_list();
    AttributeValueList* copy_avl = new AttributeValueList(in_avl);
    ComValue copystrmv(instrmv.stream_func(), copy_avl);
    copystrmv.stream_mode(instrmv.stream_mode());

    /* create new stream */
    static DestSymSpace2NextFunc* nextfunc = nil;
    if (!nextfunc) {
      nextfunc = new DestSymSpace2NextFunc(comterp());
      nextfunc->funcid(symbol_add("destsymspace2next"));
    }

    AttributeValueList* out_avl = new AttributeValueList();
    out_avl->Append(new ComValue(symv));
    out_avl->Append(new ComValue(copystrmv));
    ComValue outstrmv(nextfunc, out_avl);
    outstrmv.stream_mode(1);
    push_stack(outstrmv);
  }

}

/*****************************************************************************/

DestSymSpace2NextFunc::DestSymSpace2NextFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DestSymSpace2NextFunc::execute() {
  ComValue name1v(stack_arg(0, true));
  ComValue name2v(stack_arg(1, true));
  reset_stack();

  char buffer[BUFSIZ];
  if (name2v.is_int()) 
    snprintf(buffer, BUFSIZ, "%s%d", name1v.symbol_ptr(), name2v.int_val());
  else
    snprintf(buffer, BUFSIZ, "%s%s", name1v.symbol_ptr(), name2v.symbol_ptr());
  ComValue retval(buffer);
  retval.type(ComValue::SymbolType);
  push_stack(retval);
  if (IplFunc::debug()) fprintf(stderr, "--> DestSymSpace2Func for %s\n", buffer);
  Destination2Func destfunc(comterp());
  destfunc.exec(1, 0);
}

/*****************************************************************************/

SeparatorFunc::SeparatorFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SeparatorFunc::execute() {
  reset_stack();

  fprintf(stderr, "--> SeparatorFunc\n");
}

/*****************************************************************************/

InvoDefFunc::InvoDefFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void InvoDefFunc::execute() {
  ComValue namev(stack_arg(0, true));
  ComValue pathv(stack_arg(1));
  ComValue initv(stack_arg(2));
  reset_stack();
  Catalog* catalog = unidraw->GetCatalog();
  OverlayComp* comp;
  
  if (catalog->Retrieve(pathv.string_ptr(), (Component*&) comp)) {
    IplIdrawComp* idrawcomp = (IplIdrawComp*) (comp->IsA(IPL_IDRAW_COMP) ? comp : nil);
    if (idrawcomp) {
      ComValue* result = 
	new ComValue(new OverlayViewRef(comp), IplIdrawComp::class_symid());
      comterp()->localtable()->insert(namev.symbol_val(), result);
      if (initv.is_string()) 
	comterp()->runfile(initv.string_ptr());
      comterp()->pop_stack();
      push_stack(*result);
      return;
    }
  } 

  push_stack(ComValue::nullval());
  return;
}

/*****************************************************************************/

InvoDefFuncFunc::InvoDefFuncFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void InvoDefFuncFunc::execute() {
  ComValue src0v(stack_arg(0));
  ComValue src1v(stack_arg(1));
  ComValue src2v(stack_arg(2));
  reset_stack();

  ComValue srcvals[3] = {&src0v, &src1v, &src2v};
  
  IplIdrawComp* idrawcomp = defcomp();
  if (idrawcomp) {

    // handle sources
    int index=0;
    Iterator it;
    idrawcomp->First(it);
    FrameComp* framecomp = (FrameComp*)idrawcomp->GetComp(it);
    for (framecomp->First(it); !framecomp->Done(it); framecomp->Next(it)) {
      PipeComp* pipecomp = (PipeComp*) (framecomp->GetComp(it)->IsA(PIPE_COMP) ? framecomp->GetComp(it) : nil);
      if (pipecomp && pipecomp->nsrc()==0 && !pipecomp->ndst()==0) {
	pipecomp->src_put(new AttributeValue(srcvals[index]));
	pipecomp->dst_notify();
	if (++index>2) break;
      }
    }
    
    // handle destinations
    index=0;
    AttributeValue* dstvals[3] = {nil, nil, nil};
    for (framecomp->First(it); !framecomp->Done(it); framecomp->Next(it)) {
      PipeComp* pipecomp = (PipeComp*) (framecomp->GetComp(it)->IsA(PIPE_COMP) ? framecomp->GetComp(it) : nil);
      if (pipecomp && !pipecomp->nsrc()==0 && pipecomp->ndst()==0) {
	pipecomp->dst_get(dstvals[index]);
	if (++index>2) break;
      }
    }


    if (dstvals[0]) {
      if (!dstvals[1] && !dstvals[2]) {
	push_stack(*dstvals[0]);
	delete dstvals[0];
      } else {
	AttributeValueList* list = new AttributeValueList();
	list->Append(dstvals[0]);
	list->Append(dstvals[1]);
	if (dstvals[2]) list->Append(dstvals[2]);
	ComValue retval(list);
	push_stack(retval);
      } 
    } else 
      push_stack(ComValue::blankval());
  } 
  
  return;
}

/*****************************************************************************/

TranscribePipeFunc::TranscribePipeFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void TranscribePipeFunc::execute() {
  ComValue compv(stack_arg(0));
  static int go_symid = symbol_add("go");
  ComValue gov(stack_key(go_symid));
  int gof = gov.is_true();
  reset_stack();

  OverlayComp* comp = compv.obj_val() ? ((OverlayView*)compv.obj_val())->GetOverlayComp() : nil;
  PipeComp* pipecomp = comp && comp->IsA(PIPE_COMP) ? (PipeComp*)comp : nil;
  InvoComp* invocomp = comp && comp->IsA(INVO_COMP) ? (InvoComp*)comp : nil;
  if (pipecomp) {

    char buffer[BUFSIZ];
    int cnt=0;
    const char* name = symbol_pntr(pipecomp->namesym());
    if (invocomp || strlen(name)>0) 
      cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "%s", 
		    invocomp ? invocomp->funcname() : symbol_pntr(pipecomp->namesym()));
    else {
        char rstring[9];
	randchars(rstring, 8);
	cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "%s", rstring);
    }
	
    boolean mutex_flag = 0;
    cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, invocomp ? "(" : "<");
    mutex_flag = invocomp && invocomp->nsrcsizes()>1;
    if (mutex_flag) cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "{");
    
    int id=0;
    int order=-1;
    int startcnt = cnt;
    for(int j=0; j<(invocomp ? invocomp->nsrcsizes() : 1); j++) {
      for(int i=0; i<(invocomp ? invocomp->srcsizes()[j] : pipecomp->nsrc()); i++) {
	PipeComp* pcomp = pipecomp->remote_srccomp(id++, order);
	if (pcomp ) { 
	  
	  if(startcnt<cnt) 
	    cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, " ");
	  InvoComp* icomp = pcomp->IsA(INVO_COMP) ? (InvoComp*) pcomp : nil;
	  if (icomp)
	    cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "$%s_%d_%d", icomp->funcname(), 
			  icomp->funcnum(), order);
	  else {
	    const char* pname = symbol_pntr(pcomp->namesym());
	    if (strlen(pname)>0) 
	      cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "$%s", symbol_pntr(pcomp->namesym()));
	    else {
	      cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "$gen_%lx", (unsigned long) pcomp);
	    }
	  }
#if 0
	  // optional initial value
	  AttributeValue* initv = invocomp->FindValue(id-1 ? init1sym : init0sym);
	  if(initv) sublist->Append(new AttributeValue(initv));
#endif
	  
	}
      }
    }
    
    if (mutex_flag) cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "}");
    cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, invocomp ? ")" : ">");
    
    if (invocomp) {
      cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "(");
      mutex_flag = invocomp->ndstsizes()>1;
      if (mutex_flag) cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "{");
      
      id=0;
      startcnt = cnt;
      for(int j=0; j<invocomp->ndstsizes(); j++) {
	for(int i=0; i<invocomp->dstsizes()[j]; i++) {
	  if(startcnt<cnt) 
	    cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, " ");
	  cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "%s_%d_%d<>", invocomp->funcname(), 
			invocomp->funcnum(), id++);
	}
      }
      
      if (mutex_flag) cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, "}");
      cnt+=snprintf(buffer+cnt, BUFSIZ-cnt, ")");
    }
    
    ComValue retval(buffer);
    push_stack(retval); 
    
  }
  
  return;
}

void TranscribePipeFunc::randchars(char* randbuf, int size) {
  for (int i=0; i<size; i++)
    randbuf[i] = 0x61+(rand()%26);
}

