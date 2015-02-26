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

#ifndef iplscripts_h
#define iplscripts_h

#include <IplServ/iplcomps.h>
#include <OverlayUnidraw/scriptview.h>
#include <DrawServ/drawcomps.h>
#include <GraphUnidraw/nodecomp.h>
#include <GraphUnidraw/edgecomp.h>

class PipeComp;
class PipeScript : public NodeScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    PipeScript(PipeComp* = nil);
    virtual const char* script_name() { return "pipe"; }
    virtual boolean Definition(ostream&);
    // output variable-length ASCII record that defines the component.
    void Attributes(ostream& out);
    static int ReadTextTransform2(istream&, void*, void*, void*, void*);
    // read transform for 2nd text graphic.
    virtual boolean EmitGS(ostream&, Clipboard*, boolean);
    // specialized output of graphic states.
};

class ConnComp;
class ConnScript : public EdgeScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    ConnScript(ConnComp* = nil);
    virtual const char* script_name() { return "conn"; }
    virtual boolean Definition(ostream&);
    // output serialized view with information to allow restoring connectitivy.
    void IndexNodes(int& start, int& stop, int& startsub, int& stopsub, int& startstrcflag, int& endsrcflag);
    // return index of start and stop nodes.
    int IndexNode (PipeComp *comp, int& subindex, int& srcflag);
    // return index of given node.

};

class ArbiterComp;
class ArbiterScript : public PipeScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    ArbiterScript(ArbiterComp* = nil);
    virtual const char* script_name() { return "arbiter"; }
};

class ForkComp;
class ForkScript : public PipeScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    ForkScript(ForkComp* = nil);
    virtual const char* script_name() { return "fork"; }
};

class InvoComp;
class InvoScript : public PipeScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    InvoScript(InvoComp* = nil);
    virtual const char* script_name() { return "invo"; }
    virtual boolean Definition(ostream&);
    // output variable-length ASCII record that defines the component.
    void Attributes(ostream& out);

    static int ReadTextRotate(istream&, void*, void*, void*, void*);
    // read rotate for text graphics.
    static int ReadNameTransform(istream&, void*, void*, void*, void*);
    // read transform for text graphics.
};

#ifdef HAVE_ACE
class DistantComp;
class DistantScript : public PipeScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    DistantScript(DistantComp* = nil);
    virtual const char* script_name() { return "distant"; }
    virtual boolean Definition(ostream&);
    // output variable-length ASCII record that defines the component.
    void Attributes(ostream& out);
};
#endif

class IplIdrawScript : public DrawIdrawScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    IplIdrawScript(IplIdrawComp* = nil);

    virtual const char* script_name() { return "ipledit"; }
};

#endif
