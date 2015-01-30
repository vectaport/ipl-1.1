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
 * IplKit - object to construct objects for an IplEditor
 */

#ifndef ipl_kit_h
#define ipl_kit_h

#include <DrawServ/drawkit.h>

#include <InterViews/_enter.h>

class IplKit : public DrawKit {
public:
    IplKit();
    virtual ~IplKit();

    virtual MenuItem *MakeFileMenu();
    virtual MenuItem *MakeEditMenu();
    virtual MenuItem* MakeAlignMenu();
    virtual MenuItem* MakeFrameMenu();
    virtual MenuItem* MakeViewersMenu();
    virtual Glyph* MakeToolbar();
    static IplKit* Instance();

    virtual Glyph* appicon();
    // glyph to use for application icon.
protected:
    static IplKit* _iplkit;
};

#include <InterViews/_leave.h>

#endif
