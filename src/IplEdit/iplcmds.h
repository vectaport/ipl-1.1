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

#ifndef iplcmds_h
#define iplcmds_h

#include <DrawServ/drawcmds.h>
#include <GraphUnidraw/graphcmds.h>

//: specialized GraphDupCmd for ipledit
class IplDupCmd : public GraphDupCmd {
public:
    IplDupCmd(ControlInfo*, Clipboard* = nil);
    IplDupCmd(Editor* = nil, Clipboard* = nil);
    virtual ~IplDupCmd();

    virtual void Execute();

    virtual Command* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
};

//: specialized GraphPasteCmd for ipledit
class IplPasteCmd : public GraphPasteCmd {
public:
    IplPasteCmd(ControlInfo*, Clipboard* = nil);
    IplPasteCmd(Editor* = nil, Clipboard* = nil);
    virtual ~IplPasteCmd();

    virtual void Execute();

    virtual Command* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
};

//: SquareCmd straighens out connections
class SquareCmd : public Command {
public:
    SquareCmd(ControlInfo*);
    SquareCmd(Editor* = nil);

    virtual Command* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
};

//: ReorientTextCmd straighens out connections
class ReorientTextCmd : public Command {
public:
    ReorientTextCmd(ControlInfo*);
    ReorientTextCmd(Editor* = nil);

    virtual Command* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    virtual boolean Reversible() { return false; }
};

//: command to copy contents of current frame to create a new frame and reconnect graph.
class CopyMoveIplFrameCmd : public CopyMoveGraphFrameCmd {
public:
    CopyMoveIplFrameCmd(ControlInfo*, boolean after = true);
    CopyMoveIplFrameCmd(Editor* = nil, boolean after = true);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    virtual Command* Copy();
    virtual void Execute();
protected:
};

#endif
