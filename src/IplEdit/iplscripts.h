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

#ifndef iplscripts2_h
#define iplscripts2_h

#include <OverlayUnidraw/scriptview.h>
#include <DrawServ/drawcomps.h>
class IplIdrawComp;
class IplIdrawScript : public DrawIdrawScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    IplIdrawScript(IplIdrawComp* = nil);

    virtual const char* script_name() { return "ipledit"; }
};

#endif
