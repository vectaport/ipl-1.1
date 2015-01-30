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

#ifndef invofunc_h
#define invofunc_h

#include <ComTerp/comfunc.h>
#include <ComTerp/comterp.h>

class ComTerp;
class InvoComp;

class InvoFunc : public ComFunc {
public:
    InvoFunc(ComTerp*);
    virtual void execute()=0;
    void invocomp(InvoComp* comp) { _invocomp = comp; }
    InvoComp* invocomp() { return _invocomp; }
protected:
    InvoComp* _invocomp;
};

class InvoGreaterThanFunc : public InvoFunc {
public:
    InvoGreaterThanFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "list=GT(op0 op1) -- return sorted list of values, greatest first"; }

protected:
};

class InvoLessThanFunc : public InvoFunc {
public:
    InvoLessThanFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "list=LT(op0 op1) -- return sorted list of values, least first"; }

protected:
};

class InvoPassFunc : public InvoFunc {
public:
    InvoPassFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=PASS(val) -- do nothing, pass value on"; }

protected:
};

class InvoPass2Func : public InvoFunc {
public:
    InvoPass2Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=PASS2(val0 val1) -- do nothing on two values, and pass both on"; }

protected:
};

class InvoAckFunc : public InvoFunc {
public:
    InvoAckFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "ack=ACK(val0 [val1 [val2 [val3 ... [valn]]]]) -- generate acknowledge once all values present"; }

protected:
};

class InvoRdyFunc : public InvoFunc {
public:
    InvoRdyFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=RDY(val ack) -- pass value once acknowledge received)"; }

protected:
};

class InvoSubTestFunc : public InvoFunc {
public:
    InvoSubTestFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val,flag=SUB(op0 op1) -- subtract and return value plus negative flag)"; }

protected:
};

class InvoCondFunc : public InvoFunc {
public:
    InvoCondFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=COND(flag op0 op1) -- return first or second value based on flag)"; }

protected:
};

class InvoAdd1Func : public InvoFunc {
public:
    InvoAdd1Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=ADD1(val) -- add one to value)"; }

protected:
};

class InvoSub1Func : public InvoFunc {
public:
    InvoSub1Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=SUB1(val) -- subtract one from value)"; }

protected:
};

class InvoLsbFunc : public InvoFunc {
public:
    InvoLsbFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val,val=LSB(val) -- return lsb and value left-shifted one)"; }

protected:
};

class InvoLeftShift1Func : public InvoFunc {
public:
    InvoLeftShift1Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=LSH1(val) -- return value left-shifted one)"; }

protected:
};

class InvoRightShift1Func : public InvoFunc {
public:
    InvoRightShift1Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=RSH1(val) -- return value right-shifted one)"; }

protected:
};

class InvoRand1Func : public InvoFunc {
public:
    InvoRand1Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=RAND1(val) -- return a random 0 or 1)"; }

protected:
};

class InvoSinkFunc : public InvoFunc {
public:
    InvoSinkFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "SINK(val0 [val1 [val2 [val3 ... [valn]]]]) -- gobble up arguments and return nothing"; }

protected:
};

class InvoSteerCondFunc : public InvoFunc {
public:
    InvoSteerCondFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "[blank[,blank[,...blank]],]cond[,blank[,blank[,...blank]]]=STRCND(cond) -- steer a condition to the nth output"; }

protected:
};

class InvoSteer0Func : public InvoFunc {
public:
    InvoSteer0Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val,blank[,blank[,..blank]]=STEER0(val) -- unconditionally steer toward the zeroth output (for initialization)"; }

protected:
};

class InvoSteerValueFunc : public InvoFunc {
public:
    InvoSteerValueFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "a0,[a1,[a2...,an-1]]|b0,[b1,[b2...,bn-1]]=STRVAL(steer arg0 [arg1 [arg2 ... argn-1]]) -- steer n arguments by the value of the first argument"; }

protected:
};

class InvoSteerDelimFunc : public InvoFunc {
public:
    InvoSteerDelimFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "delim,blank|blank,val=STRDEL(delim_or_val) -- steer _DELIM_ to the left, all other values to the right"; }

protected:
};

class InvoFourWayFunc : public InvoFunc {
public:
    InvoFourWayFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val0|val1|val2|val3=FOURWAY(val|list) -- steer 4 possible ways based on a value or the first value in a list"; }

protected:
};

class InvoEightWayFunc : public InvoFunc {
public:
    InvoEightWayFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val0|...|val7=EIGHTWAY(val|list) -- steer 8 possible ways based on a value or the first value in a list"; }

protected:
};

class PipeEventQueueFunc : public InvoFunc {
public:
    PipeEventQueueFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=eventqueue([flag] :size :dump) -- set/get the event queue flag (0=disable, 1=enable, 2=paused)"; }

};

class PipeNextEventFunc : public InvoFunc {
public:
    PipeNextEventFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=nextevent() -- do next pipe queue event, return false if empty"; }

    static int _count;
};

class PipeNNextEventFunc : public InvoFunc {
public:
    PipeNNextEventFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "n=nnextevent(:request :notify) -- return number of events in queue"; }

    static int _count;
};

class MergeFunc : public InvoFunc {
public:
    MergeFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "list=MERGE(val [val [val ... [val]]]) -- join paths together"; }

protected:
};

class SteerFunc : public InvoFunc {
public:
    SteerFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "a0,[a1,[a2...,an]]|b0,[b1,[b2...,bn]]=STEER(arg0 [arg1 [arg2 ... argn]]) -- steer n+1 arguments by removed lsb of last argument"; }

protected:
};

class SteerJoinFunc : public InvoFunc {
public:
    SteerJoinFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "{a0,[a1,[a2...,an]]},nil|nil,{b0,[b1,[b2...,bn]]}=STEERJ(arg0 [arg1 [arg2 ... argn]]) -- strip first arg lsb to steer rest into list"; }

protected:
};

class SplitPathsFunc : public InvoFunc {
public:
    SplitPathsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "list=SPLIT(list) -- split paths apart"; }

protected:
};

class JoinPathsFunc : public InvoFunc {
public:
    JoinPathsFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "list=JOIN(val [val [val ... [val]]]) -- join paths together"; }

protected:
};

class InvoTrueFunc : public InvoFunc {
public:
    InvoTrueFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "true=TRUE(val)"; }

protected:
};

class InvoFalseFunc : public InvoFunc {
public:
    InvoFalseFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "false=FALSE(val)"; }

protected:
};

class InvoOneFunc : public InvoFunc {
public:
    InvoOneFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "1=ONE(val)"; }

protected:
};

class InvoZeroFunc : public InvoFunc {
public:
    InvoZeroFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "0=ZERO(val)"; }

protected:
};

class InvoInvertFunc : public InvoFunc {
public:
    InvoInvertFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "val=INVERT(val)"; }

protected:
};

class InvoMeanFunc : public InvoFunc {
public:
    InvoMeanFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "mean=MEAN(val0 [val1 ... valn])"; }

protected:
};

class InvoGt0Func : public InvoFunc {
public:
    InvoGt0Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=GT0(val)"; }

protected:
};


class InvoLt0Func : public InvoFunc {
public:
    InvoLt0Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=LT0(val)"; }

protected:
};


class InvoEq0Func : public InvoFunc {
public:
    InvoEq0Func(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=EQ0(val)"; }

protected:
};

class InvoOddFunc : public InvoFunc {
public:
    InvoOddFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=ODD(val)"; }

protected:
};

class InvoEvenFunc : public InvoFunc {
public:
    InvoEvenFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "flag=EVEN(val)"; }

protected:
};

class InvoTwoFunc : public InvoFunc {
public:
    InvoTwoFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "true=TWO(val)"; }

protected:
};

class InvoThreeFunc : public InvoFunc {
public:
    InvoThreeFunc(ComTerp*);
    virtual void execute();
    virtual const char* docstring() { 
      return "true=THREE(val)"; }

protected:
};

#endif

