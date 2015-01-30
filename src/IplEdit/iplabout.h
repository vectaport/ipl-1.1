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

#ifndef iplabout_h
#define iplabout_h

#include <Unidraw/Commands/command.h>

class IplAboutDialog;

class IplAboutCmd : public Command {
public:
    IplAboutCmd(ControlInfo*);
    IplAboutCmd(Editor* = nil);

    virtual void Execute();
    virtual boolean Reversible();

    virtual Command* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    void Init();
    IplAboutDialog* aboutdialog;
};

#endif
