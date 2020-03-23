#include "requesthandler.hpp"
#include "symart_service.hpp"
#include <QDebug>
#include <QImage>
#include <QStringList>
#include <QTextCodec>
#include <thread>
#include <iostream>
#include <limits>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>


typedef std::function<QByteArray( QStringList& )> drawingGenerateFunc;
typedef std::function<QByteArray( QString& )> predicateFunc;
typedef std::function<QByteArray( void )> loadFunc;
typedef std::function<QByteArray( void )> drawingTestFunc;

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
    //  example:  http://localhost:60564/ask?canTileImage/id123
    predicateFuncs.clear();
    predicateFuncs.emplace( "canTileImage",  [ = ]( QString & id ) {  return fromBool( canTileImage( id ) ).toLatin1();} );
    predicateFuncs.emplace( "isSymmetricView",  [ = ]( QString & id ) {  return fromBool( isSymmetricView( id ) ).toLatin1();} );
  }

  void initDrawingGetFuncs()
  {
    //  example:  http://localhost:60564/get?trap/id123//32/74/135/256/8  http://localhost:60564/get?walk2/id123/400/200/false/1  http://localhost:60564/get?hexagonalStretch/id123
    drawingGetFuncs.clear();

    drawingGetFuncs.emplace( "lastImage",  [ = ]( QStringList & list ) {  return lastGeneratedImage( list[1] );  } );

    drawingGetFuncs.emplace( "hexagonalStretch",  [ = ]( QStringList & list ) {  return hexagonalStretch( list[1] );  } );

    drawingGetFuncs.emplace( "randomizeTiles",  [ = ]( QStringList & list ) {  return randomizeTiles( list[1], list[2].toInt(), list[3].toInt() );  } );

    drawingGetFuncs.emplace( "hyperbolicImage",  [ = ]( QStringList & list ) {  return makeHyperbolic( list[1], list[2].toInt(), list[3].toInt() );  } );

    drawingGetFuncs.emplace( "squiggles",  [ = ]( QStringList & list ) {
      return paintSquiggles( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5].toDouble(),
                             list[6].toDouble(), list[7].toDouble(), list[8].toDouble() );
    } );
    drawingGetFuncs.emplace( "squigglesImageColors",  [ = ]( QStringList & list ) {
      return paintSquiggles( list[1], list[2].toDouble(), toBool( list[3] ), toBool( list[4] ),
                             toBool( list[5] ),  list[6].toInt(), list[7].toInt(), list[8].toInt(), list[9].toDouble(),
                             list[10].toDouble(), list[11].toDouble(), list[12].toDouble() );
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
    drawingGetFuncs.emplace( "trap",  [ = ]( QStringList & list ) {
      return drawTrap( list[1], list[2].toInt(), list[3].toInt(),
                       list[4].toInt(), list[5].toInt(), list[6].toInt() );
    } );

    drawingGetFuncs.emplace( "quasiTrap",  []( QStringList & list ) {
      return drawQuasiTrap( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(),
                            list[5].toInt(), list[6].toInt(),  list[7].toInt(), list[8].toDouble() );
    } );

    drawingGetFuncs.emplace( "quasiTrapPoly",  []( QStringList & list ) {
      return drawQuasiTrapPoly( list[1], list[2].toInt(), list[3].toInt(),
                                list[4].toInt(), list[5].toInt(), list[6].toInt(),
                                list[7].toInt(), list[8].toDouble() );
    } );

    drawingGetFuncs.emplace( "caStart",  [ = ]( QStringList & list ) {
      return caGenerateImage( list[1], list[2].toInt(), list[3].toInt(), list[4].toInt(), list[5].toInt(), list[6].toInt(),
                              list[7].toInt(), list[8].toInt(), list[9].toInt(), list[10].toDouble() );
    } );

    drawingGetFuncs.emplace( "caContinue",  [ = ]( QStringList & list ) {  return caContinue( list[1], list[2].toInt(), list[3].toDouble() );  } );

    drawingGetFuncs.emplace( "updateSquiggles",  [ = ]( QStringList & list ) {  return updateSquiggles( list[1], list[2].toInt(), list[3].toInt() );  } );

    drawingGetFuncs.emplace( "walk",  [ = ]( QStringList & list ) {  return drawWalk( list[1], list[2].toInt(), list[3].toInt(), toBool( list[4] ), list[5].toInt()  );  } );

    drawingGetFuncs.emplace( "walk2",  [ = ]( QStringList & list ) {  return drawWalk2( list[1], list[2].toInt(), list[3].toInt(), toBool( list[4] ), list[5].toInt()  );  } );

  }

  void initDrawingTestFuncs()
  {
    // example:  http://localhost:60564/test?quasiPeriodicStripes
    drawingTestFuncs.clear();
    drawingTestFuncs.emplace( "ca", []() { return caGenerateImage( "test", 600, 600, 3, 295, 295, 305, 305, 300, 2.5   );  } );
    drawingTestFuncs.emplace( "clouds", []() { return paintClouds( "test",  256, 9, 111, 123, 150, 90, 190, 200, 55, 178, 222, 1 );} );
    drawingTestFuncs.emplace( "clusters",  []() { return paintClusters( "test", 400, 3, 1.50 );} );
    drawingTestFuncs.emplace( "hyperbolicClouds",  []() { return paintHyperbolicClouds( "test",  256, 2, 3, 2, 2, 2, 1, 111, 123, 150, 90, 190, 200, 55, 178, 222, 1 );} );
    drawingTestFuncs.emplace( "hyperbolicLines", []() { return paintHyperbolicLines( "test",  400, 2, 3, 2, 2, 2, 1, 1, 1.0, 1.50,  11 );} );
    drawingTestFuncs.emplace( "lines", []() { return paintLines( "test", 400, 13, 5, "Line", 2, true, "Curl", 4, true, "Beads", 6, true  );} );
    drawingTestFuncs.emplace( "quasiPeriodicStripes",  []() { return paintQuasiperiodicStripes( "test", 400, 20, 1.5 );} );
    drawingTestFuncs.emplace( "quasiTrap", []() { return drawQuasiTrap( "test", 55, 178, 222, 400, 400, 8,  66.6 );} );
    drawingTestFuncs.emplace( "quasiTrapPoly", []() { return drawQuasiTrapPoly( "test", 55, 178, 222, 200, 200, 7,  44.4 );} );
    drawingTestFuncs.emplace( "squiggles",  []() { return paintSquiggles( "test", 15, 512, 10, 1.3, 1.50, 1.3, 1.5 );} );
    drawingTestFuncs.emplace( "stripes",  []() { return paintStripes( "test", 600, 5, .50 );} );
    drawingTestFuncs.emplace( "trap", []() { return drawTrap( "test", 32, 74, 135, 144, 8 );} );
    drawingTestFuncs.emplace( "walk", []() { return drawWalk( "test", 200, 400, true, 0 );} );
    drawingTestFuncs.emplace( "walk2", []() { return drawWalk( "test", 1000, 1000, false, 1 );} );
  }

private:
  void readyHandler( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    QByteArray bytes{"Available test API:\n"};

    for ( auto item : drawingTestFuncs ) {
      bytes.append( "\n/test?" );
      bytes.append( item.first.c_str() );
    }

    auto stream = response.stream( Pistache::Http::Code::Ok );
    stream.write( bytes, bytes.size() );
    stream.flush();
    stream.ends();

    response.send( Pistache::Http::Code::Ok,  request.query().as_str() );
  }

  void testHandler( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
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
    } catch ( const std::exception& e ) { qWarning() << Q_FUNC_INFO << e.what();}


    if ( !success ) {
      response.send( Pistache::Http::Code::Bad_Request, request.query().as_str() +
                     " is not a valid request. To test the API, enter the request: /ready" );
    }
  }

  void imageColorsHandler( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
  {
    Q_UNUSED ( request )
    Q_UNUSED ( response )
    qDebug() << Q_FUNC_INFO << "Use of an image palette is not yet supported.";

//    bool success{false};

//    try {
//      auto payload =  request.query().parameters()[3];

//      loadColorsImage( QTextCodec::codecForMib( 2259 )->fromUnicode( payload.c_str() ) );
//      success = true;
//    } catch ( const std::exception ) { }


//    if ( !success ) {
//      response.send( Pistache::Http::Code::Bad_Request, request.query().as_str() +
//                     " is not a valid  request. To test the API, enter the request: /ready" );

//    }
  }

  void askHandler( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
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
    } catch ( const std::exception& e ) { qWarning() << Q_FUNC_INFO << e.what();}


    if ( !success ) {
      response.send( Pistache::Http::Code::Bad_Request, request.query().as_str() +
                     " is not a valid  request. To test the API, enter the request: /ready" );

    }
  }

  void getHandler( const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response )
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
    } catch ( const std::exception& e ) { qWarning() << Q_FUNC_INFO << e.what();}


    if ( !success ) {
      response.send( Pistache::Http::Code::Bad_Request, request.query().as_str() +
                     " is not a valid  request. To test the API, enter the request: /ready" );

    }
  }

  void setupRoutes()
  {
    using namespace Pistache::Rest;

    Routes::Get( router,  "/ready", Routes::bind( &SymArtEndpoint::readyHandler, this ) );
    Routes::Get( router,  "/ask", Routes::bind( &SymArtEndpoint::askHandler,  this ) );
    Routes::Get( router,  "/get", Routes::bind( &SymArtEndpoint::getHandler,  this ) );
    Routes::Get( router,  "/test", Routes::bind( &SymArtEndpoint::testHandler,  this ) );
    Routes::Get( router, "/imageColors", Routes::bind( &SymArtEndpoint::imageColorsHandler,  this ) );
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
  Pistache::Address addr( Pistache::Ipv4::any(), static_cast<uint16_t>( port )  );
  std::cout << "Using " << nthreads << " threads" << std::endl;
  std::cout << "Connection IP is " << addr.host() << " and port is " << port   << std::endl;

  SymArtEndpoint endpoint( addr );
  endpoint.init( nthreads );
  endpoint.start();
}
