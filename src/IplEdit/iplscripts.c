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

#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcomps.h>
#include <IplEdit/iplscripts.h>
#include <IplEdit/iplviewer.h>
/*****************************************************************************/

IplIdrawScript::IplIdrawScript (IplIdrawComp* subj) : DrawIdrawScript(subj) 
{
}

ClassId IplIdrawScript::GetClassId () { return IPL_IDRAW_SCRIPT; }

boolean IplIdrawScript::IsA (ClassId id) { 
    return IPL_IDRAW_SCRIPT == id || DrawIdrawScript::IsA(id);
}


