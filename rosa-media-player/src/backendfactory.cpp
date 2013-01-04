/*  ROSA Media Player
    Julia Mineeva, Evgeniy Augin, Sergey Borovkov. Copyright (c) 2011-2012 ROSA  <support@rosalab.ru>
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "backendfactory.h"

#include <QObject>

#include "mplayerprocess.h"
#ifdef MIDI_SUPPORT
#include "wildmidibackend.h"
#endif

BackendBase *BackendFactory::createBackend( BackendType type, QObject *parent )
{
    BackendBase* backend = 0;

    switch ( type )
    {
    case Mplayer: backend = new MplayerProcess( parent ); break;
#ifdef MIDI_SUPPORT
    case WildMidi:  backend = new WildMidiBackend( parent ); break;
#endif
    }

    return backend;
}

void BackendFactory::deleteBackend( BackendBase *backend )
{
    if ( backend ) {
        delete backend;
    }
}
