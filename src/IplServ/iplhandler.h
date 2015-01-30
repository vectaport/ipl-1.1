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

#ifndef iplhandler_h
#define iplhandler_h
#include <ComTerp/comhandler.h>

class ComTerp;

class IplservHandler : public ComterpHandler {
public:
    IplservHandler(ComTerpServ* serv=NULL);
    static void AddCommands(ComTerp*);
};

#ifdef HAVE_ACE

// Specialize a IplservAcceptor.
typedef ACE_Acceptor <IplservHandler, ACE_SOCK_ACCEPTOR> 
	IplservAcceptor;

#endif
#endif
