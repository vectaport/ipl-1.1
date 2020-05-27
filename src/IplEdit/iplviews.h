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

#ifndef iplviews_h
#define iplviews_h

#include <IplEdit/iplcomps.h>
#include <IplServ/iplcomps.h>
#include <DrawServ/drawviews.h>
#include <GraphUnidraw/nodecomp.h>
#include <InterViews/transformer.h>

class FullGraphic;
class IplEditor;

class IplIdrawView : public DrawIdrawView {
public:
    IplIdrawView(IplIdrawComp* = nil);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    static void SetIplEditor(IplEditor* ed) { _ed = ed; }
    static IplEditor* GetIplEditor() {return _ed;}
protected:
    static IplEditor* _ed;
};

//: graphical view of PipeComp.
class PipeView : public NodeView {
public:
    PipeView(PipeComp* = nil);
    virtual ~PipeView();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ellipse, TextGraphic* txt, boolean reqlabel = false)
    { return new PipeComp(ellipse, txt, new TextGraphic("", stdgraphic), reqlabel); }
    // virtual function to allow construction of specialized NodeComp's by specialized NodeView's

    virtual Manipulator* CreateManipulator(Viewer*,Event&,Transformer*,Tool*);
    // create tool-specific manipulator for creating, moving, or reshaping the node.
    virtual Command* InterpretManipulator(Manipulator*);
    // interpret tool-specific manipulator for creating, moving, or reshaping 

    virtual void Update();
    // update view based on any changes to component.
    TextGraphic* GetText2();
    // return pointer to view's 2nd text graphic.

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual void AdjustForZoom(float factor, Coord cx, Coord cy);
    // called once per OverlayView before each zoom, to let
    // fixed size graphics adjust accordingly.
    static float ZoomAdjustment(float factor=1.0);
    // current zoom adjustment for current magnification
protected:
    float _textscale;
    
};

//: graphical view of ConnComp.
class ConnView : public EdgeView {
public:
    ConnView(ConnComp* = nil);
    virtual ~ConnView();

    virtual EdgeComp* NewEdgeComp(ArrowLine* al, OverlayComp* parent = nil, int start_subedge = -1, int end_subedge = -1)
    { return new ConnComp(al, parent, start_subedge, end_subedge); }
    // virtual function to allow construction of specialized EdgeComp's by specialized EdgeView's
    virtual Command* InterpretManipulator(Manipulator*);
    // interpret tool-specific manipulator for creating, moving, or reshaping 
    // the node.
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual Graphic* HighlightGraphic();
    // highlight edge graphic with black brush of width 2.
protected:
    static FullGraphic* _cv_gs;
};

//: graphical view of ArbiterComp.
class ArbiterView : public PipeView {
public:
    ArbiterView(ArbiterComp* = nil);
    virtual ~ArbiterView();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ellipse, TextGraphic* txt, boolean reqlabel = false)
    { return new ArbiterComp(ellipse, txt, new TextGraphic("", stdgraphic), reqlabel); }
    // virtual function to allow construction of specialized NodeComp's by specialized NodeView's

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

protected:
};

//: graphical view of ForkComp.
class ForkView : public PipeView {
public:
    ForkView(ForkComp* = nil);
    virtual ~ForkView();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ellipse, TextGraphic* txt, boolean reqlabel = false)
    { return new ForkComp(ellipse, txt, new TextGraphic("", stdgraphic), reqlabel); }
    // virtual function to allow construction of specialized NodeComp's by specialized NodeView's

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

protected:
};

class SF_Polygon;
//: graphical view of InvoComp.
class InvoView : public PipeView {
public:
    InvoView(InvoComp* = nil);
    virtual ~InvoView();

    virtual Manipulator* CreateManipulator(Viewer*,Event&,Transformer*,Tool*);
    // create tool-specific manipulator for creating, moving, or reshaping the node.
    virtual Command* InterpretManipulator(Manipulator*);
    // interpret tool-specific manipulator for creating, moving, or reshaping 
    // the node.
    virtual int subpipeindex(Picture*, SF_Polygon*);
    // return index of input or output triangle

    Picture* srcgraphics();
    Picture* dstgraphics();

    virtual void Update();
    // update view based on any changes to component.

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    SF_Rect* GetRect();
    TextGraphic* GetFuncText();
    TextGraphic* GetSrcText();
    TextGraphic* GetDstText();
    Picture* GetSrcPict();
    Picture* GetDstPict();
    Picture* GetSpritePict();

    virtual Rubberband* MakeRubberband(IntCoord x, IntCoord y);
    // make Rubberband specific to this node.
protected:

};

#ifdef HAVE_ACE
//: graphical view of DistantComp.
class DistantView : public PipeView {
public:
    DistantView(DistantComp* = nil);
    virtual ~DistantView();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ellipse, TextGraphic* txt, boolean reqlabel = false)
    { return new DistantComp(ellipse, txt, new TextGraphic("", stdgraphic), reqlabel); }
    // virtual function to allow construction of specialized NodeComp's by specialized NodeView's

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

protected:
    
};
#endif

#endif
