#ifndef CCC65INTERFACE_H
#define CCC65INTERFACE_H

#include <QProcess>

#include "stdint.h"

#include "dbginfo.h"

class CCC65Interface : public QObject
{
   Q_OBJECT
public:
   CCC65Interface();
   virtual ~CCC65Interface();
   static void clear();
   static bool createMakefile();
   static void clean();
   static bool assemble();
   static bool captureDebugInfo();
   static int checkDebugInfo();
   static bool captureINESImage();
   static QStringList getSourceFiles();
   static unsigned int getSourceFileModificationTime(QString sourceFile);
   static QStringList getSymbolsForSourceFile(QString sourceFile);
   static int getSymbolMatchCount(QString symbol);
   static cc65_symbol_type getSymbolType(QString symbol,int index = 0);
   static unsigned int getSymbolAddress(QString symbol,int index = 0);
   static unsigned int getSymbolAbsoluteAddress(QString symbol,int index = 0);
   static unsigned int getSymbolSegment(QString symbol,int index = 0);
   static QString getSymbolSegmentName(QString symbol, int index = 0);
   static unsigned int getSymbolIndexFromSegment(QString symbol,int segment);
   static unsigned int getSymbolSize(QString symbol,int index = 0);
   static int getSourceLineFromFileAndSymbol(QString file,QString symbol);
   static QString getSourceFileFromSymbol(QString symbol);
   static QString getSourceFileFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static int getSourceLineFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static int getLineMatchCount(QString file,int source_line);
   static unsigned int getAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   static unsigned int getAbsoluteAddressFromFileAndLine(QString file,int source_line,int entry = -1);
   static unsigned int getEndAddressFromAbsoluteAddress(uint32_t addr,uint32_t absAddr);
   static bool isAbsoluteAddressAnOpcode(uint32_t absAddr);
   static QStringList getErrors() { return errors; }
   static bool isErrorOnLineOfFile(QString file,int source_line);
   static bool isStringASymbol(QString string);

   static QStringList getCLanguageSourcesFromProject();
   static QStringList getAssemblerSourcesFromProject();

protected:
   static cc65_dbginfo        dbgInfo;
   static const cc65_libraryinfo*   dbgLibraries;
   static const cc65_moduleinfo*    dbgModules;
   static const cc65_scopeinfo*     dbgScopes;
   static const cc65_spaninfo*      dbgSpans;
   static const cc65_sourceinfo*    dbgSources;
   static const cc65_segmentinfo*   dbgSegments;
   static const cc65_lineinfo*      dbgLines;
   static const cc65_symbolinfo*    dbgSymbols;
   static QStringList         errors;
};

#endif // CCC65INTERFACE_H
