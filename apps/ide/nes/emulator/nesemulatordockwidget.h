#ifndef NESEMULATORDOCKWIDGET_H
#define NESEMULATORDOCKWIDGET_H

#include "nes_emulator_core.h"
#include "nesemulatorrenderer.h"

#include <QDockWidget>
#include <QKeyEvent>

namespace Ui {
   class NESEmulatorDockWidget;
}

class NESEmulatorDockWidget : public QDockWidget
{
   Q_OBJECT

public:
   explicit NESEmulatorDockWidget(QWidget *parent = 0);
   virtual ~NESEmulatorDockWidget();

   void setLinearInterpolation(bool enabled) { renderer->setLinearInterpolation(enabled); }
   void set43Aspect(bool enabled) { renderer->set43Aspect(enabled); }
   void setScalingFactor(float factor);

protected:
   void changeEvent(QEvent* e);
   void mousePressEvent(QMouseEvent* event);
   void mouseReleaseEvent(QMouseEvent* event);
   void keyPressEvent(QKeyEvent* event);
   void keyReleaseEvent(QKeyEvent* event);

signals:
   void controllerInput(uint32_t* joy);

private:
   Ui::NESEmulatorDockWidget *ui;
   CNESEmulatorRenderer* renderer;
   char* imgData;
   uint32_t m_joy [ NUM_CONTROLLERS ];
   QRect ncRect;

private slots:
   void renderData();
   void updateTargetMachine(QString target);
};

#endif // NESEMULATORDOCKWIDGET_H
