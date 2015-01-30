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

// moved to IplServ/iplcomps.c

#if 0
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcmds.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/ipleditor.h>
#include <IplServ/iplscripts.h>

#include <OverlayUnidraw/paramlist.h>

#include <Unidraw/unidraw.h>
#include <Unidraw/ulist.h>
#include <Unidraw/viewer.h>

#include <Unidraw/Commands/command.h>

#include <Unidraw/iterator.h>

#include <stdio.h>
#include <stream.h>
#include <string.h>

/****************************************************************************/

ParamList* IplIdrawComp::_ipl_idraw_params = nil;
int IplIdrawComp::_symid = -1;

IplIdrawComp::IplIdrawComp (const char* pathname, OverlayComp* parent) : DrawIdrawComp(pathname, parent) 
{ 
}

ClassId IplIdrawComp::GetClassId () { return IPL_IDRAW_COMP; }

boolean IplIdrawComp::IsA (ClassId id) {
    return IPL_IDRAW_COMP == id || DrawIdrawComp::IsA(id);
}

IplIdrawComp::IplIdrawComp (istream& in, const char* pathname, OverlayComp* parent) : DrawIdrawComp(pathname, parent) {
    _valid = GetParamList()->read_args(in, this);
}

ParamList* IplIdrawComp::GetParamList() {
    if (!_ipl_idraw_params) 
	GrowParamList(_ipl_idraw_params = new ParamList());
    return _ipl_idraw_params;
}

void IplIdrawComp::GrowParamList(ParamList* pl) {
    DrawIdrawComp::GrowParamList(pl); 
}

Component* IplIdrawComp::Copy () {
    return new IplIdrawComp((const char*)nil, nil);
}

#endif
