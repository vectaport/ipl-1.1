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

#include <IplEdit/iplfunc.h>
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/ipleditor.h>
#include <IplEdit/iplviewer.h>

#include <IplServ/iplcomps.h>

/*****************************************************************************/

IplEditorFunc::IplEditorFunc(ComTerp* comterp, Editor* ed) : UnidrawFunc(comterp, ed) {
}

void IplEditorFunc::execute() {
  reset_stack();
  fprintf(stderr, "--> IplEditorFunc\n");
}

/*****************************************************************************/

SingleStepFunc::SingleStepFunc(ComTerp* comterp, Editor* ed) : IplEditorFunc(comterp, ed) {
}

void SingleStepFunc::execute() {
  if (nargs()) {
    ComValue argv(stack_arg(0));
    PipeComp::_singlestep = argv.int_val();
    reset_stack();
    push_stack(argv);
  } else {
    reset_stack();
    ComValue retval(PipeComp::_singlestep, ComValue::IntType);
    push_stack(retval);
  }
}

/*****************************************************************************/

ShowForkValsFunc::ShowForkValsFunc(ComTerp* comterp, Editor* ed) : IplEditorFunc(comterp, ed) {
}

void ShowForkValsFunc::execute() {
  if (nargs()) {
    ComValue argv(stack_arg(0));
    ForkComp::_showforkvals = argv.int_val();
    reset_stack();
    push_stack(argv);
  } else {
    reset_stack();
    ComValue retval(ForkComp::_showforkvals, ComValue::IntType);
    push_stack(retval);
  }
}

