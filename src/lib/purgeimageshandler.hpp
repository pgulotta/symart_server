#pragma once

#include <QObject>
#include <QRunnable>


class PurgeImagesHandler : public QObject, public QRunnable
{
  Q_OBJECT
public:
  explicit PurgeImagesHandler( QObject* parent = nullptr );
  void run() override;

signals:

public slots:
};


