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

#ifndef iplfunc2_h
#define iplfunc2_h

#include <ComUnidraw/unifunc.h>

class ComTerp;
class Editor;

class IplEditorFunc : public UnidrawFunc {
public:
    IplEditorFunc(ComTerp*, Editor*);
    virtual void execute();

};

class SingleStepFunc : public IplEditorFunc {
public:
    SingleStepFunc(ComTerp*, Editor*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=singlestep([flag]) -- set/get the singlestep flag (0=disable, 1=enable, 2=animate only)"; }

};

class ShowForkValsFunc : public IplEditorFunc {
public:
    ShowForkValsFunc(ComTerp*, Editor*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=showforkvals([flag]) -- set/get the showforkvals flag"; }

};

#endif
