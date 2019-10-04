﻿#include "requesthandler.hpp"
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
typedef std::function<QByteArray( QString& )> predicateFunc;


class SymArtEndpoint
{
public :
  explicit SymArtEndpoint( Pistache::Address addr )
    : httpEndpoint( std::make_shared<Pistache::Http::Endpoint>( addr ) )
  {
    initDrawingTestFuncs();
    initDrawingGetFuncs();
    initPredicateFuncs();
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

  void initPredicateFuncs()
  {
    //  example:  http://localhost:9080/ask/?canTileImage/id123
    predicateFuncs.clear();
    predicateFuncs.emplace( "canTileImage",  [ = ]( QString & item ) {  return fromBool( canTileImage( item ) ).toLatin1();} );
    predicateFuncs.emplace( "isSymmetricView",  [ = ]( QString & item ) {  return fromBool( isSymmetricView( item ) ).toLatin1();} );
  }

  void initDrawingGetFuncs()
  {
    //  example:  http://localhost:9080/get/?trap/id123/200/5  http://localhost:9080/get/?walk2/id123/400/200/false/1  http://localhost:9080/get/?hexagonalStretch/id123
    drawingGetFuncs.clear();

    drawingGetFuncs.emplace( "original",  [ = ]( QStringList & list ) {  return originalImage( list[1] );  } );

    drawingGetFuncs.emplace( "hexagonalStretch",  [ = ]( QStringList & list ) {  return hexagonalStretch( list[1] );  } );

    drawingGetFuncs.emplace( "randomizeTiles",  [ = ]( QStringList & list ) {  return randomizeTiles( list[1], list[2].toInt(), list[3].toInt() );  } );

    drawingGetFuncs.emplace( "makeHyperbolic",  [ = ]( QStringList & list ) {  return makeHyperbolic( list[1], list[2].toInt(), list[3].toInt() );  } );

    drawingGetFuncs.emplace( "squiggles",  [ = ]( QStringList & list ) {
      return paintSquiggles( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5].toDouble(),
                             list[6].toDouble(), list[7].toDouble(), list[8].toDouble() );
    } );
    drawingGetFuncs.emplace( "squigglesImageColors",  [ = ]( QStringList & list ) {
      return paintSquiggles( list[1], list[2], list[3].toDouble(), toBool( list[4] ), toBool( list[5] ),
                             toBool( list[6] ),  list[7].toInt(), list[8].toInt(), list[9].toInt(), list[10].toDouble(),
                             list[11].toDouble(), list[12].toDouble(), list[13].toDouble() );
    } );

    drawingGetFuncs.emplace( "clouds",  [ = ]( QStringList & list ) {
      return paintClouds( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5].toInt(),
                          list[6].toInt(),  list[7].toInt(), list[8].toInt(), list[9].toInt(),
                          list[10].toInt(), list[11].toInt(), list[12].toInt(), list[13].toInt() );
    } );

    drawingGetFuncs.emplace( "hyperbolicClouds",  [ = ]( QStringList & list ) {
      return paintHyperbolicClouds( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5].toInt(),
                                    list[6].toInt(),  list[7].toInt(), list[8].toInt(), list[9].toInt(),
                                    list[10].toInt(), list[11].toInt(), list[12].toInt(), list[13].toInt(), list[14].toInt(), list[15].toInt(),
                                    list[16].toInt(), list[17].toInt(), list[18].toInt() );
    } );

    drawingGetFuncs.emplace( "lines",  [ = ]( QStringList & list ) {
      return paintLines( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5], list[6].toInt(),
                         toBool( list[7] ), list[8], list[9].toInt(),
                         toBool( list[10] ), list[11], list[12].toInt(), toBool( list[13] ) );
    } );

    drawingGetFuncs.emplace( "linesImageColors",  [ = ]( QStringList & list ) {
      return paintLines( list[1], list[2], list[3].toDouble(), toBool( list[4] ), toBool( list[5] ),
                         toBool( list[6] ),  list[7].toInt(), list[8].toInt(), list[9].toInt(),
                         list[10], list[11].toInt(), toBool( list[12] ),
                         list[13], list[14].toInt(), toBool( list[15] ),
                         list[160], list[17].toInt(), toBool( list[18] ) );
    } );

    drawingGetFuncs.emplace( "stripes",  [ = ]( QStringList & list ) {  return paintStripes( list[1], list[2].toInt(), list[3].toInt(), list[4].toDouble() );  } );

    drawingGetFuncs.emplace( "clusters",  [ = ]( QStringList & list ) {  return paintClusters( list[1], list[2].toInt(), list[3].toInt(), list[4].toDouble() );  } );

    drawingGetFuncs.emplace( "quasiPeriodicStripes",  []( QStringList & list ) { return paintQuasiperiodicStripes( list[1], list[2].toInt(), list[3].toInt(), list[4].toDouble() );  } );

    drawingGetFuncs.emplace( "hyperbolicLines",  [ = ]( QStringList & list ) {
      return paintHyperbolicLines( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5].toInt(),
                                   list[6].toInt(),  list[7].toInt(), list[8].toInt(), list[9].toInt(),
                                   list[10].toFloat(), list[11].toFloat(), list[12].toInt() );
    } );

    drawingGetFuncs.emplace( "trap",  [ = ]( QStringList & list ) {  return drawTrap( list[1], list[2].toInt(), list[3].toInt() );  } );

    drawingGetFuncs.emplace( "quasiTrap",  []( QStringList & list ) { return drawQuasiTrap( list[1], list[2].toInt(), list[3].toInt(),  list[4].toInt(), list[5].toDouble() );  } );

    drawingGetFuncs.emplace( "quasiTrapPoly",  []( QStringList & list ) { return drawQuasiTrapPoly( list[1], list[2].toInt(), list[3].toInt(),  list[4].toInt(), list[5].toDouble() );  } );

    drawingGetFuncs.emplace( "caStart",  [ = ]( QStringList & list ) {
      return caGenerateImage( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5].toInt(), list[6].toInt(),
                              list[7].toInt(), list[8].toInt(), list[9].toInt(), list[10].toDouble() );
    } );

    drawingGetFuncs.emplace( "caContinue",  [ = ]( QStringList & list ) {  return caContinue( list[1], list[2].toInt(), list[3].toDouble() );  } );

    drawingGetFuncs.emplace( "updateSquiggles",  [ = ]( QStringList & list ) {  return updateSquiggles( list[1], list[2].toInt(), list[3].toInt() );  } );



    drawingGetFuncs.emplace( "walk",  [ = ]( QStringList & list ) {  return drawWalk( list[1], list[2].toInt(), list[3].toInt(), toBool( list[4] ), list[5].toInt()  );  } );

    drawingGetFuncs.emplace( "walk2",  [ = ]( QStringList & list ) {  return drawWalk2( list[1], list[2].toInt(), list[3].toInt(), toBool( list[4] ), list[5].toInt()  );  } );

//    drawingGenerateFuncs.emplace( "hyperbolicClouds",  [ = ]( QStringList & list ) {
//        return paintHyperbolicClouds( list[1], list[2], list[3], list[4], list[5], list[6],  list[7], list[8], list[9],
//                                     list[10], list[11], list[12], list[13], list[14], list[15], list[16], list[17], list[18] );
//    } );


  }

  void initDrawingTestFuncs()
  {
    // example:  http://localhost:9080/test/?quasiPeriodicStripes
    drawingTestFuncs.clear();
    drawingTestFuncs.emplace( "ca", []() { return caGenerateImage( "test", 600, 600, 3, 295, 295, 305, 305, 300, 2.5   );  } );
    drawingTestFuncs.emplace( "clouds", []() { return paintClouds( "test",  256, 9, 111, 123, 150, 90, 190, 200, 55, 178, 222, 1 );} );
    drawingTestFuncs.emplace( "clusters",  []() { return paintClusters( "test", 400, 3, 1.50 );} );
    drawingTestFuncs.emplace( "hyperbolicClouds",  []() { return paintHyperbolicClouds( "test",  256, 2, 3, 2, 2, 2, 1, 111, 123, 150, 90, 190, 200, 55, 178, 222, 1 );} );
    drawingTestFuncs.emplace( "hyperbolicLines", []() { return paintHyperbolicLines( "test",  400, 2, 3, 2, 2, 2, 1, 1, 1.0, 1.50,  11 );} );
    drawingTestFuncs.emplace( "lines", []() { return paintLines( "test", 400, 13, 5, "Line", 2, true, "Curl", 4, true, "Beads", 6, true  );} );
    drawingTestFuncs.emplace( "quasiPeriodicStripes",  []() { return paintQuasiperiodicStripes( "test", 400, 20, 1.5 );} );
    drawingTestFuncs.emplace( "quasiTrap", []() { return drawQuasiTrap( "test", 800, 800, 8,  66.6 );} );
    drawingTestFuncs.emplace( "quasiTrapPoly", []() { return drawQuasiTrapPoly( "test", 800, 800, 7,  44.4 );} );
    drawingTestFuncs.emplace( "squiggles",  []() { return paintSquiggles( "test", 15, 512, 10, 1.3, 1.50, 1.3, 1.5 );} );
    drawingTestFuncs.emplace( "stripes",  []() { return paintStripes( "test", 600, 5, .50 );} );
    drawingTestFuncs.emplace( "trap", []() { return drawTrap( "test", 500, 2 );} );
    drawingTestFuncs.emplace( "walk", []() { return drawWalk( "test", 200, 400, true, 0 );} );
    drawingTestFuncs.emplace( "walk2", []() { return drawWalk( "test", 400, 200, false, 1 );} );
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
                     " is not a valid request. To test the API, enter the request: /ready" );
    }
  }

  void handleAsk( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    bool success = false;

    try {

      if ( request.method() == Pistache::Http::Method::Get ) {
        auto query{request.query().parameters()[0]};

        if ( query.length() > 0 ) {
          QString params = query.c_str();
          QStringList list =  params.split( '/', QString::SkipEmptyParts );
          auto functionId = list[0];

          if ( predicateFuncs.find( functionId ) != predicateFuncs.end() ) {
            QByteArray bytes {predicateFuncs[functionId]( list[1] )};
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
                     " is not a valid  request. To test the API, enter the request: /ready" );

    }
  }

  void handleGet( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    bool success = false;

    try {

      if ( request.method() == Pistache::Http::Method::Get ) {
        auto query{request.query().parameters()[0]};

        if ( query.length() > 0 ) {
          QString params = query.c_str();
          QStringList list =  params.split( '/', QString::SkipEmptyParts );
          auto functionId = list[0];

          if ( drawingGetFuncs.find( functionId ) != drawingGetFuncs.end() ) {
            QByteArray bytes {drawingGetFuncs[functionId]( list )};
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
                     " is not a valid  request. To test the API, enter the request: /ready" );

    }
  }

  void setupRoutes()
  {
    using namespace Pistache::Rest;
    Routes::Get( router, "/ready", Routes::bind( &SymArtEndpoint::handleReady, this ) );
    Routes::Get( router, "/ask",  Routes::bind( &SymArtEndpoint::handleAsk,  this ) );
    Routes::Get( router, "/get",  Routes::bind( &SymArtEndpoint::handleGet,  this ) );
    Routes::Get( router, "/test",  Routes::bind( &SymArtEndpoint::handleTest,  this ) );
  }


private:
  std::shared_ptr< Pistache::Http::Endpoint> httpEndpoint;
  Pistache::Rest::Router router;
  std::map<std::string, drawingTestFunc> drawingTestFuncs;
  std::map<QString, drawingGenerateFunc> drawingGetFuncs;
  std::map<QString, predicateFunc> predicateFuncs;
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