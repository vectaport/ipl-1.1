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
 * Implementation of PipeEventQueue class.
 */

#include <IplServ/iplcomps.h>
#include <IplServ/spritequeue.h>

#include <Attribute/alist.h>
#include <Attribute/aliterator.h>
#include <IplServ/eventqueue.h>

#include <ComTerp/comterpserv.h>
#include <ComTerp/comvalue.h>

#include <Unidraw/globals.h>
#include <Unidraw/iterator.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/Graphic/ellipses.h>

#include <iostream.h>
#include <string.h>

#include <IV-2_6/_enter.h>

/*****************************************************************************/

PipeEvent::PipeEvent(PipeComp * pipe, boolean notify_flag, int delay) { 
  _pipe = pipe; 
  _pipes = nil; 
  _notify_flag = notify_flag;  
  int compdelay = pipe->delay();
  if (compdelay>0 && delay==1) delay = compdelay;
  _cycletime = DistantComp::cycle_count()+delay;
  _ready = 0;
}

PipeEvent::PipeEvent(PipesComp * pipes, boolean notify_flag, int delay) { 
  _pipe = nil; 
  _pipes = pipes; 
  _notify_flag = notify_flag; 
  _cycletime = DistantComp::cycle_count()+delay;
  _ready = 0;
}

PipeEvent::PipeEvent(PipeEvent* event) { 
  _pipe = event->_pipe; 
  _pipes = event->_pipes; 
  _notify_flag = event->_notify_flag; 
  _cycletime = event->_cycletime;
  _ready = event->_ready;
}

/*****************************************************************************/

PipeEventQueue::PipeEventQueue (PipeEventQueue* s) {
    _alist = new AList;
    _count = 0;
    if (s != nil) {
        ALIterator i;

        for (s->First(i); !s->Done(i); s->Next(i)) {
	    Append(new PipeEvent(s->GetPipeEvent(i)));
	}
    }
    _nested_insert = false;
    _lasttick = 0;
    _comterp = nil;
    _statcode = nil;
    _statcodelen = 0;
}

PipeEventQueue::~PipeEventQueue () { 
    if (_alist) {
        ALIterator i;
	for (First(i); !Done(i); Next(i)) {
	    delete GetPipeEvent(i);
	}
	delete _alist; 
    }
}

PipeEvent* PipeEventQueue::GetPipeEvent (AList* r) {
    return (PipeEvent*) (*r)();
}

AList* PipeEventQueue::Elem (ALIterator i) { return (AList*) i.GetValue(); }

void PipeEventQueue::Append (PipeEvent* v) {
    _alist->Append(new AList(v));
    ++_count;
}

void PipeEventQueue::Prepend (PipeEvent* v) {
    _alist->Prepend(new AList(v));
    ++_count;
}

void PipeEventQueue::InsertAfter (ALIterator i, PipeEvent* v) {
    Elem(i)->Prepend(new AList(v));
    ++_count;
}

void PipeEventQueue::InsertBefore (ALIterator i, PipeEvent* v) {
    Elem(i)->Append(new AList(v));
    ++_count;
}

void PipeEventQueue::Remove (ALIterator& i) {
    AList* doomed = Elem(i);

    Next(i);
    _alist->Remove(doomed);
    delete doomed;
    --_count;
}	
    
void PipeEventQueue::Remove (PipeEvent* p) {
    AList* temp;

    if ((temp = _alist->Find(p)) != nil) {
	_alist->Remove(temp);
        delete temp;
	--_count;
    }
}

PipeEvent* PipeEventQueue::GetPipeEvent (ALIterator i) { return GetPipeEvent
(Elem(i)); }

void PipeEventQueue::SetPipeEvent (PipeEvent* gv, ALIterator& i) {
    i.SetValue(_alist->Find(gv));
}

void PipeEventQueue::First (ALIterator& i) { i.SetValue(_alist->First()); }
void PipeEventQueue::Last (ALIterator& i) { i.SetValue(_alist->Last()); }
void PipeEventQueue::Next (ALIterator& i) { i.SetValue(Elem(i)->Next()); }
void PipeEventQueue::Prev (ALIterator& i) { i.SetValue(Elem(i)->Prev()); }
boolean PipeEventQueue::Done (ALIterator i) { return Elem(i) == _alist->End(); }
int PipeEventQueue::Number () { return _count; }

boolean PipeEventQueue::Includes (PipeEvent* e) {
    return _alist->Find(e) != nil;
}

boolean PipeEventQueue::IsEmpty () { return _alist->IsEmpty(); }

ostream& operator<< (ostream& out, const PipeEventQueue& al) {
    PipeEventQueue* eventlist = (PipeEventQueue*)&al;
    if (!eventlist) return out;

    ALIterator i;
    int j=0;
    for (eventlist->First(i); !eventlist->Done(i);) {
	PipeEvent* event = eventlist->GetPipeEvent(i);
	out << j++ << " ";
	out << event->notify_flag() << " ";
	out << event->pipe() << " ";
	out << event->pipes() << "\n";
	eventlist->Next(i);
    }
    return out;
}


void PipeEventQueue::clear() {
  ALIterator it;
  for( First(it); !Done(it); ) {
    PipeEvent* av = GetPipeEvent(it);
    Remove(it);
    delete av;
  }
}

PipeEvent* PipeEventQueue::Get(unsigned int index) {
  if (Number()<=index) return nil;
  Iterator it;
  First(it);
  for (int i=0; i<index; i++) Next(it);
  return GetPipeEvent(it);
}

PipeEvent* PipeEventQueue::Replace (ALIterator& i, PipeEvent* av) {
    AList* doomed = Elem(i);
    PipeEvent* removed = GetPipeEvent(i);
    Next(i);
    _alist->Remove(doomed);
    delete doomed;
    Elem(i)->Append(new AList(av));
    return removed;
}	
    
int PipeEventQueue::tick(boolean dbl) {
  int nevents = 0;
  if(dbl) {
    nevents += tickimpl(0);
    nevents += tickimpl(1);
  } else
    nevents = tickimpl(-1);
  Iterator it;
  First(it);
  int cnt=0;
  while(cnt<nevents) {
    PipeEvent* event = GetPipeEvent(it);
    if (event->ready()) {
      cnt++;
      Remove(it);
      delete event;
    } else
      Next(it);
  }
  _lasttick = nevents;
  return nevents;
}

int PipeEventQueue::tickimpl(int notifyflag) {
  int nevents = nnextevent();
  int cnt = 0;

  /* mark and count all events ready to go */
  Iterator it;
  First(it);
#ifdef TRUE_BLUE_BUBBLES
  for(int i =0; i<nevents; i++) {
    PipeEvent* event = GetPipeEvent(it);
    cnt += mark_event(event, notifyflag);
    Next(it);
  }
#endif

  /* do all events ready to go */
  First(it);
  for(int i =0; i<nevents; i++) {
    PipeEvent* event = GetPipeEvent(it);
#ifdef TRUE_BLUE_BUBBLES
    if (event->ready()) do_event(event, notifyflag);
#else
    event->ready(true);
    cnt += do_event(event, notifyflag);
#endif
    Next(it);
  }
  
  int numsteps = 100;
  if (PipeComp::animateflag()) {
    SpriteQueue* spriteq = SpriteQueue::instance();
    spriteq->run(numsteps);
    spriteq->clear();
  }
  
  return cnt;
}

int PipeEventQueue::nnextevent(int notifyflag, int pipeflag) {
  Iterator it;
  First(it);
  if (Done(it)) return 0;

  PipeEvent* event = GetPipeEvent(it);
  int thistime = DistantComp::cycle_count() ? DistantComp::cycle_count() : event->cycle_time();
  int count = 0;

  // the time check is critical when using background hum
  // because after the time is incremented new values can arrive
  // before all necessary ready messages arrive, and these
  // really need to be handled in the next increment of time
  // so it is normal for the queue to have a few extra pieces of
  // future work at the end.

  while(!Done(it) && event->cycle_time()<=thistime) {
    if ((notifyflag==-1 || event->notify_flag()==notifyflag) && 
	(pipeflag==-1 || event->ispipe()==pipeflag)) count++;
    Next(it);
    event = GetPipeEvent(it);
  }
  
  return count;
}

int PipeEventQueue::nextevent() {
  Iterator it;
  First(it);
  if(do_event(GetPipeEvent(it))) {
    PipeEvent* event = GetPipeEvent(it);
    Remove(it);
    delete event;
    return 1;
  }
  return 0;
}

int PipeEventQueue::mark_event(PipeEvent* event, int notifyflag) {
  int ready = 0;
  if(notifyflag==-1 || event->notify_flag()==notifyflag) 
    if (notifyflag && event->notify_flag()) {
      if (event->ispipe()) 
	ready = event->pipe()->_dst_ready() /*&& event->pipe()->_dst_occupied()*/;
      else
	ready = event->pipes()->_dst_ready() /*&& event->pipes()->_dst_occupied()*/;
    }
    else if (notifyflag!=1 && !event->notify_flag()) {
      if (event->ispipe()) 
	ready = event->pipe()->sprite() ? 0 : 1;
      else
	ready = 1;
    }
  event->ready(ready);
  return ready;
}


int PipeEventQueue::do_event(PipeEvent* event, int notifyflag) {
  if(notifyflag==-1 || event->notify_flag()==notifyflag) 
    if (notifyflag && event->notify_flag()) {
      if (event->ispipe()) {
	event->pipe()->dst_notified(0);
	event->pipe()->_dst_notify();
      } else {
	event->pipes()->dst_notified(0);
	event->pipes()->_dst_notify();
      }
      return 1;
    }
    else if (notifyflag!=1 && !event->notify_flag()) {
      if (event->ispipe()) {
	event->pipe()->_src_request();
	event->pipe()->src_requested(0);
      } else {
	event->pipes()->_src_request();
	event->pipes()->src_requested(0);
      }
      return 1;
    }
  return 0;
}


void PipeEventQueue::append(PipeEvent* event) {

  Iterator it;
  Last(it);
  int etime = event->cycle_time();
  while(!Done(it) && etime<GetPipeEvent(it)->cycle_time())
    Next(it);
  InsertAfter(it, event);
}

int PipeEventQueue::stats_command(ComTerpServ* comterp, const char* command) {
  _comterp = comterp;
  if(_statcode) delete _statcode;
  _statcode = _comterp->gen_code(command, _statcodelen);
  return _statcode!=nil ? 0 : -1;
}
  
ComValue PipeEventQueue::stats_eval() {
  if (_statcode)
    return _comterp->run(_statcode, _statcodelen);
  else
    return ComValue::nullval();
}
