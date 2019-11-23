#include "purgeoldimageshandler.hpp"
#include "symart_service.hpp"




PurgeOldImagesHandler::PurgeOldImagesHandler( QObject* parent ) : QObject( parent )
{
}

void PurgeOldImagesHandler::run()
{
  while ( true ) {
    purgeOldImages();
  }
}
