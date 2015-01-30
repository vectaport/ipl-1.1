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

#ifndef ipleditor_h
#define ipleditor_h

#include <DrawServ/draweditor.h>
#include <IplEdit/iplkit.h>

class IplEditor : public DrawEditor {
public:
    IplEditor(OverlayComp* = nil, OverlayKit* = IplKit::Instance());
    IplEditor(boolean initflag, OverlayKit* = IplKit::Instance());
    IplEditor(const char* file, OverlayKit* = IplKit::Instance());
    void PreInit();
    void Init(OverlayComp* = nil, const char* = "IplEditor");
    virtual void AddCommands(ComTerp*);
protected:
    friend class IplKit;
};

#endif
