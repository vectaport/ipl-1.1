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

#include <OverlayUnidraw/ovclasses.h>

#include <IplServ/iplclasses.h>
#include <IplServ/iplcomps.h>
#include <IplServ/iplscripts.h>
#include <IplServ/iplcreator.h>
#include <IplServ/iplscripts.h>

#include <OverlayUnidraw/ovarrow.h>
#include <OverlayUnidraw/ovpspict.h>
#include <OverlayUnidraw/ovpsview.h>

#include <Unidraw/catalog.h>

/*****************************************************************************/

IplServCreator::IplServCreator () { }

void* IplServCreator::Create (ClassId id) {
    if (id == IPL_IDRAW_SCRIPT)  return new IplIdrawScript;

    if (id == PIPE_SCRIPT)  return new PipeScript;

    if (id == CONN_SCRIPT)  return new ConnScript;

    if (id == ARBITER_SCRIPT) return new ArbiterScript;

    if (id == FORK_SCRIPT)  return new ForkScript;

    if (id == INVO_SCRIPT)  return new InvoScript;

#ifdef HAVE_ACE
    if (id == DISTANT_SCRIPT)  return new DistantScript;
#endif

    return DrawCreator::Create(id);
}
