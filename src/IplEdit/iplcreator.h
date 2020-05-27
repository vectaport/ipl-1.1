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

#ifndef ipleditcreator_h
#define ipleditcreator_h

#include <IplServ/iplcreator.h>

class IplEditCreator : public IplServCreator {
 public:
    IplEditCreator();

    virtual void* Create(                       // for subjects
        ClassId, istream&, ObjectMap* = nil, int = 0
    );
    virtual void* Create(ClassId);              // for views
};

#endif
