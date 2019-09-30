#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/http_headers.h>
#include <pistache/cookie.h>
#include <pistache/endpoint.h>
#include <pistache/common.h>
#include "requesthandler.hpp"
#include "shared/symart_service.hpp"
#include <QImage>

using namespace std;
using namespace Pistache;

class MyHandler : public Http::Handler
{

  HTTP_PROTOTYPE( MyHandler )

  void onRequest( const Http::Request& clientRequest, Http::ResponseWriter response ) override
  {

    //  http://localhost:9080/ping
    if ( clientRequest.resource() == "/ping" ) {
      if ( clientRequest.method() == Http::Method::Get ) {

        using namespace Http;

        auto query = clientRequest.query();

        // http://localhost:9080/ping?chunked
        if ( query.has( "chunked" ) ) {
          std::cout << "Using chunked encoding" << std::endl;

          response.headers()
          .add<Header::Server>( "pistache/0.1" )
          .add<Header::ContentType>( MIME( Text, Plain ) );

          response.cookies()
          .add( Cookie( "lang", "en-US" ) );

          auto stream = response.stream( Http::Code::Ok );
          stream << "PO";
          stream << "NG";
          stream << " is streaming";
          stream << ends;
        } else {
          response.send( Http::Code::Ok, "PONG" );
        }

      }
    } else if ( clientRequest.resource() == "/echo" ) {
      if ( clientRequest.method() == Http::Method::Get ) {
        response.send( Http::Code::Ok, " 4567" );
        // response.send( Http::Code::Ok, req.body(), MIME( Text, Plain ) );
      } else {
        response.send( Http::Code::Method_Not_Allowed );
      }
    } else if ( clientRequest.resource() == "/stream_binary" ) {
      auto stream = response.stream( Http::Code::Ok );
      QByteArray bytes = paintStripes( "test", 1000, 7, 1.0 );
      const char* binary_data = bytes.constData();
      size_t chunk_size = 100;
      size_t chunck_count_total = bytes.size() / chunk_size;

      for ( size_t i = 0; i < chunck_count_total; ++i ) {
        stream.write( binary_data + ( i * chunk_size ), chunk_size );
        stream.flush();
      }

      stream.ends();
    }
    // http://localhost:9080/stream_binary
    else if ( clientRequest.resource() == "/stream_binary_X" ) {
      // http://localhost:9080/stream_binary
      QByteArray bytes {paintStripes( "test", 1000, 7, 1.0 )};
      auto stream = response.stream( Http::Code::Ok );
      stream.write( bytes, bytes.size() );
      stream.flush();
      stream.ends();
    } else if ( clientRequest.resource() == "/exception" ) {
      throw std::runtime_error( "Exception thrown in the handler" );
    } else if ( clientRequest.resource() == "/timeout" ) {
      response.timeoutAfter( std::chrono::seconds( 2 ) );
    } else if ( clientRequest.resource() == "/static" ) {
      if ( clientRequest.method() == Http::Method::Get ) {
        Http::serveFile( response, "README.md" ).then( []( ssize_t bytes ) {
          std::cout << "Sent " << bytes << " bytes" << std::endl;
        }, Async::NoExcept );
      }
    } else {
      response.send( Http::Code::Not_Found );
    }

  }

  void onTimeout(
    const Http::Request& req,
    Http::ResponseWriter response ) override
  {
    UNUSED( req );
    response
    .send( Http::Code::Request_Timeout, "Timeout" )
    .then( [ = ]( ssize_t ) { }, PrintException() );
  }

};

int main( int argc, char* argv[] )
{
  RequestHandler requestHandler{9080};

//  int port;
//  int thr = 2;

//  if ( argc >= 2 ) {
//    port = std::stol( argv[1] );

//    if ( argc == 3 ) {
//      thr = std::stol( argv[2] );
//    }
//  }

//  Address addr( Ipv4::any(), port );

//  cout << "Cores = " << hardware_concurrency() << endl;
//  cout << "Using " << thr << " threads" << endl;

//  auto server = std::make_shared<Http::Endpoint>( addr );

//  auto opts = Http::Endpoint::options().threads( thr );
//  server->init( opts );
//  server->setHandler( Http::make_handler<MyHandler>() );
//  server->serve();
}
