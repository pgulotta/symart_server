#include "requesthandler.hpp"
#include "shared/symart_service.hpp"
#include <QImage>
#include <QStringList>
#include <thread>
#include <iostream>
#include <limits>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

typedef std::function<QByteArray( void )> drawingTestFunc;
typedef std::function<QByteArray( QStringList& )> drawingGenerateFunc;

class SymArtEndpoint
{
public:
  explicit SymArtEndpoint( Pistache::Address addr )
    : httpEndpoint( std::make_shared<Pistache::Http::Endpoint>( addr ) )
  {
    initDrawingTestFuncs();
    initDrawingGenerateFuncs();
  }

  void init( size_t nthreads )
  {
    httpEndpoint->init( Pistache::Http::Endpoint::options().threads( static_cast < int > ( nthreads )  ) );
    setupRoutes();
  }

  void start()
  {
    httpEndpoint->setHandler( router.handler() );
    httpEndpoint->serve();
  }

//  QByteArray trap( QStringList& list )
//  {
//    return drawTrap( "test", list[1].toInt(), list[2].toInt() );
//  }

  void initDrawingGenerateFuncs()
  {
//    drawingGenerateFunc func = [ = ]( QStringList & list ) { return trap( list  );};
//    drawingGenerateFunc[std::string{"trap" }] = func;
//    drawingGenerateFuncs.emplace( "trap", func );
    // drawingGenerateFuncs.emplace( "trap",  [ = ]( QStringList & list ) { return trap( list  );} );
//http://localhost:9080/generate/?trap/200/5
    drawingGenerateFuncs.emplace( "trap",  [ = ]( QStringList & list ) {  return drawTrap( "test", list[1].toInt(), list[2].toInt() );  } );

  }

  void initDrawingTestFuncs()
  {
    drawingTestFuncs[std::string{"ca" }] =  []() { return caGenerateImage( "test", 600, 600, 3, 295, 295, 305, 305, 300, 2.5  );};
    drawingTestFuncs[std::string{"clouds" }] =  []() {
      return paintClouds( "test",  256, 9, 111, 123, 150, 90, 190, 200, 55, 178, 222, 1 );
    };
    drawingTestFuncs[std::string{"clusters" }] =  []() { return paintClusters( "test", 400, 3, 1.50 );};
    drawingTestFuncs[std::string{"hyperbolicClouds" }] =  []() {
      return paintHyperbolicClouds( "test",  256, 2, 3, 2, 2, 2, 1, 111, 123, 150, 90, 190, 200, 55, 178, 222, 1 );
    };
    drawingTestFuncs[std::string{"hyperbolicLines" }] =  []() { return paintHyperbolicLines( "test",  400, 2, 3, 2, 2, 2, 1, 1, 1.0, 1.50,  11 );};
    drawingTestFuncs[std::string{"lines" }] =  []() { return paintLines( "test", 400, 13, 5, "Line", 2, true, "Curl", 4, true, "Beads", 6, true  );};
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

  void handleTest( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    bool success = false;

    try {
      // example:  http://localhost:9080/test/?quasiPeriodicStripes
      if ( request.method() == Pistache::Http::Method::Get ) {
        auto functionId = request.query().parameters()[0];

        if ( drawingTestFuncs.find( functionId ) != drawingTestFuncs.end() ) {
          QByteArray bytes {drawingTestFuncs[functionId]()};
          auto stream = response.stream( Pistache::Http::Code::Ok );
          stream.write( bytes, bytes.size() );
          stream.flush();
          stream.ends();
          success = true;
        }
      }
    } catch ( const std::exception ) { }


    if ( !success ) {
      response.send( Pistache::Http::Code::Bad_Request, request.query().as_str() +
                     " is not a valid 'test' request. To test the API, enter the request: /ready" );
    }
  }

  void handleGenerate( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
//  http://localhost:9080/generate/?trap/
    bool success = false;

    try {
      // example:  http://localhost:9080/test/?quasiPeriodicStripes
      if ( request.method() == Pistache::Http::Method::Get ) {
        auto query{request.query().parameters()[0]};

        if ( query.length() > 0 ) {
          QString params = query.c_str();
          QStringList list =  params.split( '/', QString::SkipEmptyParts );
          auto functionId = list[0];

          if ( drawingGenerateFuncs.find( functionId ) != drawingGenerateFuncs.end() ) {
            QByteArray bytes {drawingGenerateFuncs[functionId]( list )};
            auto stream = response.stream( Pistache::Http::Code::Ok );
            stream.write( bytes, bytes.size() );
            stream.flush();
            stream.ends();
            success = true;
          }
        }
      }
    } catch ( const std::exception ) { }


    if ( !success ) {
      response.send( Pistache::Http::Code::Bad_Request, request.query().as_str() +
                     " is not a valid 'generate' request. To test the API, enter the request: /ready" );

    }
  }

  void setupRoutes()
  {
    using namespace Pistache::Rest;
    Routes::Get( router, "/ready", Routes::bind( &SymArtEndpoint::handleReady, this ) );
    Routes::Get( router, "/generate",  Routes::bind( &SymArtEndpoint::handleGenerate,  this ) );
    Routes::Get( router, "/test",  Routes::bind( &SymArtEndpoint::handleTest,  this ) );
  }


private:
  std::shared_ptr< Pistache::Http::Endpoint> httpEndpoint;
  Pistache::Rest::Router router;
  std::map<std::string, drawingTestFunc> drawingTestFuncs;
  std::map<QString, drawingGenerateFunc> drawingGenerateFuncs;
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
