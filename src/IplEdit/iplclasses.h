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

#ifndef iplclasses2_h
#define iplclasses2_h

#include <DrawServ/drawclasses.h>
#include <IplServ/iplclasses.h>

/* 
 * Unique IplEdit class identifiers.
 */

#define IPLABOUT_CMD       9950
#define IPLDUP_CMD         9951
#define IPLIMPORT_CMD      9952
#define IPLPASTE_CMD       9953
#define COPYMOVEIPLFRAME_CMD       9954

#define IPL_IDRAW_VIEW     Combine(IPL_IDRAW_COMP, COMPONENT_VIEW)
#define IPL_IDRAW_PS       Combine(IPL_IDRAW_COMP, POSTSCRIPT_VIEW)

#define PIPE_VIEW          Combine(PIPE_COMP, COMPONENT_VIEW)
#define PIPE_PS            Combine(PIPE_COMP, POSTSCRIPT_VIEW)

#define CONN_VIEW          Combine(CONN_COMP, COMPONENT_VIEW)
#define CONN_PS            Combine(CONN_COMP, POSTSCRIPT_VIEW)

#define ARBITER_VIEW       Combine(ARBITER_COMP, COMPONENT_VIEW)
#define ARBITER_PS         Combine(ARBITER_COMP, POSTSCRIPT_VIEW)

#define FORK_VIEW          Combine(FORK_COMP, COMPONENT_VIEW)
#define FORK_PS            Combine(FORK_COMP, POSTSCRIPT_VIEW)

#define INVO_VIEW          Combine(INVO_COMP, COMPONENT_VIEW)
#define INVO_PS            Combine(INVO_COMP, POSTSCRIPT_VIEW)

#define DISTANT_VIEW       Combine(DISTANT_COMP, COMPONENT_VIEW)
#define DISTANT_PS         Combine(DISTANT_COMP, POSTSCRIPT_VIEW)

#define ALU_VIEW           Combine(ALU_COMP, COMPONENT_VIEW)
#define ALU_PS             Combine(ALU_COMP, POSTSCRIPT_VIEW)

#define IAD_VIEW           Combine(IAD_COMP, COMPONENT_VIEW)
#define IAD_PS             Combine(IAD_COMP, POSTSCRIPT_VIEW)

#define OAD_VIEW           Combine(OAD_COMP, COMPONENT_VIEW)
#define OAD_PS             Combine(OAD_COMP, POSTSCRIPT_VIEW)

#define MEM_VIEW           Combine(MEM_COMP, COMPONENT_VIEW)
#define MEM_PS             Combine(MEM_COMP, POSTSCRIPT_VIEW)

#endif
