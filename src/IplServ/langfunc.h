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

#ifndef langfunc_h
#define langfunc_h

#include <ComTerp/comfunc.h>
#include <ComTerp/comterp.h>

class ComTerp;

class DestinationFunc : public ComFunc {
public:
    DestinationFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "destination(sym) -- establishes destination end of pipeline"; }

protected:
};

class Destination2Func : public ComFunc {
public:
    Destination2Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "destination2(sym1 sym2) -- for concatenating invocation names from multiple destinations"; }

protected:
};

class SymSpaceFunc : public ComFunc {
public:
    SymSpaceFunc(ComTerp*);
    virtual void execute();
    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "symspace(sym) -- destination end of symbol space"; }

protected:
};

class SymSpaceNextFunc : public ComFunc {
public:
    SymSpaceNextFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "hidden func used by next command for symspace"; }

protected:
};

class SymSpace2Func : public ComFunc {
public:
    SymSpace2Func(ComTerp*);
    virtual void execute();
    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "symspace2(sym strm) -- destination end of symbol space to be concatenated"; }

protected:
};

class SymSpace2NextFunc : public ComFunc {
public:
    SymSpace2NextFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "hidden func used by next command for symspace2"; }

protected:
};

class DestSymSpaceFunc : public ComFunc {
public:
    DestSymSpaceFunc(ComTerp*);
    virtual void execute();
    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "destsymspace(sym) -- destination end of symbol space combined with normal destination"; }

protected:
};

class DestSymSpaceNextFunc : public ComFunc {
public:
    DestSymSpaceNextFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "hidden func used by next command for destsymspace"; }

protected:
};

class DestSymSpace2Func : public ComFunc {
public:
    DestSymSpace2Func(ComTerp*);
    virtual void execute();
    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "destsymspace2(sym strm) -- destination end of symbol space to be concatenated combined with normal destination"; }

protected:
};

class DestSymSpace2NextFunc : public ComFunc {
public:
    DestSymSpace2NextFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "hidden func used by next command for destsymspace2"; }

protected:
};

class SeparatorFunc : public ComFunc {
public:
    SeparatorFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "separator() -- comma separator"; }

protected:
};


class InvoDefFunc : public ComFunc {
public:
    InvoDefFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "defcomp=def(name path [initpath]) -- return comp with executable graph)"; }

protected:
};

class IplIdrawComp;
class InvoDefFuncFunc : public ComFunc {
public:
    InvoDefFuncFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "dst0|dst0,dst1[,dst2]}=func(src0 [src1 [src2]]) -- setup func with defcomp"; }

    IplIdrawComp* defcomp() { return _defcomp; }
    void defcomp(IplIdrawComp* comp) { _defcomp = comp; }
protected:
    IplIdrawComp* _defcomp;
};

class TranscribePipeFunc : public ComFunc {
public:
    TranscribePipeFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "transcribe(pipe) -- transcribe pipe to external format"; }

    static void randchars(char* randbuf, int size);

protected:
};

#endif

