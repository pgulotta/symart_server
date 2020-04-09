#include "purgeimageshandler.hpp"
#include "symart_service.hpp"




PurgeImagesHandler::PurgeImagesHandler( QObject* parent ) : QObject( parent )
{
}

void PurgeImagesHandler::run()
{
  while ( true ) {
    purgeOldImages();
  }
}
