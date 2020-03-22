/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
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
#include <sigc++/sigc++.h>

#include <string>
#include <map>
#include <mutex>
#include <shared_mutex>

#include <dbus-cxx/signal_proxy_base.h>
#include <dbus-cxx/interfaceproxy.h>

#ifndef DBUSCXX_OBJECTPROXY_H
#define DBUSCXX_OBJECTPROXY_H

namespace DBus
{

  class Connection;

  /**
   * Object proxies are local proxies that provide local methods and signals for
   * remote objects with dbus interfaces.
   *
   * @ingroup proxy
   * @ingroup objects
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class ObjectProxy
  {
    protected:

      /**
       * This class has a protected constructor. Use the \c create() methods
       * to obtain a smart std::shared_ptr<ObjectProxy> to a new instance.
       */
      ObjectProxy( std::shared_ptr<Connection> conn, const std::string& destination, const std::string& path );

    public:
      /**
       * Creates an ObjectProxy with a specific path
       * @param path The path of the object that this will be a proxy for
       */
      static std::shared_ptr<ObjectProxy> create( const std::string& path );

      /**
       * Creates an ObjectProxy with a specific path
       * @param destination The destination address that this will be a proxy for
       * @param path The path of the object that this will be a proxy for
       */
      static std::shared_ptr<ObjectProxy> create( const std::string& destination, const std::string& path );

      static std::shared_ptr<ObjectProxy> create( std::shared_ptr<Connection> conn, const std::string& path );

      static std::shared_ptr<ObjectProxy> create( std::shared_ptr<Connection> conn, const std::string& destination, const std::string& path );

      virtual ~ObjectProxy();

      std::shared_ptr<Connection> connection() const;

      void set_connection( std::shared_ptr<Connection> conn );

      const std::string& destination() const;

      void set_destination( const std::string& destination );

      const Path& path() const;

      void set_path( const std::string& path );

      typedef std::multimap<std::string, std::shared_ptr<InterfaceProxy>> Interfaces;

      const Interfaces& interfaces() const;

      /** Returns the first interface with the given name */
      std::shared_ptr<InterfaceProxy> interface( const std::string& name ) const;

      /** Alias for interface(name) */
      std::shared_ptr<InterfaceProxy> operator[]( const std::string& name ) const;

      /** Adds the interface to this object */
      bool add_interface( std::shared_ptr<InterfaceProxy> interface );

      /**
       * Creates and adds the named interface to this object
       *
       * @return the newly created interface
       */
      std::shared_ptr<InterfaceProxy> create_interface( const std::string& name );

      /** Removes the first interface with the given name */
      void remove_interface( const std::string& name );

      /** Removes the given interface */
      void remove_interface( std::shared_ptr<InterfaceProxy> interface );

      bool has_interface( const std::string& name ) const;

      bool has_interface( std::shared_ptr<InterfaceProxy> interface ) const;

      std::shared_ptr<InterfaceProxy> default_interface() const;

      bool set_default_interface( const std::string& new_default_name );

      bool set_default_interface( std::shared_ptr<InterfaceProxy> new_default );

      void remove_default_interface();

      /** Adds the method to the named interface */
      bool add_method( const std::string& interface, std::shared_ptr<MethodProxyBase> method );

      /** Adds the method to the default interface */
      bool add_method( std::shared_ptr<MethodProxyBase> method );

      std::shared_ptr<CallMessage> create_call_message( const std::string& interface_name, const std::string& method_name ) const;

      std::shared_ptr<CallMessage> create_call_message( const std::string& method_name ) const;

      std::shared_ptr<const ReturnMessage> call( std::shared_ptr<const CallMessage>, int timeout_milliseconds=-1 ) const;

      std::shared_ptr<PendingCall> call_async( std::shared_ptr<const CallMessage>, int timeout_milliseconds=-1 ) const;

      /**
       * Creates a proxy method with a signature based on the template parameters and adds it to the named interface
       * @return A smart pointer to the newly created method proxy
       * @param interface_name The name of the interface to add this proxy method to
       * @param method_name The name to assign to the newly create method proxy. This name will be used to perform the dbus-call.
       */
      template <class T_type>
      std::shared_ptr<MethodProxy<T_type>>
      create_method( const std::string& interface_name, const std::string& method_name )
      {
        std::shared_ptr<InterfaceProxy> interface = this->interface(interface_name);
        if ( not interface ) interface = this->create_interface( interface_name );
        return interface->create_method<T_type>(method_name);
      }

      /**
       * Creates a signal proxy with a signature based on the template parameters and adds it to the named interface
       * @return A smart pointer to the newly created signal proxy
       * @param interface_name The name of the interface to add this proxy signal to
       * @param sig_name The name to assign to the newly created signal proxy.
       */
      template <class... T_type>
      std::shared_ptr<signal_proxy<T_type...> >
      create_signal( const std::string& interface_name, const std::string& sig_name )
      {
        std::shared_ptr<InterfaceProxy> interface = this->interface(interface_name);
        if ( not interface ) interface = this->create_interface( interface_name );
        return interface->create_signal<T_type...>(sig_name);
      }

      sigc::signal<void(std::shared_ptr<InterfaceProxy>)> signal_interface_added();

      sigc::signal<void(std::shared_ptr<InterfaceProxy>)> signal_interface_removed();

      sigc::signal<void(std::shared_ptr<InterfaceProxy>/*old default*/,std::shared_ptr<InterfaceProxy>/*new default*/)> signal_default_interface_changed();

    protected:

      std::shared_ptr<Connection> m_connection;

      std::string m_destination;

      Path m_path;
      
      mutable std::shared_mutex m_interfaces_rwlock;

      std::mutex m_name_mutex;

      Interfaces m_interfaces;

      std::shared_ptr<InterfaceProxy> m_default_interface;

      sigc::signal<void(std::shared_ptr<InterfaceProxy>,std::shared_ptr<InterfaceProxy>)> m_signal_default_interface_changed;

      sigc::signal<void(std::shared_ptr<InterfaceProxy>)> m_signal_interface_added;

      sigc::signal<void(std::shared_ptr<InterfaceProxy>)> m_signal_interface_removed;

      typedef std::map<std::shared_ptr<InterfaceProxy>,sigc::connection> InterfaceSignalNameConnections;

      InterfaceSignalNameConnections m_interface_signal_name_connections;

      void on_interface_name_changed(const std::string& oldname, const std::string& newname, std::shared_ptr<InterfaceProxy> interface);

  };

}

#endif