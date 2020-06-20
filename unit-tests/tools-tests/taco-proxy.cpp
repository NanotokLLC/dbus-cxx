#include "DbusTacoProxy.h"

int main(){
    std::shared_ptr<DBus::Dispatcher> dispatch = DBus::StandaloneDispatcher::create();
    std::shared_ptr<DBus::Connection> conn = dispatch->create_connection( DBus::BusType::SESSION );

    std::shared_ptr<DbusTacoProxy> ptr = DbusTacoProxy::create( conn, "taco.orderer" );
}