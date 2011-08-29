#include "codedataloggerdockwidget.h"
#include "ui_codedataloggerdockwidget.h"

#include "dbg_cnes6502.h"
#include "dbg_cnesppu.h"

#include "main.h"

CodeDataLoggerDockWidget::CodeDataLoggerDockWidget(QWidget *parent) :
    CDebuggerBase(parent),
    ui(new Ui::CodeDataLoggerDockWidget)
{
   int i;

   ui->setupUi(this);
   cpuImgData = new char[256*256*4];
   ppuImgData = new char[256*256*4];

   // Clear images...
   for ( i = 0; i < 256*256*4; i+=4 )
   {
      cpuImgData[i] = 0;
      cpuImgData[i+1] = 0;
      cpuImgData[i+2] = 0;
      cpuImgData[i+3] = 0xFF;
      ppuImgData[i] = 0;
      ppuImgData[i+1] = 0;
      ppuImgData[i+2] = 0;
      ppuImgData[i+3] = 0xFF;
   }
   C6502DBG::CodeDataLoggerInspectorTV ( (int8_t*)cpuImgData );
   CPPUDBG::CodeDataLoggerInspectorTV ( (int8_t*)ppuImgData );

   QObject::connect ( emulator, SIGNAL(cartridgeLoaded()), this, SLOT(renderData()));
   QObject::connect ( emulator, SIGNAL(emulatorReset()), this, SLOT(renderData()) );
   QObject::connect ( emulator, SIGNAL(emulatorPaused(bool)), this, SLOT(renderData()) );
   QObject::connect ( breakpointWatcher, SIGNAL(breakpointHit()), this, SLOT(renderData()) );

   renderer = new CCodeDataLoggerRenderer(ui->frame,cpuImgData);
   ui->frame->layout()->addWidget(renderer);
   ui->frame->layout()->update();
}

CodeDataLoggerDockWidget::~CodeDataLoggerDockWidget()
{
   delete ui;
   delete cpuImgData;
   delete ppuImgData;
}

void CodeDataLoggerDockWidget::changeEvent(QEvent* e)
{
   CDebuggerBase::changeEvent(e);

   switch (e->type())
   {
      case QEvent::LanguageChange:
         ui->retranslateUi(this);
         break;
      default:
         break;
   }
}

void CodeDataLoggerDockWidget::showEvent(QShowEvent* event)
{
   QObject::connect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
   renderData();
}

void CodeDataLoggerDockWidget::hideEvent(QHideEvent* event)
{
   QObject::disconnect ( emulator, SIGNAL(updateDebuggers()), this, SLOT(renderData()) );
}

void CodeDataLoggerDockWidget::renderData()
{
   switch ( ui->displaySelect->currentIndex() )
   {
      case 0:
         C6502DBG::RENDERCODEDATALOGGER();
         break;
      case 1:
         CPPUDBG::RENDERCODEDATALOGGER();
         break;
   }

   renderer->updateGL ();
}

void CodeDataLoggerDockWidget::resizeEvent(QResizeEvent* event)
{
   QDockWidget::resizeEvent(event);
   updateScrollbars();
}

void CodeDataLoggerDockWidget::on_zoomSlider_valueChanged(int value)
{
   renderer->changeZoom(value);
   ui->zoomValueLabel->setText(QString::number(value).append("%"));
   updateScrollbars();
}

void CodeDataLoggerDockWidget::updateScrollbars()
{
   int value = ui->zoomSlider->value();
   int viewWidth = (float)256 * ((float)value / 100.0f);
   int viewHeight = (float)256 * ((float)value / 100.0f);
   ui->horizontalScrollBar->setMaximum(viewWidth - renderer->width() < 0 ? 0 : ((viewWidth - renderer->width()) / ((float)value / 100.0f)) + 1);
   ui->verticalScrollBar->setMaximum(viewHeight - renderer->height() < 0 ? 0 : ((viewHeight - renderer->height()) / ((float)value / 100.0f)) + 1);
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->scrollY = ui->verticalScrollBar->value();
}

void CodeDataLoggerDockWidget::on_horizontalScrollBar_valueChanged(int value)
{
   renderer->scrollX = ui->horizontalScrollBar->value();
   renderer->repaint();
}

void CodeDataLoggerDockWidget::on_verticalScrollBar_valueChanged(int value)
{
   renderer->scrollY = ui->verticalScrollBar->value();
   renderer->repaint();
}

void CodeDataLoggerDockWidget::on_displaySelect_currentIndexChanged(int index)
{
   switch ( index )
   {
      case 0:
         renderer->changeImage(cpuImgData);
         break;
      case 1:
         renderer->changeImage(ppuImgData);
         break;
   }

   renderer->repaint();
}

//CDL FORMAT
//xPdcAADC
//C  = Whether it was accessed as code.
//D  = Whether it was accessed as data.
//AA = Into which ROM bank it was mapped when last accessed:
//        00 = $8000-$9FFF        01 = $A000-$BFFF
//        10 = $C000-$DFFF        11 = $E000-$FFFF
//c  = Whether indirectly accessed as code.
//        (e.g. as the destination of a JMP ($nnnn) instruction)
//d  = Whether indirectly accessed as data.
//        (e.g. as the destination of an LDA ($nn),Y instruction)
//P  = If logged as PCM audio data.
//x  = unused.

void CodeDataLoggerDockWidget::on_exportData_clicked()
{
   QString fileName = QFileDialog::getSaveFileName(NULL,"Export Code/Data Log",QDir::currentPath(),"Code+Data Log File (*.cdl)");
   int addr;
   int size = nesGetPRGROMSize();
   int byte;
   QByteArray cdls;

   if ( !fileName.isEmpty() )
   {
      QFile file(fileName);

      if ( file.open(QIODevice::ReadWrite|QIODevice::Truncate) )
      {
         for ( addr = 0; addr < size; addr += MEM_8KB )
         {
            CCodeDataLogger* pLogger = nesGetPhysicalPRGROMCodeDataLoggerDatabase(addr);
            LoggerInfo* pEntry;
            unsigned char cdl;

            cdls.clear();
            for ( byte = 0; byte < MEM_8KB; byte++ )
            {
               cdl = 0x00;
               pEntry = pLogger->GetLogEntry(byte);
               if ( pEntry->count )
               {
                  if ( (pEntry->type == eLogger_InstructionFetch) ||
                       (pEntry->type == eLogger_OperandFetch) )
                  {
                     cdl |= 0x01;
                  }
                  else if ( (pEntry->type == eLogger_DataRead) ||
                            (pEntry->type == eLogger_DataWrite) ||
                            (pEntry->type == eLogger_DMA) )
                  {
                     cdl |= 0x02;
                  }
                  cdl |= ((pEntry->cpuAddr>>SHIFT_64KB_8KB)&0x3)<<2;
                  // No information available (yet) to fill in the indirect code use bit.
                  // No information available (yet) to fill in the indirect data use bit.
                  if ( (pEntry->type == eLogger_DMA) &&
                       (pEntry->source == eSource_APU) )
                  {
                     cdl |= 0x40;
                  }
               }
               cdls.append(cdl);
            }
            file.write(cdls);
         }
         file.close();
      }
   }
}
