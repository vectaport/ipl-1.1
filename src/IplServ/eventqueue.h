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

/*
 * PipeEvent and PipeEventQueue
 */

#ifndef eventqueue_h
#define eventqueue_h

#include <OS/enter-scope.h>
#include <InterViews/resource.h>

#ifndef ALITERATOR
#define ALIterator _lib_iv(Iterator)
#define AList _lib_iv(UList)
#endif

class ALIterator;
class AList;
class PipeComp;
class PipesComp;
#include <iosfwd>

class PipeEvent {
 public:
  PipeEvent(PipeComp * pipe, boolean notify_flag, int delay=1);
  PipeEvent(PipesComp * pipes, boolean notify_flag, int delay=1);
  PipeEvent(PipeEvent* event);
  PipeComp* pipe() { return _pipe; }
  PipesComp* pipes() { return _pipes; }
  boolean ispipe() { return _pipe != nil; }
  boolean ispipes() { return _pipes != nil; }
  boolean notify_flag() { return _notify_flag; } 
  int cycle_time() { return _cycletime; }
  void cycle_time(int time) { _cycletime = time; }
  int ready() { return _ready; }
  void ready(int ready) { _ready = ready; }

 protected:
  PipeComp* _pipe;
  PipesComp* _pipes;
  boolean _notify_flag;
  int _cycletime;
  int _ready;
};

#include <ComTerp/comterpserv.h>

//: queue of PipeEvent objects.
// An PipeEventQueue is derived from Resource, so it is a reference-counted
// object that can be freely shared between other objects.
//
// An PipeEventQueue assumes responsibility for the memory of its member
// PipeEvent objects.
class PipeEventQueue : public Resource {
public:
    PipeEventQueue(PipeEventQueue* = nil);
    // construct with optional PipeEventQueue to copy.
    virtual ~PipeEventQueue();
    // do not call directly.  Frees memory of associated PipeEvent objects.

public:
    void First(ALIterator&);
    // set iterator to point to first PipeEvent in list.
    void Last(ALIterator&);
    // set iterator to point to last PipeEvent in list.
    void Next(ALIterator&);
    // set iterator to point to next PipeEvent in list.
    void Prev(ALIterator&);
    // set iterator to point to previous PipeEvent in list.
    boolean Done(ALIterator);
    // return true if iterator is pointing off the end of the list.
    // works for forward and backward traversals.
    boolean IsEmpty();
    // true if no PipeEvent objects in list.
    int Number();
    // number of PipeEvent objects in list.

    void Append(PipeEvent*);
    // append PipeEvent to end of list.
    void Prepend(PipeEvent*);
    // append PipeEvent to front of list.
    void InsertAfter(ALIterator, PipeEvent*);
    // insert PipeEvent after position pointed to by iterator.
    void InsertBefore(ALIterator, PipeEvent*);
    // insert PipeEvent before position pointed to by iterator.
    void Remove(PipeEvent*);
    // remove PipeEvent from list, returning responsibility for freeing the
    // associated memory.
    void Remove(ALIterator&);
    // remove PipeEvent pointed to by iterator from the list, 
    // returning responsibility for freeing the associated memory.
    // This requires saving a pointer to the PipeEvent before calling this method.
    PipeEvent* Replace(ALIterator&, PipeEvent*);
    // remove PipeEvent pointed to by iterator from the list, 
    // returning responsibility for freeing the associated memory.
    // Then insert new PipeEvent in the same place.


    PipeEvent* GetPipeEvent(ALIterator);
    // get PipeEvent pointed to by iterator.
    void SetPipeEvent(PipeEvent*, ALIterator&);
    // set PipeEvent pointed to by iterator.
    boolean Includes(PipeEvent*);
    // check if list includes PipeEvent by pointer-comparison.

    PipeEvent* Get(unsigned int index);
    // retrieve value by index, return nil if not there
    AList* Elem(ALIterator); 
    // return AList (UList) pointed to by ALIterator (Iterator).
    PipeEvent* GetPipeEvent(AList*);
    // return PipeEvent pointed to by AList (UList).

    friend ostream& operator << (ostream& s, const PipeEventQueue&);
    // print list to ostream.

    void clear(); 
    // empty PipeEventQueue, deleting all PipeEvent's.

    void nested_insert(boolean flag) { _nested_insert = flag; }
    // set flag to insert in a nested fashion

    boolean nested_insert() { return _nested_insert; }
    // get flag to insert in a nested fashion

    int tick(boolean dbl=false);
    // execute everything currently in the queue, returning number of events executed

    int tickimpl(int notifyflag);
    // implementation of tick that works for either notifies (1), requests (0) or both (-1)

    int nextevent();
    // execute next event in queue, return 0 if ok, -1 if not

    int do_event(PipeEvent* event, int notifyflag=-1);
    // execute event pointed to by Iterator, return 1 if executed, 0 if not

    int mark_event(PipeEvent* event, int notifyflag=-1);
    // mark event as ready to go, return 1 if ready, 0 if not

    int nnextevent(int notifyflag=-1, int pipeflag=-1);
    // returns count of events in next time batch

    void append(PipeEvent*);
    // append PipeEvent at end of its time batch

    int lasttick() { return _lasttick; }
    // number of events in last tick

    int stats_command(ComTerpServ* comterp, const char* command);
    // set command used to generate stats string

    ComValue stats_eval();
    // evaluate stats command

protected:
    AList* _alist;
    unsigned int _count;
    boolean _nested_insert;
    int _lasttick;

    ComTerpServ* _comterp;
    postfix_token* _statcode;
    int _statcodelen;
};

#endif
