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

#include <Attribute/aliterator.h>
#include <Attribute/attribute.h>
#include <Attribute/attrlist.h>
#if defined(ARCH_READY)
#include <IplServ/archcomps.h>
#endif
#include <IplServ/eventqueue.h>
#include <IplServ/iplfunc.h>
#include <IplServ/iplhandler.h>
#include <IplServ/iplclasses.h>
#include <IplServ/iplcomps.h>
#include <IplServ/spritequeue.h>

#include <Unidraw/Graphic/ellipses.h>
#include <Unidraw/Graphic/picture.h>
#include <Unidraw/clipboard.h>
#include <Unidraw/Commands/edit.h>
#include <ComTerp/comterpserv.h>
#include <ComTerp/comvalue.h>
#include <TopoFace/topoedge.h>
#include <TopoFace/toponode.h>

#ifdef HAVE_ACE
#include <ace/SOCK_Connector.h>
#endif

#include <ComUtil/comutil.h>

extern int _angle_brackets;
extern int _detail_matched_delims;
extern int _sticky_matched_delims;
extern int _ignore_numerics;

static int fifo_symid = -1;

/*****************************************************************************/

int IplFunc::_debug = 0;

IplFunc::IplFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void IplFunc::execute() {
    static int echo_symid = symbol_add("echo");
    boolean echo_flag = stack_key(echo_symid).is_true();
    static int debug_symid = symbol_add("debug");
    boolean debug_flag = stack_key(debug_symid).is_true();
    reset_stack();
    comterp()->echo_postfix(echo_flag);
    if (!comterp()->delim_func()) {
      enable(debug_flag);
      ComValue result(1);
      push_stack(result);    
    } else {
      disable();
      ComValue result(0);
      push_stack(result);    
    }

}

void IplFunc::enable(int debug) {
    static void* opr_tbl_ptr_save = nil;
    static int opr_tbl_numop_save;
    static int opr_tbl_maxop_save;
    static int opr_tbl_maxpri_save;
    static int opr_tbl_lastop_save;
    _debug = debug;
    _angle_brackets = 1;
    _detail_matched_delims = 1;
    _sticky_matched_delims = 1;
    comterp()->delim_func(1);
    comterp()->autostream(1);
    // _ignore_numerics = 1;
    if (!opr_tbl_ptr_save) {
      opr_tbl_ptr_set(nil);
      opr_tbl_create(7);   /* comma, dollar, and colon */
      opr_tbl_insert("$$$", "destsymspace", 1050, TRUE, OPTYPE_UNARY_PREFIX);      
      opr_tbl_insert("$$$", "destsymspace2", 1050, TRUE, OPTYPE_BINARY);      
      opr_tbl_insert("$$", "symspace", 1050, TRUE, OPTYPE_UNARY_PREFIX);      
      opr_tbl_insert("$$", "symspace2", 1050, TRUE, OPTYPE_BINARY);      
      opr_tbl_insert("$", "destination", 1050, TRUE, OPTYPE_UNARY_PREFIX);      
      opr_tbl_insert("$", "destination2", 1050, TRUE, OPTYPE_BINARY);      
      opr_tbl_insert(",", "separator", 20, FALSE, OPTYPE_BINARY);
      opr_tbl_ptr_save = opr_tbl_ptr_get();
      opr_tbl_numop_save = opr_tbl_numop_get();
      opr_tbl_maxop_save = opr_tbl_maxop_get();
      opr_tbl_maxpri_save = opr_tbl_maxpri_get();
      opr_tbl_lastop_save = opr_tbl_lastop_get();
   } else {
      opr_tbl_ptr_set(opr_tbl_ptr_save);
      opr_tbl_numop_set(opr_tbl_numop_save);
      opr_tbl_maxop_set(opr_tbl_maxop_save);
      opr_tbl_maxpri_set(opr_tbl_maxpri_save);
      opr_tbl_lastop_set(opr_tbl_lastop_save);
    }
}

void IplFunc::disable() {
    _angle_brackets = 0;
    _detail_matched_delims = 0;
    _sticky_matched_delims = 0;
    comterp()->autostream(0);
    comterp()->delim_func(0);
    // _ignore_numerics = 0;
    opr_tbl_ptr_set(nil);
    opr_tbl_default();
}

/*****************************************************************************/

AngleBracketsFlagFunc::AngleBracketsFlagFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void AngleBracketsFlagFunc::execute() {
    if (nargs()==0) {
        reset_stack();
        ComValue result(_angle_brackets);
        push_stack(result);
    } else {
      ComValue retval(stack_arg(0));
      reset_stack();
      _angle_brackets = retval.int_val();
      push_stack(retval);
    }
}

/*****************************************************************************/

CreatePipeFunc::CreatePipeFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void CreatePipeFunc::execute() {
    ComValue buffsizev(stack_arg(0));
    ComValue namev(stack_arg(1, true));
    boolean buffsizeflag = nargs()>=0 && buffsizev.is_int();
    reset_stack();
    PipeComp* comp = new PipeComp();
    if (namev.is_symbol()) comp->namesym(namev.symbol_val());
    if (buffsizeflag) comp->buffsize(buffsizev.int_val());			    
    ComValue result(new OverlayViewRef(comp), PipeComp::class_symid());
    push_stack(result);
#if 0
    IplViewerFunc func(comterp());
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
#endif
}

/*****************************************************************************/

CreatePipesFunc::CreatePipesFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void CreatePipesFunc::execute() {
    ComValue buffsizev(stack_arg(0));
    boolean buffsizeflag = nargs()>=0 && buffsizev.is_int();
    reset_stack();
    PipesComp* comp = new PipesComp();
    ComValue result(new OverlayViewRef(comp), PipesComp::class_symid());
    push_stack(result);
#if 0
    IplViewerFunc func(comterp());
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
#endif
}

/*****************************************************************************/

CreateInvoPipesFunc::CreateInvoPipesFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void CreateInvoPipesFunc::execute() {
  ComValue srcpipesv(stack_arg(0)); 
  ComValue dstpipesv(stack_arg(1)); 
  reset_stack();
  InvoPipesComp* comp = new InvoPipesComp();

  PipesComp* srcpipes = (PipesComp*) srcpipesv.geta(PipesComp::class_symid());
  PipesComp* dstpipes = (PipesComp*) dstpipesv.geta(PipesComp::class_symid());
  if (srcpipes) {
    comp->Append(srcpipes);
  } else {
    comp->Append(new PipesComp());
  }
  if (dstpipes) {
    comp->Append(dstpipes);
  } else {
    comp->Append(new PipesComp());
  }

  ComValue result(new OverlayViewRef(comp), InvoPipesComp::class_symid());
  push_stack(result);
#if 0
  IplViewerFunc func(comterp());
  func.push_funcstate(1,0);
  func.execute();
  func.pop_funcstate();
#endif
}

/*****************************************************************************/

CreateInvoFunc::CreateInvoFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void CreateInvoFunc::execute() {
  ComValue funcv(stack_arg(0)); 
  ComValue invopipesv(stack_arg_post_eval(1)); 
  reset_stack();
  if (!funcv.is_command()) return;
  InvoComp* comp = new InvoComp();
  ComValue funcv2(lookup_symval(funcv));
  comp->func((ComFunc*)funcv2.obj_val());

  InvoPipesComp* invopipes = (InvoPipesComp*) invopipesv.geta(InvoPipesComp::class_symid());
  if (invopipes) {
    comp->SetGraph(invopipes);
    comp->nsrc(invopipes->srccomp()->Count());
    comp->ndst(invopipes->dstcomp()->Count());
    invopipes->invocomp(comp);
  }
  else 
    return;

  ComValue result(new OverlayViewRef(comp), InvoComp::class_symid());
  push_stack(result);
#if 0
  IplViewerFunc func(comterp());
  func.push_funcstate(1,0);
  func.execute();
  func.pop_funcstate();
#endif
}

/*****************************************************************************/

CreateArbiterFunc::CreateArbiterFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void CreateArbiterFunc::execute() {
    reset_stack();
    ArbiterComp* comp = new ArbiterComp();
    ComValue result(new OverlayViewRef(comp), ArbiterComp::class_symid());
    push_stack(result);
#if 0
    IplViewerFunc func(comterp());
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
#endif
}

/*****************************************************************************/

ConnectDstToSrcFunc::ConnectDstToSrcFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void ConnectDstToSrcFunc::execute() {
    ComValue dstv(stack_arg(0));
    ComValue srcv(stack_arg(1));
    ComValue connv(stack_arg(2));
    reset_stack();

    PipeComp* dstcomp = (PipeComp*) dstv.geta(PipeComp::class_symid());
    PipeComp* srccomp = (PipeComp*) srcv.geta(PipeComp::class_symid());
    if (!dstcomp) dstcomp = (PipeComp*) dstv.geta(ArbiterComp::class_symid());
    if (!srccomp) srccomp = (PipeComp*) srcv.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!dstcomp) dstcomp = (PipeComp*) dstv.geta(DistantComp::class_symid());
    if (!srccomp) srccomp = (PipeComp*) srcv.geta(DistantComp::class_symid());
#endif
    if (!srccomp) {
      push_stack(ComValue::nullval());
      return;
    }

    ConnComp* conncomp = connv.is_unknown() ? new ConnComp : (ConnComp*) connv.geta(ConnComp::class_symid());
    if (!conncomp) {
      push_stack(ComValue::nullval());
      return;
    }
    if (!dstcomp) {
      dstcomp = (PipeComp*)conncomp->Edge()->start_node()->value();
      if (!dstcomp) {
	push_stack(ComValue::nullval());
	return;
      }
    }

    conncomp->Edge()->attach_nodes(dstcomp->Node(), srccomp->Node());
    dstcomp->dst_notify();
    ComValue result(new OverlayViewRef(conncomp), ConnComp::class_symid());
    push_stack(result);
#if 0
    IplViewerFunc func(comterp());
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
#endif
}

/*****************************************************************************/

ConnectArbiterFunc::ConnectArbiterFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void ConnectArbiterFunc::execute() {
    ComValue dst1v(stack_arg(0));
    ComValue dst2v(stack_arg(1));
    ComValue srcv(stack_arg(2));
    ComValue arbiterv(stack_arg(3));
    reset_stack();

    PipeComp* dst1comp = (PipeComp*) dst1v.geta(PipeComp::class_symid());
    PipeComp* dst2comp = (PipeComp*) dst2v.geta(PipeComp::class_symid());
    PipeComp* srccomp = (PipeComp*) srcv.geta(PipeComp::class_symid());
    if (!dst1comp) dst1comp = (PipeComp*) dst1v.geta(ArbiterComp::class_symid());
    if (!dst2comp) dst2comp = (PipeComp*) dst2v.geta(ArbiterComp::class_symid());
    if (!srccomp) srccomp = (PipeComp*) srcv.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!dst1comp) dst1comp = (PipeComp*) dst1v.geta(DistantComp::class_symid());
    if (!dst2comp) dst2comp = (PipeComp*) dst2v.geta(DistantComp::class_symid());
    if (!srccomp) srccomp = (PipeComp*) srcv.geta(DistantComp::class_symid());
#endif

    ArbiterComp* arbitercomp = arbiterv.is_known() ? 
      (ArbiterComp*) arbiterv.geta(ArbiterComp::class_symid()) :
      new ArbiterComp();

    if (!dst1comp || !dst2comp || !srccomp || !arbitercomp) {
      push_stack(ComValue::nullval());
      return;
    }

    EdgeComp* dst1edgecomp = new EdgeComp();
    EdgeComp* dst2edgecomp = new EdgeComp();
    EdgeComp* srcedgecomp = new EdgeComp();
    dst1edgecomp->Edge()->attach_nodes(dst1comp->Node(), arbitercomp->Node());
    dst2edgecomp->Edge()->attach_nodes(dst2comp->Node(), arbitercomp->Node());
    srcedgecomp->Edge()->attach_nodes(arbitercomp->Node(),srccomp->Node());
    dst1comp->dst_notify();
    dst2comp->dst_notify();
    ComValue result(new OverlayViewRef(arbitercomp), ArbiterComp::class_symid());
    push_stack(result);
#if 0
    IplViewerFunc func(comterp());
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
#endif
}

/*****************************************************************************/

BuffSizeFunc::BuffSizeFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void BuffSizeFunc::execute() {
    ComValue pipev(stack_arg(0));
    ComValue sizev(stack_arg(1));
    boolean putflag = nargs()>1 && sizev.is_int();
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    if (putflag) {
      comp->buffsize(sizev.int_val());
      ComValue retval(sizev.int_val(), AttributeValue::IntType);
      push_stack(retval);
    } else {
      ComValue retval(comp->buffsize(), AttributeValue::IntType);
      push_stack(retval);
    }
    
}

/*****************************************************************************/

BuffFunc::BuffFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void BuffFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->buff());
    push_stack(retval);
    
}

/*****************************************************************************/

PutRateFunc::PutRateFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void PutRateFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->put_rate());
    push_stack(retval);
    
}

/*****************************************************************************/

NumSrcFunc::NumSrcFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void NumSrcFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) comp = (PipeComp*) pipev.geta(InvoComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->nsrc(), AttributeValue::IntType);
    push_stack(retval);
    
}

/*****************************************************************************/

NumDstFunc::NumDstFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void NumDstFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) comp = (PipeComp*) pipev.geta(InvoComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->ndst(), AttributeValue::IntType);
    push_stack(retval);
    
}

/*****************************************************************************/

SrcVacantFunc::SrcVacantFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SrcVacantFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->src_vacant(), AttributeValue::IntType);
    push_stack(retval);
    
}

/*****************************************************************************/

DstOccupiedFunc::DstOccupiedFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DstOccupiedFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->dst_occupied(), AttributeValue::IntType);
    push_stack(retval);
    
}

/*****************************************************************************/

SrcPutFunc::SrcPutFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SrcPutFunc::execute() {
    ComValue srcv(stack_arg(0));
    ComValue* valvp = new ComValue(stack_arg(1));
    reset_stack();

    PipeComp* comp = (PipeComp*) srcv.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) srcv.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) srcv.geta(ForkComp::class_symid());
    boolean distantflag = false;
#ifdef HAVE_ACE
    if (!comp) {
      comp = (PipeComp*) srcv.geta(DistantComp::class_symid());
      if (comp) distantflag = ((DistantComp*)comp)->upflag();
    }
#endif
    if (!comp) {
      delete valvp;
      push_stack(ComValue::nullval());
      return;
    }

    if (comp->src_put(valvp)) {
      if (distantflag) { 
	if (!((DistantComp*)comp)->distant_request())
	  fprintf(stderr, "unexpected downstream distant comp with request flag false\n");
	((DistantComp*)comp)->distant_request(false);
      }
      comp->dst_notify();
      ComValue retval(ComValue::trueval());
      push_stack(retval);
    } else {
      delete valvp;
      ComValue retval(ComValue::falseval());
      push_stack(retval);
    }
}

/*****************************************************************************/

DstGetFunc::DstGetFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DstGetFunc::execute() {
    ComValue dstv(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) dstv.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) dstv.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) dstv.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) dstv.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    AttributeValue* av = nil;
    if (comp->dst_get(av) && av) {
      push_stack(*av);
      delete av;
    } else
      push_stack(ComValue::blankval());

}

/*****************************************************************************/

#ifdef HAVE_ACE
RequestFunc::RequestFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void RequestFunc::execute() {
    ComValue dstv(stack_arg(0));
    reset_stack();

    DistantComp* comp = (DistantComp*) dstv.geta(DistantComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    comp->distant_request(true);
    push_stack(ComValue::blankval());
}
#endif

/*****************************************************************************/

PipesAddFunc::PipesAddFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void PipesAddFunc::execute() {
    ComValue pipesv(stack_arg(0));
    ComValue pipev(stack_arg(1));
    reset_stack();

    PipesComp* pipescomp = (PipesComp*) pipesv.geta(PipesComp::class_symid());
    PipeComp* pipecomp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!pipecomp) pipecomp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!pipecomp) pipecomp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!pipescomp || !pipecomp) {
      push_stack(ComValue::nullval());
      return;
    }

    pipescomp->Append(pipecomp);
    pipecomp->SetParent(pipecomp, pipescomp);
    ComValue result(new OverlayViewRef(pipescomp), PipesComp::class_symid());
    push_stack(result);
#if 0
    IplViewerFunc func(comterp());
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
#endif
}

/*****************************************************************************/

PipeText2Func::PipeText2Func(ComTerp* comterp) : ComFunc(comterp) {
}

void PipeText2Func::execute() {
    ComValue pipev(stack_arg(0));
    ComValue text2v(stack_arg(1));
    reset_stack();

    PipeComp* pipecomp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!pipecomp) pipecomp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!pipecomp) pipecomp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!pipecomp) {
      push_stack(ComValue::nullval());
      return;
    }

    if (text2v.is_string())
      pipecomp->ChangeText2(text2v.symbol_ptr());
    else {
      TextGraphic* tg = pipecomp->GetText2();
      if (tg) {
	ComValue retval(tg->GetOriginal());
	push_stack(retval);
      } else
	push_stack(ComValue::nullval());
    }
}

/*****************************************************************************/

BuildInvoFunc::BuildInvoFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void BuildInvoFunc::execute() {
    ComValue funcnamev(stack_arg(0));
    ComValue nsrcv(stack_arg(1));
    ComValue ndstv(stack_arg(2));
    reset_stack();

    InvoComp* invocomp = new InvoComp();

    invocomp->funcname(funcnamev.string_ptr());
    invocomp->nsrc(nsrcv.int_val());
    invocomp->ndst(ndstv.int_val());
    invocomp->build_invocation();

    ComValue result(new OverlayViewRef(invocomp), InvoComp::class_symid());
    push_stack(result);
#if 0
    IplViewerFunc func(comterp());
    func.push_funcstate(1,0);
    func.execute();
    func.pop_funcstate();
#endif
}

/*****************************************************************************/

GetInvoPipesFunc::GetInvoPipesFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void GetInvoPipesFunc::execute() {
    ComValue invov(stack_arg(0));
    reset_stack();

    InvoComp* invocomp = (InvoComp*) invov.geta(InvoComp::class_symid());
    if (!invocomp) {
      push_stack(ComValue::nullval());
      return;
    }

    InvoPipesComp* invopipescomp = invocomp->invopipes();
    ComValue result(new OverlayViewRef(invopipescomp), InvoPipesComp::class_symid());
    push_stack(result);
}

/*****************************************************************************/

GetSrcPipesFunc::GetSrcPipesFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void GetSrcPipesFunc::execute() {
    ComValue invopipesv(stack_arg(0));
    reset_stack();

    InvoPipesComp* invopipescomp = (InvoPipesComp*) invopipesv.geta(InvoPipesComp::class_symid());
    if (!invopipescomp) {
      InvoComp* invocomp = (InvoComp*)invopipesv.geta(InvoComp::class_symid());
      invopipescomp = invocomp ? invocomp->invopipes() : nil;
      if (!invopipescomp) {
	push_stack(ComValue::nullval());
	return;
      }
    }

    PipesComp* srccomp = invopipescomp->srccomp();
    ComValue result(new OverlayViewRef(srccomp), PipesComp::class_symid());
    push_stack(result);
}

/*****************************************************************************/

GetDstPipesFunc::GetDstPipesFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void GetDstPipesFunc::execute() {
    ComValue invopipesv(stack_arg(0));
    reset_stack();

    InvoPipesComp* invopipescomp = (InvoPipesComp*) invopipesv.geta(InvoPipesComp::class_symid());
    if (!invopipescomp) {
      InvoComp* invocomp = (InvoComp*)invopipesv.geta(InvoComp::class_symid());
      invopipescomp = invocomp ? invocomp->invopipes() : nil;
      if (!invopipescomp) {
	push_stack(ComValue::nullval());
	return;
      }
    }

    PipesComp* dstcomp = invopipescomp->dstcomp();
    ComValue result(new OverlayViewRef(dstcomp), PipesComp::class_symid());
    push_stack(result);
}

/*****************************************************************************/

PipesCompleteFunc::PipesCompleteFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void PipesCompleteFunc::execute() {
    ComValue pipesv(stack_arg(0));
    reset_stack();

    PipesComp* pipescomp = (PipesComp*) pipesv.geta(PipesComp::class_symid());
    if (!pipescomp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue result(pipescomp->complete(), ComValue::BooleanType);
    push_stack(result);
}

/*****************************************************************************/

#if defined(MANUAL_BREAKPOINT)
DebugCompFunc::DebugCompFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DebugCompFunc::execute() {
    ComValue invocompv(stack_arg(0));
    ComValue funcidv(stack_arg(1, true));
    reset_stack();

    InvoComp* invocomp = (InvoComp*) invocompv.geta(InvoComp::class_symid());
    if (!invocomp) {
#if defined(ARCH_READY)
      invocomp = (InvoComp*) invocompv.geta(AluComp::class_symid());
      if (!invocomp) {
#endif
	push_stack(ComValue::zeroval());
	return;
#if defined(ARCH_READY)
      }
#endif
    }

    _debug0_comp = invocomp;
    _debug0_func = funcidv.command_symid();

    push_stack(ComValue::oneval());
}
#endif

/*****************************************************************************/

TickFunc::TickFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void TickFunc::execute() {
    reset_stack();

#if 0
    ComValue retval(comterpserv()->run("nevent=eventqueue(:size);for(_tickvar=0 _tickvar<nevent _tickvar++ nextevent);nevent", true));
    push_stack(retval);
#else
    if (PipeComp::_eventqueue) {
      ComValue retval(PipeComp::_eventqueue->tick(/*true*/)); 
      push_stack(retval);
    } else {
      fprintf(stderr, "eventqueue not initialized\n");
      push_stack(ComValue::nullval());
    }
#endif

}

/*****************************************************************************/

CycleFunc::CycleFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void CycleFunc::execute() {
    static int nostats_symid = symbol_add("nostats");
    ComValue nostatsv(stack_key(nostats_symid));
    reset_stack();

    DistantComp::nostats(nostatsv.is_true());
    ComValue retval(DistantComp::cycle());
    push_stack(retval);
}

/*****************************************************************************/

CycleCountFunc::CycleCountFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void CycleCountFunc::execute() {
    reset_stack();

    ComValue retval(DistantComp::cycle_count());
    push_stack(retval);
}

/*****************************************************************************/

#ifdef HAVE_ACE
ReadyFunc::ReadyFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void ReadyFunc::execute() {
    ComValue dstv(stack_arg(0));
    reset_stack();

    DistantComp* comp = (DistantComp*) dstv.geta(DistantComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    comp->ready();
    push_stack(ComValue::blankval());
}
#endif

/*****************************************************************************/

LastInputFunc::LastInputFunc(ComTerp* comterp) : ComFunc(comterp) {
}

#ifdef ARBITER_DEBUG
extern ArbiterComp* _arbiter_debug; 
#endif

void LastInputFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    ArbiterComp* comp = (ArbiterComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

#ifdef ARBITER_DEBUG
    _arbiter_debug = comp;
#endif

    ComValue retval(comp->lastinput(), AttributeValue::IntType);
    push_stack(retval);
    
}

/*****************************************************************************/

PipeNameFunc::PipeNameFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void PipeNameFunc::execute() {
    ComValue pipev(stack_arg(0));
    static int set_symid = symbol_add("set");
    ComValue setv(stack_key(set_symid));
    reset_stack();

    boolean invoflag = false;
    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      comp = (PipeComp*) pipev.geta(InvoComp::class_symid());
      invoflag = true;
    }
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    if(setv.is_string() || setv.is_symbol()) {
      comp->namesym(setv.symbol_val());
      comp->SetText(new TextGraphic(symbol_pntr(setv.symbol_val()), comp->GetText()));
      comp->Notify();
    }
    
    const char* str = symbol_pntr(comp->namesym());
    if (!str && invoflag) 
      str = ((InvoComp*)comp)->funcname();
    ComValue retval(str ? str : ComValue::nullval());
    push_stack(retval);
    
}

/*****************************************************************************/

NeighborsFunc::NeighborsFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void NeighborsFunc::execute() {
    ComValue pipev(stack_arg(0));
    static int src_symv = symbol_add("src");
    static int dst_symv = symbol_add("dst");
    static int pipe_symv = symbol_add("pipe");
    ComValue srcv(stack_key(src_symv, false, ComValue::oneval()));
    ComValue dstv(stack_key(dst_symv, false, ComValue::oneval()));
    ComValue pipeflagv(stack_key(pipe_symv, false, ComValue::oneval()));
    reset_stack();

    boolean pipeflag = pipeflagv.is_known();

    boolean srcflag, dstflag;
    if(srcv.is_unknown()&&dstv.is_known()) {
      srcflag = !dstflag;
      dstflag = dstv.int_val();
    }
    else if(srcv.is_known()&&dstv.is_unknown()) {
      srcflag = srcv.int_val();
      dstflag = !srcflag;
    } else if(srcv.is_known() && dstv.is_known()) {
      srcflag = srcv.int_val();
      dstflag = dstv.int_val();
    } else
      srcflag = dstflag = 1;

    boolean invoflag = false;
    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      comp = (PipeComp*) pipev.geta(InvoComp::class_symid());
      invoflag = true;
    }
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    AttributeValueList* al = new AttributeValueList();
    if (srcflag) {
      for (int i=0; i<comp->nsrc(); i++) {
	int order;
	PipeComp* neighbor = comp->remote_srccomp(i, order, pipeflag);
	if (neighbor!=nil) {
	  ComValue* neighborv = new ComValue(new OverlayViewRef(neighbor), 
					     neighbor->virtual_classid());
	  al->Append(neighborv);
	} else 
	  al->Append(new AttributeValue());
      }
    }
    if (dstflag) {
      for (int i=0; i<comp->ndst(); i++) {
	int order;
	PipeComp* neighbor = comp->remote_dstcomp(i, order, pipeflag);
	if (neighbor!=nil) {
	  ComValue* neighborv = new ComValue(new OverlayViewRef(neighbor),
					     neighbor->virtual_classid());
	  al->Append(neighborv);
	} 
	else 
	  al->Append(new AttributeValue());
      }
    }
      
    ComValue retval(al);
    push_stack(retval);
}


/*****************************************************************************/

SubSrcPipeFunc::SubSrcPipeFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SubSrcPipeFunc::execute() {
    ComValue invov(stack_arg(0));
    ComValue indexv(stack_arg(1));
    reset_stack();

    InvoComp* comp = (InvoComp*) invov.geta(InvoComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }
    
    PipeComp* subpipe = comp->find_subpipe(true, indexv.int_val());
    if (subpipe) {
      ComValue retval(new OverlayViewRef(subpipe), subpipe->virtual_classid());
      push_stack(retval);
      return;
    } else {
      push_stack(ComValue::nullval());
      return;
    }
}

/*****************************************************************************/

SubDstPipeFunc::SubDstPipeFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SubDstPipeFunc::execute() {
    ComValue invov(stack_arg(0));
    ComValue indexv(stack_arg(1));
    reset_stack();

    InvoComp* comp = (InvoComp*) invov.geta(InvoComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }
    
    PipeComp* subpipe = comp->find_subpipe(false, indexv.int_val());
    if (subpipe) {
      ComValue retval(new OverlayViewRef(subpipe), subpipe->virtual_classid());
      push_stack(retval);
      return;
    } else {
      push_stack(ComValue::nullval());
      return;
    }
}

/*****************************************************************************/

SrcConnsFunc::SrcConnsFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SrcConnsFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    boolean invoflag = false;
    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      comp = (PipeComp*) pipev.geta(InvoComp::class_symid());
      invoflag = true;
    }
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    AttributeValueList* al = new AttributeValueList();
    for (int i=0; i<comp->nsrc(); i++) {
      ConnComp* srcconn = nil;
      if (comp->IsA(INVO_COMP)) {
	PipeComp* pipecomp = ((InvoComp*)comp)->find_subpipe(true, i);
	srcconn = pipecomp->srcconn(0);
      } else
	srcconn = comp->srcconn(i);
      ComValue* srcconnv = new ComValue(new OverlayViewRef(srcconn), srcconn->virtual_classid());
      al->Append(srcconnv);
    }

    ComValue retval(al);
    push_stack(retval);
}

/*****************************************************************************/

DstConnsFunc::DstConnsFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void DstConnsFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    boolean invoflag = false;
    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      comp = (PipeComp*) pipev.geta(InvoComp::class_symid());
      invoflag = true;
    }
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    AttributeValueList* al = new AttributeValueList();
    for (int i=0; i<comp->ndst(); i++) {
      ConnComp* dstconn = nil;
      if (comp->IsA(INVO_COMP)) {
	PipeComp* pipecomp = ((InvoComp*)comp)->find_subpipe(false, i);
	dstconn = pipecomp->dstconn(0);
      } else
	dstconn = comp->dstconn(i);
      ComValue* dstconnv = new ComValue(new OverlayViewRef(dstconn), dstconn->virtual_classid());
      al->Append(dstconnv);
    }

    ComValue retval(al);
    push_stack(retval);
}

/*****************************************************************************/

ConnViewFunc::ConnViewFunc(ComTerp* comterp, Editor* ed) : UnidrawFunc(comterp, ed) {
}

void ConnViewFunc::execute() {
  ComValue conncompv(stack_arg(0));
  reset_stack();

  ConnComp* conncomp = (ConnComp*) conncompv.geta(ConnComp::class_symid());
  if (conncomp) {

    conncomp->build_conn_graphic();
    
    PasteCmd* cmd = new PasteCmd(_ed, new Clipboard(conncomp));
    execute_log(cmd);
    push_stack(conncompv);
    
  } else 
    push_stack(ComValue::nullval());
}

/*****************************************************************************/

PipeViewFunc::PipeViewFunc(ComTerp* comterp, Editor* ed) : UnidrawFunc(comterp, ed) {
}

void PipeViewFunc::execute() {
  ComValue pipecompv(stack_arg(0));
  reset_stack();

  PipeComp* pipecomp = (PipeComp*) pipecompv.geta(PipeComp::class_symid());
  if (pipecomp) {

    pipecomp->build_pipe_graphic();
    
    PasteCmd* cmd = new PasteCmd(_ed, new Clipboard(pipecomp));
    execute_log(cmd);
    push_stack(pipecompv);
    
  } else 
    push_stack(ComValue::nullval());
}

/*****************************************************************************/

InvoViewFunc::InvoViewFunc(ComTerp* comterp, Editor* ed) : UnidrawFunc(comterp, ed) {
}

void InvoViewFunc::execute() {
  ComValue invocompv(stack_arg(0));
  reset_stack();

  InvoComp* invocomp = (InvoComp*) invocompv.geta(InvoComp::class_symid());
  if (invocomp) {

    invocomp->build_invocation_graphic();
    
    PasteCmd* cmd = new PasteCmd(_ed, new Clipboard(invocomp));
    execute_log(cmd);
    push_stack(invocompv);
    
  } else 
    push_stack(ComValue::nullval());
}

/*****************************************************************************/

HandsoutFunc::HandsoutFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void HandsoutFunc::execute() {
    ComValue pipev(stack_arg(0));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ForkComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->handsout());
    push_stack(retval);
    
}

/*****************************************************************************/

HandoutFunc::HandoutFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void HandoutFunc::execute() {
    ComValue connv(stack_arg(0));
    reset_stack();

    ConnComp* comp = (ConnComp*) connv.geta(ConnComp::class_symid());
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    ComValue retval(comp->handout());
    push_stack(retval);
    
}

/*****************************************************************************/

MoveText2Func::MoveText2Func(ComTerp* comterp) : ComFunc(comterp) {
}

void MoveText2Func::execute() {
    ComValue pipev(stack_arg(0));
    ComValue dxv(stack_arg(1));
    ComValue dyv(stack_arg(2));
    reset_stack();

    PipeComp* comp = (PipeComp*) pipev.geta(PipeComp::class_symid());
    if (!comp) comp = (PipeComp*) pipev.geta(ArbiterComp::class_symid());
#ifdef HAVE_ACE
    if (!comp) comp = (PipeComp*) pipev.geta(DistantComp::class_symid());
#endif
    if (!comp) {
      push_stack(ComValue::nullval());
      return;
    }

    comp->MoveText2(dxv.int_val(), dyv.int_val());
    push_stack(pipev);
    
}

/*****************************************************************************/

SpritesFunc::SpritesFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void SpritesFunc::execute() {
  reset_stack();
  
  if (SpriteQueue::instance()) {
    SpriteQueue::instance()->run(100);
    ComValue retval(ComValue::trueval());
    push_stack(retval);
  } else {
    fprintf(stderr, "spritequeue not initialized\n");
    push_stack(ComValue::nullval());
  }

}

/*****************************************************************************/

PipeAnimateFunc::PipeAnimateFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void PipeAnimateFunc::execute() {
  if (nargs()) {
    ComValue argv(stack_arg(0));
    PipeComp::_animateflag = argv.int_val();
    reset_stack();
    push_stack(argv);
  } else {
    reset_stack();
    ComValue retval(PipeComp::_animateflag, ComValue::IntType);
    push_stack(retval);
  }
}

/*****************************************************************************/

InvoUnitaryFunc::InvoUnitaryFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void InvoUnitaryFunc::execute() {
  if (nargs()) {
    ComValue argv(stack_arg(0));
    PipeComp::_unitaryflag = argv.int_val();
    reset_stack();
    push_stack(argv);
  } else {
    reset_stack();
    ComValue retval(PipeComp::_unitaryflag, ComValue::IntType);
    push_stack(retval);
  }
}

/*****************************************************************************/

LastTickFunc::LastTickFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void LastTickFunc::execute() {
    reset_stack();

    ComValue retval(PipeComp::_eventqueue ? PipeComp::_eventqueue->lasttick() : 0);
    push_stack(retval);
}

/*****************************************************************************/

StatsCommFunc::StatsCommFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void StatsCommFunc::execute() {
  ComValue commandv(stack_arg(0));
  reset_stack();

  if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
  ComValue retval(PipeComp::_eventqueue->stats_command(comterpserv(), commandv.string_ptr()));
  push_stack(retval);
}

/*****************************************************************************/

StatsEvalFunc::StatsEvalFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void StatsEvalFunc::execute() {
  reset_stack();

  if (!PipeComp::_eventqueue) PipeComp::_eventqueue = new PipeEventQueue();
  ComValue retval = PipeComp::_eventqueue->stats_eval();
  push_stack(retval);
}

