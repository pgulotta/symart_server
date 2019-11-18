#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/endpoint.h>
#include <pistache/common.h>
#include "requesthandler.hpp"
#include "symart_service.hpp"
#include <QCoreApplication>
#include <QCommandLineParser>

using namespace Pistache;

int main( int argc, char* argv[] )
{
  QCoreApplication app( argc, argv );
  QCoreApplication::setApplicationName( "symart_server" );
  QCoreApplication::setApplicationVersion( "1.0" );
  QCommandLineParser parser;
  parser.setApplicationDescription( "Service that generates symmetric images" );
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption definePortOption( QStringList() << "p" << "service-port",
                                       QCoreApplication::translate( "main", "Port the service listens to for symart requests." ),
                                       QCoreApplication::translate( "main", "port" ) );
  parser.addOption( definePortOption );
  const QCommandLineOption helpOption = parser.addHelpOption();
  const QCommandLineOption versionOption = parser.addVersionOption();
  parser.process( app );

  int port{60564};

  if ( parser.isSet( definePortOption ) ) {
    auto definedPort =  parser.value( definePortOption ).toInt();

    if ( definedPort > 0 ) {
      port = definedPort;
    }
  }

  RequestHandler requestHandler{port};
}
