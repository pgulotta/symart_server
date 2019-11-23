#pragma once

#include <QObject>
#include <QRunnable>


class PurgeOldImagesHandler : public QObject, public QRunnable
{
  Q_OBJECT
public:
  explicit PurgeOldImagesHandler( QObject* parent = nullptr );
  void run() override;

signals:

public slots:
};


