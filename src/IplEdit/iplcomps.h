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

#ifndef iplcomps2_h
#define iplcomps2_h

#if 1
#include <IplServ/iplcomps.h>
#else   // moved to IplEdit/iplcomps.h
#include <OverlayUnidraw/ovcomps.h>
#include <DrawServ/drawcomps.h>

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
#endif
#endif
