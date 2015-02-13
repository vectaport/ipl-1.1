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
 * IplCatalog - can read and write (serialize/deserialize) components
 */

#ifndef iplcatalog_h
#define iplcatalog_h

#include <DrawServ/drawcatalog.h>

class IplCatalog : public DrawCatalog {
public:
    IplCatalog(const char*, Creator*);

    virtual boolean Retrieve (const char*, Component*&);
    virtual OverlayComp* ReadComp(const char*, istream&, OverlayComp* =nil);
    static IplCatalog* Instance();
    static void Instance(IplCatalog*);

    virtual void graph_init(DrawIdrawComp* comps, int num_edge, int num_node);
    virtual void graph_finish();

protected:
    static IplCatalog* _instance;
    int* _startsubnode;
    int* _endsubnode;
};

#endif
