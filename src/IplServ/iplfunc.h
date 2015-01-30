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

#ifndef iplfunc_h
#define iplfunc_h

#include <ComTerp/comfunc.h>
#include <ComTerp/comterp.h>
#include <IplServ/iplcomps.h>

class ComTerp;

class IplFunc : public ComFunc {
public:
    IplFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "ipl(:echo :debug) -- switches to ipl syntax"; }
    void enable(int debug);
    void disable();

    static int debug() { return _debug; }
    static void debug(int flag) { _debug = flag; }

protected:
    static int _debug;
};

class AngleBracketsFlagFunc : public ComFunc {
public:
    AngleBracketsFlagFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=anglebracketsflag([flag]) -- sets/gets angle brackets flag"; }

protected:
};
   
class CreatePipeFunc : public ComFunc {
public:
    CreatePipeFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=pipe([buffsize [namesym]]) -- creates a pipe object"; }

protected:
};
   
class CreatePipesFunc : public ComFunc {
public:
    CreatePipesFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=pipes([buffsize]) -- creates a pipes object (basis for arbitration or completion detection)"; }

protected:
};
   
class CreateInvoPipesFunc : public ComFunc {
public:
    CreateInvoPipesFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=invopipes(srcpipes [dstpipes]) -- creates an invocation pipes object"; }

protected:
};
   

class CreateInvoFunc : public ComFunc {
public:
    CreateInvoFunc(ComTerp*);
    virtual void execute();
    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "obj=invo(func invopipes) -- creates an invocation object"; }

protected:
};
   

class BuildInvoFunc : public ComFunc {
public:
    BuildInvoFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=buildinvo(funcname ndst nsrc) -- build internals of an invocation object"; }

protected:
};
   

class ConnectDstToSrcFunc : public ComFunc {
public:
    ConnectDstToSrcFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=conn(dst src [conn]) -- connects destination to a source"; }

protected:
};
   

class CreateArbiterFunc : public ComFunc {
public:
    CreateArbiterFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=arbiter() -- creates a arbiter object"; }

protected:
};
   
class ConnectArbiterFunc : public ComFunc {
public:
    ConnectArbiterFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=mconn(dest1 dest2 src [arbiter]) -- create/connect up a arbiter object"; }

protected:
};
   
class BuffSizeFunc : public ComFunc {
public:
    BuffSizeFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "max=buffsize(pipe [max]) -- set/get maximum size of pipe's buffer"; }

protected:
};
   
class BuffFunc : public ComFunc {
public:
    BuffFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=buff(pipe) -- return buffer associated with pipe"; }

protected:
};
   
class PutRateFunc : public ComFunc {
public:
    PutRateFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=putrate(pipe) -- initialize then return rate of puts per cycle"; }

protected:
};
   
class NumSrcFunc : public ComFunc {
public:
    NumSrcFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "n=nsrc(pipe) -- return number of sources associated with pipe"; }

protected:
};
   
class NumDstFunc : public ComFunc {
public:
    NumDstFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "n=ndst(pipe) -- return number of destinations associated with pipe"; }

protected:
};
   
class SrcVacantFunc : public ComFunc {
public:
    SrcVacantFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "bool=srcvacant(src) -- check if source end of a pipeline is ready to receive more"; }

protected:
};
   
class DstOccupiedFunc : public ComFunc {
public:
    DstOccupiedFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "bool=dstfull(dst) -- check if destination end of a pipeline is ready with data"; }

protected:
};
   
class SrcPutFunc : public ComFunc {
public:
    SrcPutFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "srcput(src val) -- put a value into the source end of a pipeline"; }

protected:
};
   
class DstGetFunc : public ComFunc {
public:
    DstGetFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=dstget(dst) -- get a value from the destination end of a pipeline"; }

protected:
};

#ifdef HAVE_ACE
class RequestFunc : public ComFunc {
public:
    RequestFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=request(dst) -- request a value from a remote a pipeline"; }

protected:
};
#endif

class PipesAddFunc : public ComFunc {
public:
    PipesAddFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=pipesadd(pipes pipe) -- add a pipe object to a pipes object"; }

protected:
};

class PipeText2Func : public ComFunc {
public:
    PipeText2Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=ptext2(pipe [text]) -- set/get 2nd text string associated with a pipe object"; }

protected:
};

class GetInvoPipesFunc : public ComFunc {
public:
    GetInvoPipesFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=getinvopipes(invo) -- return the invocation pipes object"; }

protected:
};

class GetSrcPipesFunc : public ComFunc {
public:
    GetSrcPipesFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=getsrcpipes(invopipes) -- return the source pipes of an invocation pipes object"; }

protected:
};

class GetDstPipesFunc : public ComFunc {
public:
    GetDstPipesFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=getdstpipes(invopipes) -- return the destination pipes of an invocation pipes object"; }

protected:
};

class PipesCompleteFunc : public ComFunc {
public:
    PipesCompleteFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=pipescomplete(pipes) -- return complete flag for compound pipe"; }

protected:
};

#if defined(MANUAL_BREAKPOINT)
class DebugCompFunc : public ComFunc {
public:
    DebugCompFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "debugcomp(comp funcsym) -- set manual breakpoint data items"; }

protected:
};
#endif

class TickFunc : public ComFunc {
public:
    TickFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "stat=tick() -- run all events in queue"; }

protected:
};

class CycleFunc : public ComFunc {
public:
    CycleFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "cycle(:nostats) -- start local cycle running"; }

protected:
};

class CycleCountFunc : public ComFunc {
public:
    CycleCountFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "cyclecnt() -- number of cycles"; }

protected:
};

#ifdef HAVE_ACE
class ReadyFunc : public ComFunc {
public:
    ReadyFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=ready(dst) -- tell remote end that this end is ready"; }

protected:
};
#endif

class LastInputFunc : public ComFunc {
public:
    LastInputFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=lastinput(arbiter) -- return lastinput flag for arbiter"; }

protected:
};
   
class PipeNameFunc : public ComFunc {
public:
    PipeNameFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "str=pipename(pipe :set str) -- get/set name of pipe string"; }

protected:
};
   
class NeighborsFunc : public ComFunc {
public:
    NeighborsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "lst=neighbors(pipe :src [flag] :dst [flag] :pipe) -- return list of neighbor pipe nodes"; }

protected:
};

class SubSrcPipeFunc : public ComFunc {
public:
    SubSrcPipeFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "comp=subsrc(invo index ) -- return nth subpipe on the src side"; }

protected:
};

class SubDstPipeFunc : public ComFunc {
public:
    SubDstPipeFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "comp=subdst(invo index ) -- return nth subpipe on the dst side"; }

protected:
};

class SrcConnsFunc : public ComFunc {
public:
    SrcConnsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "lst=srcconns(pipe) -- return list of source connections"; }

protected:
};
   
class DstConnsFunc : public ComFunc {
public:
    DstConnsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "lst=dstconns(pipe) -- return list of destination connections"; }

protected:
};
   
#include <ComUnidraw/unifunc.h>

class ConnViewFunc : public UnidrawFunc {
public:
    ConnViewFunc(ComTerp*, Editor*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=connview(obj) -- create a connection graphic"; }

};

class PipeViewFunc : public UnidrawFunc {
public:
    PipeViewFunc(ComTerp*, Editor*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=pipeview(obj) -- create a pipe node graphic"; }

};

class InvoViewFunc : public UnidrawFunc {
public:
    InvoViewFunc(ComTerp*, Editor*);
    virtual void execute();
    virtual const char* docstring() { 
      return "obj=invoview(obj) -- create an invocation graphic"; }

};

class HandsoutFunc : public ComFunc {
public:
    HandsoutFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=handsout(pipe) -- return handsout count"; }

protected:
};
   
class HandoutFunc : public ComFunc {
public:
    HandoutFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=handout(conn) -- return handout flag"; }

protected:
};
   
class MoveText2Func : public ComFunc {
public:
    MoveText2Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "movetext2(pipe dx dy) -- offset pipe's text descriptor"; }

protected:
};
   
class SpritesFunc : public ComFunc {
public:
    SpritesFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "stat=sprites() -- run all sprites in queue"; }

protected:
};


class PipeAnimateFunc : public ComFunc {
public:
    PipeAnimateFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=animate([flag]) -- set/get the animate flag"; }

};

class InvoUnitaryFunc : public ComFunc {
public:
    InvoUnitaryFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=unitary([flag]) -- set/get the unitary flag"; }

};

class LastTickFunc : public ComFunc {
public:
    LastTickFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "lasttick() -- number of events serviced during last tick"; }

protected:
};

class StatsCommFunc : public ComFunc {
public:
    StatsCommFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "statscomm(commstr) -- set command string for generating stats"; }

protected:
};

class StatsEvalFunc : public ComFunc {
public:
    StatsEvalFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "statseval() -- evaluate stats command string"; }

protected:
};

#endif

