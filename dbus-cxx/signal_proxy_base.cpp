/***************************************************************************
 *   Copyright (C) 2009 by Rick L. Vinyard, Jr.                            *
 *   rvinyard@cs.nmsu.edu                                                  *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "signal_proxy_base.h"

namespace DBus
{

  signal_proxy_base::signal_proxy_base( const std::string& path, const std::string& interface, const std::string& name ):
      signal_base( path, interface, name )
  {
  }

  signal_proxy_base::signal_proxy_base( const std::string& interface, const std::string& name ):
      signal_base( interface, name )
  {
  }

  signal_proxy_base::signal_proxy_base( std::shared_ptr<Connection>  connection, const std::string& path, const std::string& interface, const std::string& name ):
      signal_base( connection, path, interface, name )
  {
  }

  signal_proxy_base::signal_proxy_base( std::shared_ptr<Connection>  connection, const std::string& interface, const std::string& name ):
      signal_base( connection, interface, name )
  {
  }

  signal_proxy_base::signal_proxy_base( const signal_proxy_base& other ):
      signal_base( other )
  {
    // TODO connect to the other's connection
  }

  signal_proxy_base::~signal_proxy_base()
  {
  }

  HandlerResult signal_proxy_base::handle_signal( std::shared_ptr<const SignalMessage> msg )
  {
    if ( not this->matches( msg ) ) return HandlerResult::NOT_HANDLED;

    return m_signal_dbus_incoming.emit( msg );
  }

  sigc::signal< HandlerResult(std::shared_ptr<const SignalMessage>)>::accumulated< MessageHandlerAccumulator > signal_proxy_base::signal_dbus_incoming()
  {
    return m_signal_dbus_incoming;
  }

  const std::string & signal_proxy_base::match_rule()
  {
    if ( m_interface.empty() or m_name.empty() ) {
      m_match_rule = std::string();
      return m_match_rule;
    }
    m_match_rule = "type='signal'";
    m_match_rule += ",interface='"   + m_interface   + "'";
    m_match_rule += ",member='"      + m_name      + "'";
    if ( not m_sender.empty() )      m_match_rule += ",sender='"      + m_sender      + "'";
    if ( not m_path.empty() )        m_match_rule += ",path='"        + m_path        + "'";
    if ( not m_destination.empty() ) m_match_rule += ",destination='" + m_destination + "'";
    return m_match_rule;
  }

  bool signal_proxy_base::matches( std::shared_ptr<const Message> msg )
  {
    if ( not msg or not msg->is_valid() ) return false;
    if ( msg->type() != MessageType::SIGNAL ) return false;
    if ( m_interface.empty() or m_name.empty() ) return false;

    std::shared_ptr<const SignalMessage> smsg;
    smsg = std::dynamic_pointer_cast<const SignalMessage>( msg );

    if ( not smsg ) smsg = SignalMessage::create( msg );

    if ( m_interface != smsg->interface() ) return false;

    if ( m_name != smsg->member() ) return false;

    if ( not m_sender.empty() and m_sender != smsg->sender() ) return false;

    if ( not m_destination.empty() and m_destination != smsg->destination() ) return false;

    if ( not m_path.empty() and m_path != smsg->path() ) return false;

    return true;
  }

  signal_proxy_simple::signal_proxy_simple( const std::string& path, const std::string& interface, const std::string& name ):
      signal_proxy_base( path, interface, name )
  {
  }

  signal_proxy_simple::signal_proxy_simple( const std::string& interface, const std::string& name ):
      signal_proxy_base( interface, name )
  {
  }

  signal_proxy_simple::signal_proxy_simple( std::shared_ptr<Connection>  connection, const std::string& path, const std::string& interface, const std::string& name ):
      signal_proxy_base( connection, path, interface, name )
  {
  }

  signal_proxy_simple::signal_proxy_simple( std::shared_ptr<Connection>  connection, const std::string& interface, const std::string& name ):
      signal_proxy_base( connection, interface, name )
  {
  }

  signal_proxy_simple::signal_proxy_simple( const signal_proxy_simple& other ):
      signal_proxy_base( other )
  {
    // TODO connect to the other's connection
  }

  std::shared_ptr<signal_proxy_simple> signal_proxy_simple::create( const std::string & path, const std::string & interface, const std::string & name )
  {
    return std::shared_ptr<signal_proxy_simple>( new signal_proxy_simple( path, interface, name ) );
  }

  std::shared_ptr<signal_proxy_simple> signal_proxy_simple::create( const std::string & interface, const std::string & name )
  {
    return std::shared_ptr<signal_proxy_simple>( new signal_proxy_simple( interface, name ) );
  }

  std::shared_ptr<signal_proxy_simple> signal_proxy_simple::create( std::shared_ptr< Connection > connection, const std::string & path, const std::string & interface, const std::string & name )
  {
    return std::shared_ptr<signal_proxy_simple>( new signal_proxy_simple( connection, path, interface, name ) );
  }

  std::shared_ptr<signal_proxy_simple> signal_proxy_simple::create( std::shared_ptr< Connection > connection, const std::string & interface, const std::string & name )
  {
    return std::shared_ptr<signal_proxy_simple>( new signal_proxy_simple( connection, interface, name ) );
  }

  std::shared_ptr<signal_proxy_simple> signal_proxy_simple::create( const signal_proxy_simple & other )
  {
    return std::shared_ptr<signal_proxy_simple>( new signal_proxy_simple( other ) );
  }

  signal_proxy_simple::~signal_proxy_simple()
  {
  }

  std::shared_ptr<signal_base> signal_proxy_simple::clone()
  {
    return std::shared_ptr<signal_base>( new signal_proxy_simple( *this ) );
  }
  
}

