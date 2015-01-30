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
 * IplImportCmd - a command for importing graphs
 */

#ifndef ipl_import_h
#define ipl_import_h

#include <DrawServ/drawimport.h>

#include <stdio.h>

//: specialized DrawImportCmd for importing sub-graphs
class IplImportCmd : public DrawImportCmd {
public:
    IplImportCmd(ControlInfo*, ImportChooser* = nil);
    IplImportCmd(Editor* = nil, ImportChooser* = nil);

    virtual void Execute();
    virtual Command* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual GraphicComp* Import(const char*);
};

#endif
