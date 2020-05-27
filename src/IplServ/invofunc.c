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

#include <IplServ/eventqueue.h>
#include <IplServ/invofunc.h>
#include <IplServ/iplclasses.h>
#include <IplServ/iplcomps.h>
#include <Unidraw/iterator.h>
#include <ComTerp/bitfunc.h>
#include <ComTerp/boolfunc.h>
#include <ComTerp/randfunc.h>
#include <Attribute/attrlist.h>
#include <Attribute/attrvalue.h>
using std::cout;

/*****************************************************************************/

InvoFunc::InvoFunc(ComTerp* comterp) : ComFunc(comterp) {
  _invocomp = nil;
}

/*****************************************************************************/

InvoGreaterThanFunc::InvoGreaterThanFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoGreaterThanFunc::execute() {
  ComValue op0v(stack_arg(0));
  ComValue op1v(stack_arg(1));
  GreaterThanFunc func(comterp());
  func.execute();
  ComValue boolv(pop_stack());
  AttributeValueList* list = new AttributeValueList();
  if (boolv.is_true()) {
    list->Append(new AttributeValue(op0v));
    list->Append(new AttributeValue(op1v));
  } else {
    list->Append(new AttributeValue(op1v));
    list->Append(new AttributeValue(op0v));
  }
  ComValue retval(list);
  push_stack(retval);
}

/*****************************************************************************/

InvoLessThanFunc::InvoLessThanFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoLessThanFunc::execute() {
  ComValue op0v(stack_arg(0));
  ComValue op1v(stack_arg(1));
  LessThanFunc func(comterp());
  func.execute();
  ComValue boolv(pop_stack());
  AttributeValueList* list = new AttributeValueList();
  if (boolv.is_true()) {
    list->Append(new AttributeValue(op0v));
    list->Append(new AttributeValue(op1v));
  } else {
    list->Append(new AttributeValue(op1v));
    list->Append(new AttributeValue(op0v));
  }
  ComValue retval(list);
  push_stack(retval);
}

/*****************************************************************************/

InvoPassFunc::InvoPassFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoPassFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(valv);
  return;
}


/*****************************************************************************/

InvoPass2Func::InvoPass2Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoPass2Func::execute() {
  ComValue val0v(stack_arg(0));
  ComValue val1v(stack_arg(1));
  reset_stack();
  AttributeValueList* list = new AttributeValueList();
  list->Append(new AttributeValue(val0v));
  if (val1v.is_unknown())
    list->Append(new AttributeValue(val0v));
  else
    list->Append(new AttributeValue(val1v));
  ComValue retval(list);
  push_stack(retval);
  return;
}


/*****************************************************************************/

InvoAckFunc::InvoAckFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoAckFunc::execute() {
  // doesn't matter what the args are, all that matters is that they were all ready
  reset_stack();
  push_stack(ComValue::trueval());
  return;
}

/*****************************************************************************/

InvoRdyFunc::InvoRdyFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoRdyFunc::execute() {
  ComValue valv(stack_arg(0));
  // doesn't matter what the acknowledge arg is, all that matters is that it was there
  reset_stack();
  
  if (valv.is_array()) {
    AttributeValueList* list = new AttributeValueList();
    list->Append(new AttributeValue(valv.array_val()));
    ComValue retval(list);
    push_stack(retval);
  } else
    push_stack(valv);
  return;
}


/*****************************************************************************/

InvoSubTestFunc::InvoSubTestFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoSubTestFunc::execute() {
  ComValue op0v(stack_arg(0));
  ComValue op1v(stack_arg(1));
  SubFunc subfunc(comterp());
  subfunc.execute();
  ComValue result(pop_stack());
  push_stack(result);
  push_stack(ComValue::zeroval());
  LessThanFunc ltfunc(comterp());
  ltfunc.exec(2,0);
  ComValue ltzero(pop_stack());
  AttributeValueList* list = new AttributeValueList();
  list->Append(new AttributeValue(result));
  list->Append(new AttributeValue(ltzero));
  ComValue retval(list);
  push_stack(retval);
}

/*****************************************************************************/

InvoCondFunc::InvoCondFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoCondFunc::execute() {
  ComValue flagv(stack_arg(0));
  ComValue op0v(stack_arg(1));
  ComValue op1v(stack_arg(2));
  reset_stack();
  if (flagv.is_true())
    push_stack(op0v);
  else
    push_stack(op1v);
  return;
}

/*****************************************************************************/

InvoAdd1Func::InvoAdd1Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoAdd1Func::execute() {
  ComValue op0v(stack_arg(0));
  reset_stack();
  push_stack(op0v);
  push_stack(ComValue::oneval());
  AddFunc func(comterp());
  func.exec(2, 0);
}

/*****************************************************************************/

InvoSub1Func::InvoSub1Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoSub1Func::execute() {
  ComValue op0v(stack_arg(0));
  reset_stack();
  push_stack(op0v);
  push_stack(ComValue::oneval());
  SubFunc func(comterp());
  func.exec(2, 0);
}

/*****************************************************************************/

InvoLsbFunc::InvoLsbFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoLsbFunc::execute() {
  ComValue op0v(stack_arg(0));
  reset_stack();
  AttributeValueList* al = new AttributeValueList();

  push_stack(op0v);
  push_stack(ComValue::oneval());
  BitAndFunc andfunc(comterp());
  andfunc.exec(2, 0);
  ComValue lsbval(comterp()->pop_stack());
  al->Append(new AttributeValue(lsbval));

  push_stack(op0v);
  push_stack(ComValue::oneval());
  RightShiftFunc rshiftfunc(comterp());
  rshiftfunc.exec(2, 0);
  ComValue shiftval(comterp()->pop_stack());
  al->Append(new AttributeValue(shiftval));

  ComValue retval(al);
  push_stack(retval);
}

/*****************************************************************************/

InvoLeftShift1Func::InvoLeftShift1Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoLeftShift1Func::execute() {
  ComValue op0v(stack_arg(0));
  reset_stack();
  push_stack(op0v);
  push_stack(ComValue::oneval());
  LeftShiftFunc func(comterp());
  func.exec(2, 0);
}

/*****************************************************************************/

InvoRightShift1Func::InvoRightShift1Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoRightShift1Func::execute() {
  ComValue op0v(stack_arg(0));
  reset_stack();
  push_stack(op0v);
  push_stack(ComValue::oneval());
  RightShiftFunc func(comterp());
  func.exec(2, 0);
}

/*****************************************************************************/

InvoRand1Func::InvoRand1Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoRand1Func::execute() {
  reset_stack();
  AttributeValueList* alist = new AttributeValueList();
  ComValue alistv(alist);
  alist->Append(new ComValue(0.0));
  alist->Append(new ComValue(2.0));
  push_stack(alistv);
  RandFunc func(comterp());
  func.exec(1, 0);
  IntFunc ifunc(comterp());
  ifunc.exec(1, 0);
}

/*****************************************************************************/

InvoSinkFunc::InvoSinkFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoSinkFunc::execute() {
  ComValue sinkv(stack_arg(0, true));
  reset_stack();
  if (!context() || !context()->IsA(ALU_COMP)) {
    Iterator it;
    InvoComp* invocomp = (InvoComp*)context();
    if(invocomp->buff()->Number()>=25) {
      invocomp->buff()->Last(it);
      AttributeValue* av = invocomp->buff()->GetAttrVal(it);
      invocomp->buff()->Remove(it);
      delete av;
    }
    invocomp->buff()->First(it);
    invocomp->buff()->Prepend(new AttributeValue(sinkv));
  }
    
  if(context() && context()->IsA(ALU_COMP))
    push_stack(ComValue::nullval());
  else
    push_stack(ComValue::blankval());
  return;
}

/*****************************************************************************/

InvoSteerCondFunc::InvoSteerCondFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoSteerCondFunc::execute() {
  ComValue steerv(stack_arg(0));
  reset_stack();

  InvoComp* invocomp = (InvoComp*)(context() && context()->IsA(INVO_COMP) ? context() : nil);
  int ndst = invocomp ? invocomp->expected_ndst() : 0;
  if (ndst>1 /* && steerv.int_val()>=0 */ ) {
    AttributeValueList* list = new AttributeValueList();
    for (int i=0; i<abs(steerv.int_val())&&i<ndst-1; i++) {
      if (context()->IsA(ALU_COMP)) 
	list->Append(new AttributeValue(ComValue::nullval()));
      else
	list->Append(new AttributeValue(ComValue::blankval()));
    }
    list->Append(new AttributeValue(steerv));
    for (int i=max(ndst-1,abs(steerv.int_val())+1); i<ndst; i++) {
      if (context()->IsA(ALU_COMP)) 
	list->Append(new AttributeValue(ComValue::nullval()));
      else
	list->Append(new AttributeValue(ComValue::blankval()));
    }
    ComValue retval(list);
    push_stack(retval);
  } else 
    push_stack(context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval());
  return;
}

/*****************************************************************************/

InvoSteer0Func::InvoSteer0Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoSteer0Func::execute() {
  ComValue steerv(stack_arg(0));
  reset_stack();

  InvoComp* invocomp = (InvoComp*)(context() && context()->IsA(INVO_COMP) ? context() : nil);
  int ndst = invocomp ? invocomp->expected_ndst() : 0;
  AttributeValueList* list = new AttributeValueList();
  list->Append(new AttributeValue(steerv));

  for (int i=1; i<ndst; i++) {
    if (context()->IsA(ALU_COMP)) 
      list->Append(new AttributeValue(ComValue::nullval()));
    else
      list->Append(new AttributeValue(ComValue::blankval()));
  }
  ComValue retval(list);
  push_stack(retval);
}

/*****************************************************************************/

InvoSteerValueFunc::InvoSteerValueFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoSteerValueFunc::execute() {
  const int size = nargsfixed()-1;
  ComValue steerv(stack_arg(0));
  AttributeValue* argsv = new AttributeValue[size];
  for (int i=0; i<size; i++)
    argsv[i] = stack_arg(i+1);
  reset_stack();

  AttributeValueList* list = new AttributeValueList();
  if (steerv.int_val())
    for (int i=0; i<size; i++) 
      list->Append(new AttributeValue(context() && context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval()));

/* removed June 2009 working on transpose element */
#define DONT_KNOW_WHY

  for (int i=0; i<size; i++) {
#ifndef DONT_KNOW_WHY
    if(argsv[i].is_array() && (!context() || !context()->IsA(ALU_COMP))) {
      AttributeValueList* newlist = new AttributeValueList();
      newlist->Append(new AttributeValue(argsv[i]));
      list->Append(new AttributeValue(newlist));
    } else
#endif
      list->Append(new AttributeValue(argsv[i]));
  }
  
  if (!steerv.int_val())
    for (int i=0; i<size; i++) 
      list->Append(new AttributeValue(context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval()));

  ComValue retval(list);
  push_stack(retval);
  delete [] argsv;
  return;
}

/*****************************************************************************/

InvoSteerDelimFunc::InvoSteerDelimFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoSteerDelimFunc::execute() {
  static int delimsymid = symbol_add("__DELIM__");
  ComValue vordv(stack_arg(0, true));
  reset_stack();

  InvoComp* invocomp = (InvoComp*)(context() && context()->IsA(INVO_COMP) ? context() : nil);
  AttributeValueList* list = new AttributeValueList();
  boolean delim_flag = vordv.symbol_val() == delimsymid;
  int ndst = invocomp ? invocomp->expected_ndst() : 0;
  if (ndst==2) {
    if (delim_flag) list->Append(new AttributeValue(vordv));
    if (context()->IsA(ALU_COMP)) 
      list->Append(new AttributeValue(ComValue::nullval()));
    else
      list->Append(new AttributeValue(ComValue::blankval()));
    if (!delim_flag) list->Append(new AttributeValue(vordv));
    ComValue retval(list);
    push_stack(retval);
  } else 
    push_stack(context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval());
  return;
}

/*****************************************************************************/

InvoFourWayFunc::InvoFourWayFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoFourWayFunc::execute() {
  ComValue valorlistv(stack_arg(0));
  reset_stack();

  int steering;
  if (valorlistv.is_list()) {
    Iterator it;
    AttributeValueList* inlist = valorlistv.array_val();
    inlist->First(it);
    AttributeValue* av = inlist->GetAttrVal(it);
    steering = av->int_val();
  } else
    steering = valorlistv.int_val();
  steering = steering % 4;


  AttributeValueList* list = new AttributeValueList();
    for (int i=0; i<steering; i++) 
      list->Append(new AttributeValue(context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval()));

    list->Append(new AttributeValue(valorlistv));
  
    for (int i=steering+1; i<4; i++) 
      list->Append(new AttributeValue(context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval()));

  ComValue retval(list);
  push_stack(retval);
  return;
}

/*****************************************************************************/

InvoEightWayFunc::InvoEightWayFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoEightWayFunc::execute() {
  ComValue valorlistv(stack_arg(0));
  reset_stack();

  int steering;
  if (valorlistv.is_list()) {
    Iterator it;
    AttributeValueList* inlist = valorlistv.array_val();
    inlist->First(it);
    AttributeValue* av = inlist->GetAttrVal(it);
    steering = av->int_val();
  } else
    steering = valorlistv.int_val();
  steering = steering % 8;


  AttributeValueList* list = new AttributeValueList();
    for (int i=0; i<steering; i++) 
      list->Append(new AttributeValue(context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval()));

    list->Append(new AttributeValue(valorlistv));
  
    for (int i=steering+1; i<8; i++) 
      list->Append(new AttributeValue(context()->IsA(ALU_COMP) ? ComValue::nullval() : ComValue::blankval()));

  ComValue retval(list);
  push_stack(retval);
  return;
}

/*****************************************************************************/

PipeEventQueueFunc::PipeEventQueueFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void PipeEventQueueFunc::execute() {
  static int size_symid = symbol_add("size");
  ComValue size_flagv(stack_key(size_symid));
  static int dump_symid = symbol_add("dump");
  ComValue dump_flagv(stack_key(dump_symid));

  if (size_flagv.is_known()) {
    reset_stack();
    ComValue retval(PipeComp::_eventqueue ? PipeComp::_eventqueue->Number() : 0);
    push_stack(retval);
    return;
  }

  if (dump_flagv.is_known()) {
    reset_stack();
    cout << *PipeComp::_eventqueue;
    push_stack(ComValue::zeroval());
    return;
  }

  if (nargs()) {
    ComValue argv(stack_arg(0));
    PipeComp::_eventqueueflag = argv.int_val();
    reset_stack();
    push_stack(argv);
  } else {
    reset_stack();
    ComValue retval(PipeComp::_eventqueueflag, ComValue::IntType);
    push_stack(retval);
  }
}

/*****************************************************************************/

int PipeNextEventFunc::_count = 0;

PipeNextEventFunc::PipeNextEventFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void PipeNextEventFunc::execute() {
  reset_stack();
#if 0
  if (PipeComp::_eventqueueflag && PipeComp::_eventqueue->Number()>0) {
    Iterator it;
    PipeComp::_eventqueue->First(it);
    PipeEvent* event = PipeComp::_eventqueue->GetPipeEvent(it);
    PipeComp::_eventqueue->Remove(it);
    if (event->notify_flag()) {
      if (event->ispipe()) {
	event->pipe()->_dst_notify();
	event->pipe()->dst_notified(0);
     } else {
	event->pipes()->_dst_notify();
	event->pipes()->dst_notified(0);
      }
    }
    else {
      if (event->ispipe()) {
	event->pipe()->_src_request();
	event->pipe()->src_requested(0);
      } else {
	event->pipes()->_src_request();
	event->pipes()->src_requested(0);
      }
    }
    delete event;
    push_stack(ComValue::oneval());  
  } else {
    push_stack(ComValue::zeroval());
  }
#else
  if (PipeComp::_eventqueue) {
    int status = PipeComp::_eventqueue->nextevent();
    ComValue retval(status==0 ? 1 : 0, ComValue::IntType);
    push_stack(retval);
  } else {
    fprintf(stderr, "eventqueue is not initialized\n");
    push_stack(ComValue::nullval());
  }
#endif
  _count++;
}

/*****************************************************************************/

PipeNNextEventFunc::PipeNNextEventFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void PipeNNextEventFunc::execute() {
  static int request_symid = symbol_add("request");
  ComValue request_flagv(stack_key(request_symid));
  static int notify_symid = symbol_add("notify");
  ComValue notify_flagv(stack_key(notify_symid));
  reset_stack();

  if (PipeComp::_eventqueue) {
    int notifyflag;
    if (request_flagv.is_true())
      notifyflag = notify_flagv.is_true() ? -1 : 0;
    else if (notify_flagv.is_true())
      notifyflag = request_flagv.is_true() ? -1 : 1;
    else
      notifyflag = -1;
    int nevents = PipeComp::_eventqueue->nnextevent(notifyflag);
    ComValue retval(nevents, ComValue::IntType);
    push_stack(retval);
  } else {
    fprintf(stderr, "eventqueue is not initialized\n");
    push_stack(ComValue::nullval());
  }
}

/*****************************************************************************/

MergeFunc::MergeFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void MergeFunc::execute() {
  int size = nargsfixed();
  AttributeValueList* list = new AttributeValueList();
  for (int i=0; i<size; i++)
    list->Append(new AttributeValue(stack_arg(i)));
  reset_stack();
  ComValue retval(list);
  push_stack(retval);
}

/*****************************************************************************/

SteerFunc::SteerFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void SteerFunc::execute() {
  int size = nargsfixed();
  AttributeValue* argsv = new AttributeValue[size];
  for (int i=0; i<size; i++)
    argsv[i] = stack_arg(i+1);
  reset_stack();

  AttributeValue* lastv = argsv+size-1;
  int steer = lastv->int_val();
  int lsb = steer&0x1;
  steer = steer >> 1;
  lastv->int_ref() = steer;

  AttributeValueList* list = new AttributeValueList();
  if (lsb) 
    for (int i=0; i<size; i++) 
      list->Append(new AttributeValue(ComValue::blankval()));

  for (int i=0; i<size; i++)
	 list->Append(new AttributeValue(argsv[i]));
  
  if (!lsb) 
    for (int i=0; i<size; i++) 
      list->Append(new AttributeValue(ComValue::blankval()));

  ComValue retval(list);
  push_stack(retval);
  delete [] argsv;
  return;
}

/*****************************************************************************/

SteerJoinFunc::SteerJoinFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void SteerJoinFunc::execute() {
  int size = nargsfixed();
  AttributeValue* argsv = new AttributeValue[size];
  for (int i=0; i<size; i++)
    argsv[i] = stack_arg(i+1);
  reset_stack();

  AttributeValue* lastv = argsv+size-1;
  int steer = lastv->int_val();
  int lsb = steer&0x1;
  steer = steer >> 1;
  lastv->int_ref() = steer;

  AttributeValueList* toplist = new AttributeValueList();
  AttributeValueList* list = new AttributeValueList();
  if (lsb) 
    toplist->Append(new AttributeValue(ComValue::blankval()));

  for (int i=0; i<size; i++)
	 list->Append(new AttributeValue(argsv[i]));
  toplist->Append(new AttributeValue(list));
  
  if (!lsb) 
    toplist->Append(new AttributeValue(ComValue::blankval()));

  ComValue retval(toplist);
  push_stack(retval);
  delete [] argsv;
  return;
}

/*****************************************************************************/

SplitPathsFunc::SplitPathsFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void SplitPathsFunc::execute() {
  ComValue listv(stack_arg(0));
  reset_stack();
  push_stack(listv);
}

/*****************************************************************************/

JoinPathsFunc::JoinPathsFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void JoinPathsFunc::execute() {
  int size = nargsfixed();
  AttributeValueList* list = new AttributeValueList();
  for (int i=0; i<size; i++)
    list->Append(new AttributeValue(stack_arg(i)));
  reset_stack();
  AttributeValueList* list2 = new AttributeValueList();
  list2->Append(new AttributeValue(list));
  ComValue retval(list2);
  push_stack(retval);
}

/*****************************************************************************/

InvoTrueFunc::InvoTrueFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoTrueFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(ComValue::oneval());
  return;
}

/*****************************************************************************/

InvoFalseFunc::InvoFalseFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoFalseFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(ComValue::zeroval());
  return;
}

/*****************************************************************************/

InvoOneFunc::InvoOneFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoOneFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(ComValue::oneval());
  return;
}

/*****************************************************************************/

InvoZeroFunc::InvoZeroFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoZeroFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(ComValue::zeroval());
  return;
}

/*****************************************************************************/

InvoInvertFunc::InvoInvertFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoInvertFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(ComValue::oneval());
  push_stack(valv);
  DivFunc func(comterp());
  func.exec(2, 0);
  return;
}


/*****************************************************************************/

InvoMeanFunc::InvoMeanFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoMeanFunc::execute() {
  int size = nargsfixed();
  AttributeValue* argsv = new AttributeValue[size];
  for (int i=0; i<size; i++)
    argsv[i] = stack_arg(i+1);
  reset_stack();

  AddFunc addfunc(comterp());
  ComValue sumv(ComValue::zeroval());
  push_stack(sumv);
  for (int i=0; i<size; i++) {
    push_stack(argsv[i]);
    addfunc.exec(2, 0);
  }

  DivFunc divfunc(comterp());
  ComValue sizev(size, ComValue::IntType);
  push_stack(sizev);
  divfunc.exec(2, 0);

  delete [] argsv;
  return;
}

/*****************************************************************************/

InvoGt0Func::InvoGt0Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoGt0Func::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(valv);
  push_stack(ComValue::zeroval());
  GreaterThanFunc func(comterp());
  func.exec(2, 0);
  return;
}

/*****************************************************************************/

InvoLt0Func::InvoLt0Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoLt0Func::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(valv);
  push_stack(ComValue::zeroval());
  LessThanFunc func(comterp());
  func.exec(2, 0);
  return;
}

/*****************************************************************************/

InvoEq0Func::InvoEq0Func(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoEq0Func::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(valv);
  push_stack(ComValue::zeroval());
  EqualFunc func(comterp());
  func.exec(2, 0);
  return;
}

/*****************************************************************************/

InvoOddFunc::InvoOddFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoOddFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(valv);
  push_stack(ComValue::oneval());
  BitAndFunc func(comterp());
  func.exec(2, 0);
  return;
}

/*****************************************************************************/

InvoEvenFunc::InvoEvenFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoEvenFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  push_stack(valv);
  BitNotFunc notfunc(comterp());
  notfunc.exec(1, 0);
  push_stack(ComValue::oneval());
  BitAndFunc andfunc(comterp());
  andfunc.exec(2, 0);
  return;
}

/*****************************************************************************/

InvoTwoFunc::InvoTwoFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoTwoFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  ComValue retval(2, ComValue::IntType);
  push_stack(retval);
  return;
}

/*****************************************************************************/

InvoThreeFunc::InvoThreeFunc(ComTerp* comterp) : InvoFunc(comterp) {
}

void InvoThreeFunc::execute() {
  ComValue valv(stack_arg(0));
  reset_stack();
  ComValue retval(3, ComValue::IntType);
  push_stack(retval);
  return;
}



