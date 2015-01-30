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
#ifdef HAVE_ACE
#include <iostream.h>
#include <fstream.h>
#if __GNUC__==2 && __GNUC_MINOR__<=7
#else
#include <vector.h>
#endif
#endif

#if defined(ARCH_READY)
#include <IplServ/archfunc.h>
#endif
#include <IplServ/delimfunc.h>
#include <IplServ/invofunc.h>
#include <IplServ/iplhandler.h>
#include <IplServ/iplfunc.h>
#include <IplServ/langfunc.h>
#include <ComTerp/comterpserv.h>

IplservHandler::IplservHandler(ComTerpServ* serv) : ComterpHandler(serv) {
  AddCommands(comterp_);
}

void IplservHandler::AddCommands(ComTerp* comterp) {
  comterp->add_command("ipl", new IplFunc(comterp));
  comterp->add_command("anglebracketsflag", new AngleBracketsFlagFunc(comterp));
  comterp->add_command("pipe", new CreatePipeFunc(comterp));
  comterp->add_command("pipes", new CreatePipesFunc(comterp));
  comterp->add_command("invopipes", new CreateInvoPipesFunc(comterp));
  comterp->add_command("invo", new CreateInvoFunc(comterp));
  comterp->add_command("buildinvo", new BuildInvoFunc(comterp));
  comterp->add_command("arbiter", new CreateArbiterFunc(comterp));
  comterp->add_command("conn", new ConnectDstToSrcFunc(comterp));
  comterp->add_command("mconn", new ConnectArbiterFunc(comterp));
  comterp->add_command("buffsize", new BuffSizeFunc(comterp));
  comterp->add_command("buff", new BuffFunc(comterp));
  comterp->add_command("lastinput", new LastInputFunc(comterp));
  comterp->add_command("pipename", new PipeNameFunc(comterp));
  comterp->add_command("neighbors", new NeighborsFunc(comterp));
  comterp->add_command("subsrc", new SubSrcPipeFunc(comterp));
  comterp->add_command("subdst", new SubDstPipeFunc(comterp));
  comterp->add_command("srcconns", new SrcConnsFunc(comterp));
  comterp->add_command("dstconns", new DstConnsFunc(comterp));
  comterp->add_command("nsrc", new NumSrcFunc(comterp));
  comterp->add_command("ndst", new NumDstFunc(comterp));
  comterp->add_command("srcvacant", new SrcVacantFunc(comterp));
  comterp->add_command("dstoccupied", new DstOccupiedFunc(comterp));
  comterp->add_command("srcput", new SrcPutFunc(comterp));
  comterp->add_command("dstget", new DstGetFunc(comterp));
  #ifdef HAVE_ACE
  comterp->add_command("request", new RequestFunc(comterp));
  #endif
  comterp->add_command("pipesadd", new PipesAddFunc(comterp));
  comterp->add_command("ptext2", new PipeText2Func(comterp));
  comterp->add_command("getinvopipes", new GetInvoPipesFunc(comterp));
  comterp->add_command("getsrcpipes", new GetSrcPipesFunc(comterp));
  comterp->add_command("getdstpipes", new GetDstPipesFunc(comterp));
  comterp->add_command("pipescomplete", new PipesCompleteFunc(comterp));
  comterp->add_command("debugcomp", new DebugCompFunc(comterp));
  comterp->add_command("eventqueue", new PipeEventQueueFunc(comterp));
  comterp->add_command("nextevent", new PipeNextEventFunc(comterp));
  comterp->add_command("nnextevent", new PipeNNextEventFunc(comterp));
  comterp->add_command("tick", new TickFunc(comterp));
  comterp->add_command("cycle", new CycleFunc(comterp));
  comterp->add_command("ready", new ReadyFunc(comterp));
  comterp->add_command("cyclecnt", new CycleCountFunc(comterp));
  comterp->add_command("putrate", new PutRateFunc(comterp));
  comterp->add_command("handsout", new HandsoutFunc(comterp));
  comterp->add_command("handout", new HandoutFunc(comterp));
  comterp->add_command("movetext2", new MoveText2Func(comterp));
  comterp->add_command("animate", new PipeAnimateFunc(comterp));
  comterp->add_command("sprites", new SpritesFunc(comterp));
  comterp->add_command("lasttick", new LastTickFunc(comterp));
  comterp->add_command("statscomm", new StatsCommFunc(comterp));
  comterp->add_command("statseval", new StatsEvalFunc(comterp));
  comterp->add_command("unitary", new InvoUnitaryFunc(comterp));

  comterp->add_command("destination", new DestinationFunc(comterp));
  comterp->add_command("destination2", new Destination2Func(comterp));
  comterp->add_command("symspace", new SymSpaceFunc(comterp));
  comterp->add_command("symspace2", new SymSpace2Func(comterp));
  comterp->add_command("destsymspace", new DestSymSpaceFunc(comterp));
  comterp->add_command("destsymspace2", new DestSymSpace2Func(comterp));
  comterp->add_command("separator", new SeparatorFunc(comterp));

  comterp->add_command("()", new ParensFunc(comterp));
  comterp->add_command("[]", new BracketsFunc(comterp));
  comterp->add_command("{}", new BracesFunc(comterp));
  comterp->add_command("<>", new AngleBracketsFunc(comterp));
  comterp->add_command("<<>>", new DoubleAngleBracketsFunc(comterp));

  comterp->add_command("def", new InvoDefFunc(comterp));

  comterp->add_command("GT", new InvoGreaterThanFunc(comterp));
  comterp->add_command("LT", new InvoLessThanFunc(comterp));
  comterp->add_command("PASS", new InvoPassFunc(comterp));
  comterp->add_command("PASS2", new InvoPass2Func(comterp));
  comterp->add_command("ACK", new InvoAckFunc(comterp));
  comterp->add_command("RDY", new InvoRdyFunc(comterp));
  comterp->add_command("SINK", new InvoSinkFunc(comterp));

  comterp->add_command("SUB", new InvoSubTestFunc(comterp));
  comterp->add_command("COND", new InvoCondFunc(comterp));
  comterp->add_command("ADD1", new InvoAdd1Func(comterp));
  comterp->add_command("SUB1", new InvoSub1Func(comterp));
  comterp->add_command("LSB", new InvoLsbFunc(comterp));
  comterp->add_command("LSH1", new InvoLeftShift1Func(comterp));
  comterp->add_command("RSH1", new InvoRightShift1Func(comterp));
  comterp->add_command("RAND1", new InvoRand1Func(comterp));
  comterp->add_command("STRCND", new InvoSteerCondFunc(comterp));
  comterp->add_command("STEER0", new InvoSteer0Func(comterp));
  comterp->add_command("STRVAL", new InvoSteerValueFunc(comterp));
  comterp->add_command("STRDEL", new InvoSteerDelimFunc(comterp));
  comterp->add_command("FOURWAY", new InvoFourWayFunc(comterp));
  comterp->add_command("EIGHTWAY", new InvoEightWayFunc(comterp));
  comterp->add_command("TRUE", new InvoTrueFunc(comterp));
  comterp->add_command("FALSE", new InvoFalseFunc(comterp));
  comterp->add_command("INVERT", new InvoInvertFunc(comterp));
  comterp->add_command("MEAN", new InvoMeanFunc(comterp));
  comterp->add_command("ZERO", new InvoFalseFunc(comterp));
  comterp->add_command("ONE", new InvoTrueFunc(comterp));
  comterp->add_command("TWO", new InvoTwoFunc(comterp));
  comterp->add_command("THREE", new InvoThreeFunc(comterp));
  comterp->add_command("GT0", new InvoGt0Func(comterp));
  comterp->add_command("LT0", new InvoLt0Func(comterp));
  comterp->add_command("EQ0", new InvoEq0Func(comterp));
  comterp->add_command("ODD", new InvoOddFunc(comterp));
  comterp->add_command("EVEN", new InvoEvenFunc(comterp));

  comterp->add_command("MERGE", new MergeFunc(comterp));
  comterp->add_command("SPLIT", new SplitPathsFunc(comterp));
  comterp->add_command("JOIN", new JoinPathsFunc(comterp));
  comterp->add_command("STEER", new SteerFunc(comterp));
  comterp->add_command("STEERJ", new SteerJoinFunc(comterp));
  comterp->add_command("transcribe", new TranscribePipeFunc(comterp));

#if defined(ARCH_READY)
  comterp->add_command("MEM", new MemFunc(comterp));
  comterp->add_command("ALUPASS", new AluPassFunc(comterp));
  comterp->add_command("EITHER", new EitherFunc(comterp));
  comterp->add_command("EITHER4", new EitherFunc(comterp));
  comterp->add_command("ARBITER", new ArbiterFunc(comterp));
  comterp->add_command("ARBIT4", new ArbiterFunc(comterp));
  comterp->add_command("ARBFLAG", new ArbiterFlagFunc(comterp));
  comterp->add_command("NXT", new NextFromPipeFunc(comterp));
  comterp->add_command("TEE", new TeeFunc(comterp));

  comterp->add_command("IAD", new IadFunc(comterp));
  comterp->add_command("OAD", new OadFunc(comterp));
  comterp->add_command("ALU", new AluFunc(comterp));

  comterp->add_command("iadtable", new IadTableFunc(comterp));
  comterp->add_command("oadtable", new OadTableFunc(comterp));
  comterp->add_command("alutable", new AluTableFunc(comterp));
  comterp->add_command("oadmem", new OadMemoryFunc(comterp));

  comterp->add_command("instr", new RemoteInstructionFunc(comterp));
#endif
}

