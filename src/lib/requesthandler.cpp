#include "requesthandler.hpp"
#include "shared/symart_service.hpp"
#include <QImage>
#include <thread>
#include <iostream>
#include <limits>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

typedef std::function<QByteArray( void )> drawingTestFunc;

class SymArtEndpoint
{
public:
  explicit SymArtEndpoint( Pistache::Address addr )
    : httpEndpoint( std::make_shared<Pistache::Http::Endpoint>( addr ) )
  {
    initDrawingTestFuncs();
  }

  void init( size_t nthreads )
  {
//    auto opts = Pistache::Http::Endpoint::options().threads( static_cast < int > ( nthreads )  );
//    httpEndpoint->init( opts );
    httpEndpoint->init( Pistache::Http::Endpoint::options().threads( static_cast < int > ( nthreads )  ) );
    setupRoutes();
  }

  void start()
  {
    httpEndpoint->setHandler( router.handler() );
    httpEndpoint->serve();
  }

  void initDrawingTestFuncs()
  {
    drawingTestFuncs[std::string{"clusters" }] =  []() { return paintClusters( "test", 400, 3, 1.50 );};
    drawingTestFuncs[std::string{"quasiPeriodicStripes" }] =  []() { return paintQuasiperiodicStripes( "test", 400, 20, 1.5 );};
    drawingTestFuncs[std::string{"quasiTrap" }] =  []() { return drawQuasiTrap( "test", 800, 800, 8,  66.6 );};
    drawingTestFuncs[std::string{"quasiTrapPoly" }] =  []() { return drawQuasiTrapPoly( "test", 800, 800, 7,  44.4 );};
    drawingTestFuncs[std::string{"squiggles" }] =  []() { return paintSquiggles( "test", 15, 512, 10, 1.3, 1.50, 1.3, 1.5 );};
    drawingTestFuncs[std::string{"stripes" }] =  []() { return paintStripes( "test", 600, 5, .50 );};
    drawingTestFuncs[std::string{"trap" }] =  []() { return drawTrap( "test", 500, 2 );};
    drawingTestFuncs[std::string{"walk" }] =  []() { return drawWalk( "test", 200, 400, true, 0 );};
    drawingTestFuncs[std::string{"walk2" }] =  []() { return drawWalk( "test", 400, 200, false, 1 );};
  }


private:
  void handleTest( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    bool success = false;

    // example:  http://localhost:9080/test/?quasiPeriodicStripes
    if ( request.method() == Pistache::Http::Method::Get ) {
      auto param = request.query().parameters()[0];

      if ( drawingTestFuncs.find( param ) != drawingTestFuncs.end() ) {
        QByteArray bytes {drawingTestFuncs[param]()};
        auto stream = response.stream( Pistache::Http::Code::Ok );
        stream.write( bytes, bytes.size() );
        stream.flush();
        stream.ends();
        success = true;
      }
    }

    if ( !success ) {
      response.send( Pistache::Http::Code::Bad_Request,
                     "This is not a valid request. To test the API, enter the request: /ready" );

    }
  }

  void handleReady( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    QByteArray bytes{"Available test API:\n"};

    for ( auto item : drawingTestFuncs ) {
      bytes.append( "\n/test/?" );
      bytes.append( item.first.c_str() );
    }

    auto stream = response.stream( Pistache::Http::Code::Ok );
    stream.write( bytes, bytes.size() );
    stream.flush();
    stream.ends();

    response.send( Pistache::Http::Code::Ok,  request.query().as_str() );
  }

  void handleDraw( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    response.send( Pistache::Http::Code::Ok,  request.query().as_str() );
  }

  void setupRoutes()
  {
    using namespace Pistache::Rest;
    Routes::Get( router, "/ready", Routes::bind( &SymArtEndpoint::handleReady, this ) );
    Routes::Get( router, "/draw",  Routes::bind( &SymArtEndpoint::handleDraw,  this ) );
    Routes::Get( router, "/test",  Routes::bind( &SymArtEndpoint::handleTest,  this ) );
  }


private:
  std::shared_ptr< Pistache::Http::Endpoint> httpEndpoint;
  Pistache::Rest::Router router;
  std::map<std::string, drawingTestFunc> drawingTestFuncs;
};



RequestHandler::RequestHandler( int  port )
{
  unsigned int nthreads = std::thread::hardware_concurrency();
  std::cout << "Using " << nthreads << " threads" << std::endl;
  std::cout << "Listening on port " << port   << std::endl;


  Pistache::Address addr( Pistache::Ipv4::any(), static_cast<uint16_t>( port )  );
  SymArtEndpoint endpoint( addr );
  endpoint.init( nthreads );
  endpoint.start();
}
