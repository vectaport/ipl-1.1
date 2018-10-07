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

#ifndef iplcomps_h
#define iplcomps_h

#define TRUE_BLUE_BUBBLES

#include <DrawServ/drawcomps.h>
#include <GraphUnidraw/edgecomp.h>
#include <GraphUnidraw/graphcomp.h>
#include <GraphUnidraw/nodecomp.h>
#include <Unidraw/Components/text.h>
#include <InterViews/transformer.h>

class AttributeValue;
class AttributeValueList;
class ComFunc;
class InvoComp;
class PipeEventQueue;
class PSColor;
class Sprite;

class ConnComp : public EdgeComp {
public:
    ConnComp(ArrowLine*, OverlayComp* parent = nil, int start_subedge = -1, 
	int end_subedge = -1);
    // construct edge component with given ArrowLine graphic, optional
    // 'parent', and optional specification of sub-edges in graphs internal
    // to nodes it gets connected to.
    ConnComp(istream&, OverlayComp* parent = nil);
    // construct edge component from istream, relying on GraphCatalog
    // to re-establish connections between edges and nodes.
    ConnComp(OverlayComp* parent = nil);
    // construct edge component but defer anything graphical
    virtual ~ConnComp();

    virtual EdgeComp* NewEdgeComp(ArrowLine* al, OverlayComp* parent = nil, int start_subedge = -1, 
	int end_subedge = -1)
      { return new ConnComp(al, parent, start_subedge, end_subedge); }
    // virtual constructor for use of derived classes
    virtual EdgeComp* NewEdgeComp(istream& strm, OverlayComp* parent = nil)
      { return new ConnComp(strm, parent); }
    // virtual constructor for use of derived classes
    virtual EdgeComp* NewEdgeComp(OverlayComp* parent = nil)
      { return new ConnComp(parent); }
    // virtual constructor for use of derived classes

    void init();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    
    int handout() { return _handout; }
    void handout(int flag);

    void color();

    virtual void Interpret(Command*);

    int GetStartSubNode() { return _start_subedge; }
    int GetEndSubNode() { return _end_subedge; }

    virtual NodeComp* NodeStart() const;
    // return pointer to start node comp.

    virtual NodeComp* NodeEnd() const;
    // return pointer to end node comp.

    void build_conn_graphic();
    // build up graphic

    void UpdateArrowLine();
    // modify arrow graphic to reflect current state

    virtual int virtual_classid() { return classid(); }
protected:
    int _handout;
    
protected:
    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _conn_params;

    CLASS_SYMID("ConnComp");

};

class PipeComp : public NodeComp {
public:
    PipeComp(SF_Ellipse*, TextGraphic* txt1, TextGraphic* txt2,
	boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with ellipse and text graphic
    // (text display not requiredd by default).
    PipeComp(SF_Ellipse*, TextGraphic*, SF_Ellipse*, GraphComp*, TextGraphic* txt2,
	boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with internal graph
    // (text display not requiredd by default).
    PipeComp(Picture*, boolean reqlabel =false, OverlayComp* parent = nil);
    // construct node component with pre-made Picture graphic.
    PipeComp(GraphComp*);
    // construct node component around a graph.
    PipeComp(istream&, OverlayComp* parent = nil);
    // construct node component from istream, relying on GraphCatalog
    // to re-establish connections between edges and nodes.
    PipeComp(OverlayComp* parent = nil);
    // construct node component but defer anything graphical
    virtual ~PipeComp();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ell, TextGraphic* txt,
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new PipeComp(ell, txt, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(SF_Ellipse* ell1, TextGraphic* txt, SF_Ellipse* ell2, GraphComp* comp, 
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new PipeComp(ell1, txt, ell2, comp, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(Picture* pict, boolean reqlabel =false, OverlayComp* parent = nil)
      { return new PipeComp(pict, reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(GraphComp* comp)
      { return new PipeComp(comp); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(istream& strm, OverlayComp* parent = nil)
      { return new PipeComp(strm, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(OverlayComp* parent = nil)
      { return new PipeComp(parent); }
    // virtual constructor for use of derived classes

    virtual Component* Copy();

    void init(TextGraphic* txt=nil, TextGraphic* txt2=nil);
    TextGraphic* GetText2();
    void SetText2(TextGraphic*);
    void ChangeText2(const char *);
    void MoveText2(float dx, float dy);
    void MoveText2To(float dx, float dy);

    virtual void update_text2();
    boolean text2_update_reserved() 
      { return _text2_update_reserved; }
    boolean reserve_text2_update() 
      { if (!_text2_update_reserved) {_text2_update_reserved = true; return true;} else return false; }
    void unreserve_text2_update() 
      { _text2_update_reserved = false; }
    boolean already_animated() { return _already_animated; }
    void already_animated(int flag) { _already_animated = flag; }

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual void Interpret(Command*);
    // handle move commands, pass rest to base class.
    virtual void Uninterpret(Command*);
    // handle undoing move command, pass rest to base class.

    int buffsize();
    void buffsize(int size);
    AttributeValueList* buff();
    virtual boolean get_buff(int index, AttributeValue*& av);

    virtual boolean src_vacant();
    virtual boolean _src_vacant();
    virtual boolean dst_occupied();
    virtual boolean _dst_occupied();

    virtual boolean src_put(AttributeValue* av);
    virtual boolean dst_get(AttributeValue*& av);
    virtual void _dst_get(AttributeValue*& av);

    virtual void rdst_to_src(ConnComp*, PipeComp*);
    virtual AttributeValue* rdst_cpy_clr(ConnComp*, PipeComp*);
    virtual AttributeValue* rdst_cpy(PipeComp*);
    virtual void rdst_clr_handshake(ConnComp*, PipeComp*);
    virtual void rdst_clr_val(PipeComp*);
    virtual void rdst_chk(PipeComp*);

    virtual void src_request();
    virtual void _src_request();
    virtual int _src_ready();
    virtual void dst_notify();
    virtual void _dst_notify();
    virtual int _dst_ready();

    int handsout() { return _handsout; }
    void handsout(int val) { _handsout = val; }

    boolean onscreen();
    InvoComp* invocomp();

    boolean inconn_exists();
    boolean outconn_exists();

    virtual int update_and_pause();
    virtual void color_outconn(PSColor*);

    virtual int nsrc();
    virtual int ndst();

    PSColor* current_outconn_color() { return _current_outconn_color; }

    static int eventqueueflag() { return _eventqueueflag; }
    static void eventqueueflag(int flag) { _eventqueueflag = flag; }
    static int animateflag() { return _animateflag; }
    static void animateflag(int flag) { _animateflag = flag; }
    static int unitaryflag() { return _unitaryflag; }
    static void unitaryflag(int flag) { _unitaryflag = flag; }
    int src_requested() { return _src_requested; }
    void src_requested(int flag) { _src_requested = flag; }
    int dst_notified() { return _dst_notified; }
    void dst_notified(int flag) { _dst_notified = flag; }

    static boolean _singlestep;
    static boolean _eventqueueflag;
    static int _animateflag;
    static int _unitaryflag;
    static PipeEventQueue* _eventqueue;

    void init_global();
    virtual int virtual_classid() { return classid(); }
    int delay();

    virtual PipeComp* remote_srccomp(int index, int& order, boolean lowlevel=false);
    // remote src comp at the instruction level
    virtual PipeComp* remote_dstcomp(int index, int& order, boolean lowlevel=false);
    // remote dst comp at the instruction level

    virtual ConnComp* srcconn(int index);
    // upstream source connections
    virtual ConnComp* dstconn(int index);
    // downstream destination connections

    int namesym() { return _namesym; }
    // return symbol id (for text in TextGraphic if used with graphics)

    void namesym(int symid) { _namesym = symid; }
    // set symbol id

    Picture* init_picture(); 
    // create new centered Picture graphic using drawing editor state

    void build_pipe_graphic();
    // build up graphic

    int put_cnt() { return _put_cnt; }
    // number of successful srcputs

    float put_rate();
    // cycles per put

    void sprite(Sprite* sprite) { _sprite = sprite; }
    // set sprite object for animation (managed from SpriteQueue)

    Sprite* sprite() { return _sprite; }
    // get sprite object for animation 

    static PSColor* psblue;
    static PSColor* psred;
    static PSColor* psorange;
    static PSColor* psgreen;
    static PSColor* pspurple;

protected:
    int _buffsize;
    AttributeValueList *_buff;
    int _handsout;
    int _text2_update_reserved;
    int _already_animated;
    PSColor* _current_outconn_color;
    int _dst_notified;
    int _src_requested;
    int _delay;
    int _namesym;
    int _put_cnt;
    int* _put_history;
    Sprite* _sprite;

protected:
    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _pipe_params;

    CLASS_SYMID("PipeComp");

};

class ArbiterComp : public PipeComp {
public:
    ArbiterComp(SF_Ellipse*, TextGraphic*, TextGraphic* txt2,
	boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with ellipse and text graphic
    // (text display not requiredd by default).
    ArbiterComp(SF_Ellipse*, TextGraphic*, SF_Ellipse*, GraphComp*, 
	TextGraphic* txt2, boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with internal graph
    // (text display not requiredd by default).
    ArbiterComp(Picture*, boolean reqlabel =false, OverlayComp* parent = nil);
    // construct node component with pre-made Picture graphic.
    ArbiterComp(GraphComp*);
    // construct node component around a graph.
    ArbiterComp(istream&, OverlayComp* parent = nil);
    // construct node component from istream, relying on GraphCatalog
    // to re-establish connections between edges and nodes.
    ArbiterComp(OverlayComp* parent = nil);
    // construct node component but defer anything graphical
    virtual ~ArbiterComp();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ell, TextGraphic* txt,
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new ArbiterComp(ell, txt, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(SF_Ellipse* ell1, TextGraphic* txt, SF_Ellipse* ell2, GraphComp* comp, 
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new ArbiterComp(ell1, txt, ell2, comp, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(Picture* pict, boolean reqlabel =false, OverlayComp* parent = nil)
      { return new ArbiterComp(pict, reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(GraphComp* comp)
      { return new ArbiterComp(comp); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(istream& strm, OverlayComp* parent = nil)
      { return new ArbiterComp(strm, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(OverlayComp* parent = nil)
      { return new ArbiterComp(parent); }
    // virtual constructor for use of derived classes

    void init();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual void src_request();
    virtual void _src_request();
    virtual int _src_ready();
    virtual void rdst_to_src(ConnComp*, PipeComp*);
    virtual void dst_notify();
    virtual void _dst_notify();
    virtual int _dst_ready();

    virtual boolean arbiter_vacant(PipeComp* pipe);

    virtual int virtual_classid() { return classid(); }

    int lastinput() { return _lastinput; }

protected:
    int _lastinput;

protected:
    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _arbiter_params;

    CLASS_SYMID("ArbiterComp");
};

class ForkComp : public PipeComp {
public:
    ForkComp(SF_Ellipse*, TextGraphic*, TextGraphic* txt2,
	boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with ellipse and text graphic
    // (text display not requiredd by default).
    ForkComp(SF_Ellipse*, TextGraphic*, SF_Ellipse*, GraphComp*, 
	TextGraphic* txt2, boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with internal graph
    // (text display not requiredd by default).
    ForkComp(Picture*, boolean reqlabel =false, OverlayComp* parent = nil);
    // construct node component with pre-made Picture graphic.
    ForkComp(GraphComp*);
    // construct node component around a graph.
    ForkComp(istream&, OverlayComp* parent = nil);
    // construct node component from istream, relying on GraphCatalog
    // to re-establish connections between edges and nodes.
    ForkComp(OverlayComp* parent = nil);
    // construct node component but defer anything graphical
    virtual ~ForkComp();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ell, TextGraphic* txt,
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new ForkComp(ell, txt, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(SF_Ellipse* ell1, TextGraphic* txt, SF_Ellipse* ell2, GraphComp* comp, 
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new ForkComp(ell1, txt, ell2, comp, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(Picture* pict, boolean reqlabel =false, OverlayComp* parent = nil)
      { return new ForkComp(pict, reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(GraphComp* comp)
      { return new ForkComp(comp); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(istream& strm, OverlayComp* parent = nil)
      { return new ForkComp(strm, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(OverlayComp* parent = nil)
      { return new ForkComp(parent); }
    // virtual constructor for use of derived classes

    virtual void Interpret(Command*);
    // handle move commands, pass rest to base class.
    virtual void Uninterpret(Command*);
    // handle undoing move command, pass rest to base class.

    void init();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual void rdst_to_src(ConnComp*, PipeComp*);

    virtual void update_text2();
    virtual int update_and_pause();
    virtual void color_outconn(PSColor*);

    static boolean _showforkvals;

    virtual int virtual_classid() { return classid(); }

    virtual void src_request();
    virtual void dst_notify();

    virtual void _src_request();
    virtual void _dst_notify();

#ifdef TRUE_BLUE_BUBBLES
    // experiment with forward reaching src_vacant
    // failed because src_request needs it to go vacant
    virtual boolean src_vacant();
#endif

    void prep();
protected:
    boolean _forking;
    ConnComp* _conn;
    int _distnamesym;
    boolean _prepped;
    boolean _upflag;

    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _fork_params;

    CLASS_SYMID("ForkComp");
};

class PipesComp : public GraphComp {
public:
    PipesComp(OverlayComp* parent = nil);
    PipesComp(istream&, const char* pathname = nil, OverlayComp* parent = nil);
    virtual ~PipesComp();

    void init();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual boolean src_vacant();
    virtual boolean _src_vacant();
    virtual boolean dst_occupied();
    virtual boolean _dst_occupied();

    virtual void src_request();
    virtual void _src_request();
    virtual int _src_ready();
    virtual void dst_notify();
    virtual void _dst_notify();
    virtual int _dst_ready();

    virtual void update_parent_text(PipesComp* child, PipeComp* grandchild);
    
    InvoComp* invocomp();

    boolean complete() { _dst_occupied(); return _complete; }
    boolean exclusive() { return _exclusive; }
    void exclusive(boolean flag) { _exclusive = flag; }

    int numleaf();

    int src_requested() { return _src_requested; }
    void src_requested(int flag) { _src_requested = flag; }
    int dst_notified() { return _dst_notified; }
    void dst_notified(int flag) { _dst_notified = flag; }

    int ackwait() { return _ackwait; }
    void ackwait(int flag) { _ackwait = flag; }

protected:
    boolean _complete;
    boolean _exclusive;
    int _dst_notified;
    int _src_requested;
    int _ackwait; // wait for ack before clearing _complete

    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _pipes_params;

    CLASS_SYMID("PipesComp");
};

class InvoPipesComp : public PipesComp {
public:
    InvoPipesComp(OverlayComp* parent = nil);
    virtual ~InvoPipesComp();

    void init();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    PipesComp* srccomp();
    PipesComp* dstcomp();

    void invocomp(InvoComp* comp) { _invocomp = comp; }
    InvoComp* invocomp() { return _invocomp; }

    PipeComp* find_subpipe(boolean srcflag, int index);

    virtual void update_parent_text(PipesComp* child, PipeComp* grandchild);

    friend ostream& operator << (ostream& s, const PipeComp&);
    // print pipes to ostream.

protected:
    InvoComp* _invocomp;

    CLASS_SYMID("InvoPipesComp");
};

#include <OS/table.h>

declareTable(FuncCountTable,int,int)

class SF_Rect;

class InvoComp : public PipeComp {
public:
    InvoComp(Picture*, boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with pre-made Picture graphic.
    InvoComp(GraphComp*);
    // construct node component around a graph.
    InvoComp(istream&, OverlayComp* parent = nil);
    // construct node component from istream, relying on GraphCatalog
    // to re-establish connections between edges and nodes.
    InvoComp(OverlayComp* parent = nil);
    // construct node component but defer anything graphical
    virtual ~InvoComp();

    virtual NodeComp* NewNodeComp(Picture* pict, boolean reqlabel =false, OverlayComp* parent = nil)
      { return new InvoComp(pict, reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(GraphComp* comp)
      { return new InvoComp(comp); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(istream& strm, OverlayComp* parent = nil)
      { return new InvoComp(strm, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(OverlayComp* parent = nil)
      { return new InvoComp(parent); }
    // virtual constructor for use of derived classes

    virtual Component* Copy();
    InvoComp* CopyOnly();

    void init();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    ComFunc* func() { return _func; }
    void func(ComFunc* ptr);
    void invofunc();

    InvoPipesComp* invopipes() { return (InvoPipesComp*)_graph; }
    void invopipes(InvoPipesComp* pipes) { if (_graph) delete _graph; _graph = pipes; }

    const char* funcname();
    void funcname(const char* name) { if(_funcname) delete _funcname; _funcname = strnew(name); }

    virtual int nsrc();
    void nsrc(int n);
    
    int* srcsizes() { return _srcsizes; }
    int nsrcsizes() { return _nsrcsizes; }
    void srcsizes(int* nsrcs, int nnsrcs);
    
    virtual int ndst();
    void ndst(int n);
    virtual int expected_ndst() { return ndst(); }
    virtual void expected_ndst(int n) { ndst(n); }

    int* dstsizes() { return _dstsizes; }
    int ndstsizes() { return _ndstsizes; }
    void dstsizes(int* dstsizes, int ndstsizes);
    
    void build_invocation();
    void build_pipes();
    void build_invocation_graphic();

    SF_Rect* GetRect();
    TextGraphic* GetFuncText();
    void SetFuncText(TextGraphic*);
    void ChangeFuncText(const char* functext);
    Picture* GetSrcPict();
    Picture* GetDstPict();

    TextGraphic* GetSrcText();
    void SetSrcText(TextGraphic* srctg);
    void ChangeSrcText(const char* srctext);
    TextGraphic* GetDstText();
    void SetDstText(TextGraphic* dsttg);
    void ChangeDstText(const char* dsttext);
    void MoveDstTextTo(float x, float y);

    Picture* GetSpritePict();
    void ChangeSpriteText(int index, const char* text);
    
    void update_srctext(PipeComp* pipe=nil);
    boolean srctext_update_reserved() 
      { return _srctext_update_reserved; }
    boolean reserve_srctext_update() 
      { if (!_srctext_update_reserved) {_srctext_update_reserved = true; return true;} else return false; }
    void unreserve_srctext_update() 
      { _srctext_update_reserved = false; }

    void update_dsttext(PipeComp *pipe=nil);
    boolean dsttext_update_reserved() 
      { return _dsttext_update_reserved; }
    boolean reserve_dsttext_update() 
      { if (!_dsttext_update_reserved) {_dsttext_update_reserved = true; return true;} else return false; }
    void unreserve_dsttext_update() 
      { _dsttext_update_reserved = false; }

    void get_triangle_loc(int srcflag, int pipeindex, int& x, int& y);
    int find_subpipe_index(PipeComp*, int& srcflag);
    PipeComp* find_subpipe(boolean srcflag, int index);


#if defined(COMPLETION_BLOCK)
    void enable_completion_block() { _completion_block = true; }
    void disable_completion_block() { _completion_block = false; }
    boolean completion_block() { return _completion_block; }
#else
    void enable_invofunc_block() { _invofunc_block = true; }
    void disable_invofunc_block() { _invofunc_block = false; }
    boolean invofunc_block() { return _invofunc_block; }
#endif
    int subbuff_count(boolean srcflag);

    virtual void Interpret(Command*);
    // handle move commands, pass rest to base class.
    virtual void Uninterpret(Command*);
    // handle undoing move command, pass rest to base class.

    virtual void color_outconn(PSColor*);
    void update_conns(Editor* ed);

    virtual boolean alustyle(boolean srcflag) { return false; }
    virtual boolean alucomplete(boolean srcflag) { return false; }

    int next_in_line() { return _next_in_line; }
    void next_in_line(int last) { _next_in_line = last; }
    PipesComp* next_complete();

    static void triangle_points(int*& x, int*& y) { x = _trix; y = _triy; }
    static int triangle_height() { return _triy[1]+1; }
    static int triangle_width() { return _trix[2]+1; }

    int width() { return _width; }
    void width(int w) { _width = w; }
    int height() { return _height; }
    void height(int h) { _height = h; }
    Transformer nametrans() { return _nametrans; }
    void nametrans(Transformer trans) { _nametrans = trans; }

    virtual int virtual_classid() { return classid(); }

    virtual PipeComp* remote_srccomp(int index, int& order, boolean lowlevel=0);
    // remote src comp at the instruction level
    virtual PipeComp* remote_dstcomp(int index, int& order, boolean lowlevel=0);
    // remote dst comp at the instruction level

    virtual PipeComp* remote_dstcomp_fanout(int index, int fanout, int& order, boolean lowlevel=0);
    // remote dst comp at the instruction level with support for fanout
    virtual int remote_dstcomps(int index, PipeComp** pipecomps, int* order, 
				int maxpipecomps, boolean lowlevel=0);
    // array of remote dst comps at the instruction level (supports fan-out)

    FuncCountTable* func_count_table() { return _func_count_table; }
    int funcnum() { return _funcnum; }

protected:
    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _invo_params;

    CLASS_SYMID("InvoComp");
    ComFunc* _func;
    const char* _funcname;
    int _funcnum;
    int* _srcsizes;
    int _nsrcsizes;
    int* _dstsizes;
    int _ndstsizes;
#if defined(COMPLETION_BLOCK)
    boolean _completion_block;
#else
    boolean _invofunc_block;
#endif
    boolean _func_block;
    int _next_in_line;
    int _width;
    int _height;
    Transformer _nametrans;
    static int _trix[3];    
    static int _triy[3];

    boolean _srctext_update_reserved;
    boolean _dsttext_update_reserved;

    static FuncCountTable* _func_count_table;
};

class IplIdrawComp : public DrawIdrawComp {
public:
    IplIdrawComp(const char* pathname = nil, OverlayComp* parent = nil);
    IplIdrawComp(istream&, const char* pathname = nil, OverlayComp* parent = nil);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    Component* Copy();

protected:
    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _ipl_idraw_params;

    CLASS_SYMID("IplIdrawComp");
};

#ifdef HAVE_ACE
#include <ace/SOCK_Connector.h>

class ACE_INET_Addr;
class ACE_SOCK_Stream;
class ACE_SOCK_Stream;
#endif
class ComValue;

class DistantComp : public PipeComp {
public:
    DistantComp(SF_Ellipse*, TextGraphic* txt1, TextGraphic* txt2,
	boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with ellipse and text graphic
    // (text display not requiredd by default).
    DistantComp(SF_Ellipse*, TextGraphic*, SF_Ellipse*, GraphComp*, TextGraphic* txt2,
	boolean reqlabel = false, OverlayComp* parent = nil);
    // construct node component with internal graph
    // (text display not requiredd by default).
    DistantComp(Picture*, boolean reqlabel =false, OverlayComp* parent = nil);
    // construct node component with pre-made Picture graphic.
    DistantComp(GraphComp*);
    // construct node component around a graph.
    DistantComp(istream&, OverlayComp* parent = nil);
    // construct node component from istream, relying on GraphCatalog
    // to re-establish connections between edges and nodes.
    DistantComp(OverlayComp* parent = nil);
    // construct node component but defer anything graphical
    virtual ~DistantComp();

    virtual NodeComp* NewNodeComp(SF_Ellipse* ell, TextGraphic* txt,
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new DistantComp(ell, txt, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(SF_Ellipse* ell1, TextGraphic* txt, SF_Ellipse* ell2, GraphComp* comp, 
	boolean reqlabel = false, OverlayComp* parent = nil)
      { return new DistantComp(ell1, txt, ell2, comp, new TextGraphic("", stdgraphic), reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(Picture* pict, boolean reqlabel =false, OverlayComp* parent = nil)
      { return new DistantComp(pict, reqlabel, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(GraphComp* comp)
      { return new DistantComp(comp); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(istream& strm, OverlayComp* parent = nil)
      { return new DistantComp(strm, parent); }
    // virtual constructor for use of derived classes
    virtual NodeComp* NewNodeComp(OverlayComp* parent = nil)
      { return new DistantComp(parent); }
    // virtual constructor for use of derived classes

    void init();
    void prep();
    virtual Component* Copy();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual void _src_request();
    virtual int _src_ready();
    virtual void _dst_notify();
    virtual int _dst_ready();

    int distant_request() { return _distant_request; }
    void distant_request(int flag);
 
    int cycle_ready() { return _cycle_ready; }
    void cycle_ready(int flag) { _cycle_ready = flag; }
 
    int upflag();

    virtual int virtual_classid() { return classid(); }

    static AttributeValueList* distant_list() { return _distant_list; }
    static int cycle();
    static int cycle_count() { return _cycle_count; }
    int ready();
    boolean disabled();

    static int nostats() { return _nostats; }
    static void nostats(int flag) { _nostats = flag; }

protected:
    int _distant_request;
    int _cycle_ready;
#ifdef HAVE_ACE
    ACE_INET_Addr* _addr;
    ACE_SOCK_Stream* _socket;
    ACE_SOCK_Connector* _conn;
#endif
    int _distnamesym;
    boolean _prepped;
    boolean _upflag;
    boolean _disabled;

    static AttributeValueList* _distant_list;
    static int _first_cycle;
    static int _cycle_count;
    static ComValue* _statval;
    static int _nostats;

protected:

    CLASS_SYMID("DistantComp");

};

#define MANUAL_BREAKPOINT
#ifdef MANUAL_BREAKPOINT
extern int _debug0_func;
extern InvoComp* _debug0_comp;
#endif

#define ARIBITER_DEBUG
#endif /* iplcomps_h */
