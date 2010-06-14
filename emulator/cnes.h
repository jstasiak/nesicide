// cnes.h: interface for the CNES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined ( NES_H )
#define NES_H

#include "cbreakpointinfo.h"
#include "ctracer.h"

#include "cnesicidecommon.h"

// Identifiers of tooltip info to provide.
#define TOOLTIP_BYTES 0
#define TOOLTIP_INFO  1

// Video modes.
#define MODE_NTSC 0
#define MODE_PAL  1

class CNES
{
public:
	CNES();
	virtual ~CNES();

   static inline void VIDEOMODE ( int mode ) { m_videoMode = mode; }
   static inline int VIDEOMODE ( void ) { return m_videoMode; }

   static void BREAKPOINTS ( bool enable );
   static void CLEAROPCODEMASKS ( void );

   static void RESET ( UINT mapper );
   static void RUN ( unsigned char* joy );
   static void REPLAY ( bool enable ) { m_bReplay = enable; }
   static bool REPLAY () { return m_bReplay; }
   static unsigned int FRAME () { return m_frame; }

   static inline CTracer* TRACER ( void ) { return &m_tracer; }

   static CBreakpointInfo* BREAKPOINTS ( void ) { return &m_breakpoints; }
   static void CHECKBREAKPOINT ( eBreakpointTarget target, eBreakpointType type = (eBreakpointType)-1, int data = 0, int event = 0 );
   static void FORCEBREAKPOINT ( void );
   static bool ATBREAKPOINT ( void ) { return m_bAtBreakpoint; }
   static void CLEARBREAKPOINT ( void ) { m_bAtBreakpoint = false; }
   static void STEPCPUBREAKPOINT ( void ) { m_bStepCPUBreakpoint = true; }
   static void STEPPPUBREAKPOINT ( void ) { m_bStepPPUBreakpoint = true; }

   static char* DISASSEMBLY ( UINT addr );
   static UINT SLOC2ADDR ( unsigned short sloc );
   static unsigned short ADDR2SLOC ( UINT addr );
   static unsigned int SLOC ( UINT addr );
   static unsigned char _MEM ( UINT addr );
   static void DISASSEMBLE ( void );
   static void CODEBROWSERTOOLTIP ( int tipType, UINT addr, char* tooltipBuffer );

protected:
   static bool         m_bReplay;
   static unsigned int m_frame;
   static CBreakpointInfo m_breakpoints;
   static bool            m_bAtBreakpoint;
   static bool            m_bStepCPUBreakpoint;
   static bool            m_bStepPPUBreakpoint;

   static int             m_videoMode;

   static CTracer         m_tracer;
};

#endif
