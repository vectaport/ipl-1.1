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

/*
 * IplCatalog implementation.
 */

#include <IplEdit/iplcatalog.h>
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/iplcreator.h>
#include <IplServ/iplcomps.h>
#include <OverlayUnidraw/ovfile.h>
#include <Unidraw/unidraw.h>
#include <Attribute/paramlist.h>
#include <TopoFace/topoedge.h>
#include <TopoFace/toponode.h>
#include <stdio.h>
#include <stream.h>
#include <ctype.h>
#include <fstream.h>

/*****************************************************************************/

IplCatalog::IplCatalog (
    const char* name, Creator* creator
) : DrawCatalog(name, creator) {
  SetCompactions(true, false, false);
  _startsubnode = _endsubnode = nil;
}

boolean IplCatalog::Retrieve (const char* name, Component*& comp) {
    OverlayCatalog* oldinst = OverlayCatalog::Instance();
    OverlayCatalog::Instance(this);

    if (Valid(name, comp)) {
        _valid = true;

    } else if (UnidrawFormat(name)) {
        _valid = Catalog::Retrieve(name, comp);

    } else {
        filebuf fbuf;
        _valid = fbuf.open(name, ios_base::in) != 0;

        if (_valid) {
	    istream in(&fbuf);
	    const int len = BUFSIZ;
	    char buf[len];

	    char ch;
	    while (isspace(ch = in.get())) {}; in.putback(ch);
	    ParamList::parse_token(in, buf, len);
	    if (strcmp(buf, "ipledit") == 0) { 
	      comp = new IplIdrawComp(in, name);
	      _valid = in.good() && ((OverlayComp*)comp)->valid();
	      if (_import) {
		/* convert into pasteable graph */
		
	      }
	    } else 
		_valid = false;

            if (_valid) {
                Forget(comp, name);
                Register(comp, name);
            }
        }
    }
    OverlayCatalog::Instance(oldinst);
    return _valid;
}

OverlayComp* IplCatalog::ReadComp(const char* name, istream& in, OverlayComp* parent) {
  OverlayComp* child = nil;

  if (strcmp(name, "conn") == 0) {
     child = new ConnComp(in, parent);
     ConnComp* comp = (ConnComp*)child;
     _startnode[_edge_cnt] = comp->GetStartNode();
     _endnode[_edge_cnt] = comp->GetEndNode();
     _startsubnode[_edge_cnt] = comp->GetStartSubNode();
     _endsubnode[_edge_cnt] = comp->GetEndSubNode();
     _edges[_edge_cnt] = comp;
     _edge_cnt++;
  }
  
  else if (strcmp(name, "pipe") == 0) {
     child = new PipeComp(in, parent);
     ((PipeComp*)child)->init_global();
     _nodes[_node_cnt] = (PipeComp*)child;
     _node_cnt++;
  }
  
  else if (strcmp(name, "mutex") == 0 || strcmp(name, "arbiter") == 0) {
     child = new ArbiterComp(in, parent);
     _nodes[_node_cnt] = (ArbiterComp*)child;
     _node_cnt++;
  }
  
  else if (strcmp(name, "fork") == 0) {
     child = new ForkComp(in, parent);
     ((PipeComp*)child)->init_global();
     _nodes[_node_cnt] = (ForkComp*)child;
     _node_cnt++;
  }
  
  else if (strcmp(name, "invo") == 0) {
     child = new InvoComp(in, parent);
     _nodes[_node_cnt] = (InvoComp*)child;
     _node_cnt++;
  }
  
#ifdef HAVE_ACE
  else if (strcmp(name, "distant") == 0) {
     child = new DistantComp(in, parent);
     ((PipeComp*)child)->init_global();
     _nodes[_node_cnt] = (DistantComp*)child;
     _node_cnt++;
  }
#endif
  
  else
     child = OverlayCatalog::ReadComp(name, in, parent);
    
  return child;
}

IplCatalog* IplCatalog::_instance = nil;

IplCatalog* IplCatalog::Instance() {
  if (!_instance) {
    if (Component::use_unidraw()) 
      _instance = (IplCatalog*)unidraw->GetCatalog();
    else
      _instance = new IplCatalog("IplCatalog", new IplCreator());
  }
  return _instance;
}

void IplCatalog::Instance(IplCatalog* catalog) {
  _instance = catalog;
}

void IplCatalog::graph_init(DrawIdrawComp* comps, int num_edge, int num_node) {
  delete _startnode;
  delete _endnode;
  delete _startsubnode;
  delete _endsubnode;
  delete _edges;
  delete _nodes;
  _comps = comps;
  _startnode = new int[num_edge];
  _endnode = new int[num_edge];
  _startsubnode = new int[num_edge];
  _endsubnode = new int[num_edge];
  _edges = new EdgeComp*[num_edge];
  _nodes = new NodeComp*[num_node];
  _num_edge = num_edge;
  _num_node = num_node;
  _edge_cnt = 0;
  _node_cnt = 0;
}

void IplCatalog::graph_finish() {
  for (int i=0; i<_num_edge; i++) {
    int start_id = _startnode[i];
    int end_id = _endnode[i];
    int start_subid = _startsubnode[i];
    int end_subid = _endsubnode[i];

    if (start_id < 0 || end_id < 0)
      _comps->AppendEdge(_edges[i]);

    TopoNode* startnode = nil;
    if (start_subid < 0 )
      startnode = start_id < 0 ? nil : _nodes[start_id]->Node();
    else {
      PipeComp* subpipe = ((InvoComp*)_nodes[start_id])->find_subpipe(false /*dst*/, start_subid);
      if (subpipe) startnode = subpipe->Node();
      else fprintf(stderr, "unexpected trouble finding subpipe\n");
    }

    TopoNode* endnode = nil;
    if (end_subid < 0 )
      endnode = end_id < 0 ? nil : _nodes[end_id]->Node();
    else {
      PipeComp* subpipe = ((InvoComp*)_nodes[end_id])->find_subpipe(true /*src*/, end_subid);
      if (subpipe) endnode = subpipe->Node();
      else fprintf(stderr, "unexpected trouble finding subpipe\n");
    }

    _edges[i]->Edge()->
      attach_nodes(startnode, endnode);

    // Observer/Observable stuff  -- does this get used??
#if defined(GRAPH_OBSERVABLES)
    if (start_id >=0 && end_id >=0) 
      _edges[i]->NodeStart()->attach(_edges[i]->NodeEnd());
#endif
  }
  delete _startnode; _startnode = nil;
  delete _endnode; _endnode = nil;
  delete _startsubnode; _startsubnode = nil;
  delete _endsubnode; _endsubnode = nil;
  delete _edges; _edges = nil;
  delete _nodes; _nodes = nil;
  _comps = nil;
}

