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

#if defined(ARCH_READY)
#include <IplEdit/archviews.h>
#endif
#include <IplEdit/iplabout.h>
#include <IplEdit/iplclasses.h>
#include <IplEdit/iplcmds.h>
#include <IplEdit/iplcreator.h>
#include <IplEdit/iplscripts.h>
#include <IplEdit/iplviews.h>

#if defined(ARCH_READY)
#include <IplServ/archscripts.h>
#endif
#include <IplServ/iplscripts.h>

#include <OverlayUnidraw/ovarrow.h>
#include <OverlayUnidraw/ovpspict.h>
#include <OverlayUnidraw/ovpsview.h>

#include <Unidraw/catalog.h>

/*****************************************************************************/

IplCreator::IplCreator () { }

void* IplCreator::Create (
    ClassId id, istream& in, ObjectMap* objmap, int objid
) {
    switch (id) {
        case IPLABOUT_CMD:         CREATE(IplAboutCmd, in,objmap,objid);

        default:                   return FrameCreator::Create(id, in,objmap,objid);
    }
}

void* IplCreator::Create (ClassId id) {
    if (id == IPL_IDRAW_VIEW)    return new IplIdrawView;
    if (id == IPL_IDRAW_SCRIPT)  return new IplIdrawScript;
    if (id == IPL_IDRAW_PS)      return new OverlayIdrawPS;

    if (id == PIPE_VIEW)    return new PipeView;
    if (id == PIPE_SCRIPT)  return new PipeScript;
    if (id == PIPE_PS)      return new PicturePS;

    if (id == CONN_VIEW)    return new ConnView;
    if (id == CONN_SCRIPT)  return new ConnScript;
    if (id == CONN_PS)      return new ArrowLinePS;

    if (id == ARBITER_VIEW)   return new ArbiterView;
    if (id == ARBITER_SCRIPT) return new ArbiterScript;
    if (id == ARBITER_PS)     return new PicturePS;

    if (id == FORK_VIEW)    return new ForkView;
    if (id == FORK_SCRIPT)  return new ForkScript;
    if (id == FORK_PS)      return new PicturePS;

    if (id == INVO_VIEW)    return new InvoView;
    if (id == INVO_SCRIPT)  return new InvoScript;
    if (id == INVO_PS)      return new PicturePS;

#ifdef HAVE_ACE
    if (id == DISTANT_VIEW)    return new DistantView;
    if (id == DISTANT_SCRIPT)  return new DistantScript;
    if (id == DISTANT_PS)      return new PicturePS;
#endif

#if defined(ARCH_READY)
    if (id == ALU_VIEW)    return new AluView;
    if (id == ALU_SCRIPT)  return new AluScript;
    if (id == ALU_PS)      return new PicturePS;

    if (id == IAD_VIEW)    return new IadView;
    if (id == IAD_SCRIPT)  return new IadScript;
    if (id == IAD_PS)      return new PicturePS;

    if (id == OAD_VIEW)    return new OadView;
    if (id == OAD_SCRIPT)  return new OadScript;
    if (id == OAD_PS)      return new PicturePS;

    if (id == MEM_VIEW)    return new MemView;
    if (id == MEM_SCRIPT)  return new MemScript;
    if (id == MEM_PS)      return new PicturePS;
#endif

    return DrawCreator::Create(id);
}
