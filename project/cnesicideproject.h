#ifndef CNESICIDEPROJECT_H
#define CNESICIDEPROJECT_H

#include "cprojectbase.h"
#include "ccartridge.h"
#include "cproject.h"

#include <QString>
#include <QMessageBox>
#include <QDomDocument>

typedef QColor CPaletteEntry;

class CNesicideProject : public CProjectBase
{
   Q_OBJECT
public:
   // Global Constructor / Destructors
   CNesicideProject();
   virtual ~CNesicideProject();

   // Search functions
   int findSource ( char* objname, char** objdata, int* size );

   // Helper functions
   void initializeProject();
   void terminateProject();
   bool createProjectFromRom(QString fileName,bool silent=false);
   void setDirty(bool dirty) { m_isDirty = dirty; }
   bool isDirty() const { return m_isDirty; }

   // Member Getters
   bool isInitialized() const { return m_isInitialized; }
   QString getProjectTitle() { return m_projectTitle; }
   QString getProjectOutputBasePath() { return m_projectOutputBasePath; }
   QString getProjectOutputName() { return m_projectOutputName; }
   QString getProjectLinkerOutputBasePath() { return m_projectLinkerOutputBasePath; }
   QString getProjectLinkerOutputName() { return m_projectLinkerOutputName; }
   QString getProjectDebugInfoName() { return m_projectDebugInfoName; }
   QString getProjectCHRROMOutputBasePath() { return m_projectCHRROMOutputBasePath; }
   QString getProjectCHRROMOutputName() { return m_projectCHRROMOutputName; }
   bool    getProjectUsesCHRROM() { return m_projectUsesCHRROM; }
   QString getProjectCartridgeOutputName() { return m_projectCartridgeOutputName; }
   QString getProjectCartridgeSaveStateName() { return m_projectCartridgeSaveStateName; }
   QString getCompilerDefinedSymbols() { return m_compilerDefinedSymbols; }
   QString getCompilerIncludePaths() { return m_compilerIncludePaths; }
   QString getCompilerAdditionalOptions() { return m_compilerAdditionalOptions; }
   QString getAssemblerDefinedSymbols() { return m_assemblerDefinedSymbols; }
   QString getAssemblerIncludePaths() { return m_assemblerIncludePaths; }
   QString getAssemblerAdditionalOptions() { return m_assemblerAdditionalOptions; }
   QString getLinkerConfigFile() { return m_linkerConfigFile; }
   QString getLinkerAdditionalOptions() { return m_linkerAdditionalOptions; }

   QDomDocument& getSaveStateDoc() { return m_saveStateDoc; }

   QList<CPaletteEntry> *getProjectPaletteEntries() { return &m_projectPaletteEntries; }
   CCartridge* getCartridge() { return m_pCartridge; }
   CProject* getProject() { return m_pProject; }

   // Member Setters
   void setProjectTitle(QString value)
   {
      m_projectTitle = value;
      m_projectOutputName = m_projectTitle.toLower().replace(" ","_");
      m_projectLinkerOutputName = m_projectTitle.toLower().replace(" ","_")+".prg";
      m_projectDebugInfoName = m_projectTitle.toLower().replace(" ","_")+".dbg";
      m_projectCHRROMOutputName = m_projectTitle.toLower().replace(" ","_")+".chr";
      m_projectCartridgeOutputName = m_projectTitle.toLower().replace(" ","_")+".nes";
      m_projectCartridgeSaveStateName = m_projectTitle.toLower().replace(" ","_")+".sav";
   }
   void setProjectOutputBasePath(QString value)
   {
      if (!value.isEmpty()) m_projectOutputBasePath = value; else m_projectOutputBasePath=".";
      m_projectLinkerOutputBasePath = m_projectOutputBasePath;
      m_projectCHRROMOutputBasePath = m_projectOutputBasePath;
   }
   void setProjectOutputName(QString value) { m_projectOutputName = value; }
   void setProjectLinkerOutputBasePath(QString value) { m_projectLinkerOutputBasePath = value; }
   void setProjectLinkerOutputName(QString value) { m_projectLinkerOutputName = value; }
   void setProjectDebugInfoName(QString value) { m_projectDebugInfoName = value; }
   void setProjectCHRROMOutputBasePath(QString value) { m_projectCHRROMOutputBasePath = value; }
   void setProjectCHRROMOutputName(QString value) { m_projectCHRROMOutputName = value; }
   void setProjectUsesCHRROM(bool value) { m_projectUsesCHRROM = value; }
   void setProjectCartridgeOutputName(QString value) { m_projectCartridgeOutputName = value; }
   void setProjectCartridgeSaveStateName(QString value) { m_projectCartridgeSaveStateName = value; }
   void setCompilerDefinedSymbols(QString value) { m_compilerDefinedSymbols = value; }
   void setCompilerIncludePaths(QString value) { m_compilerIncludePaths = value; }
   void setCompilerAdditionalOptions(QString value) { m_compilerAdditionalOptions = value; }
   void setAssemblerDefinedSymbols(QString value) { m_assemblerDefinedSymbols = value; }
   void setAssemblerIncludePaths(QString value) { m_assemblerIncludePaths = value; }
   void setAssemblerAdditionalOptions(QString value) { m_assemblerAdditionalOptions = value; }
   void setLinkerConfigFile(QString value) { m_linkerConfigFile = value; }
   void setLinkerAdditionalOptions(QString value) { m_linkerAdditionalOptions = value; }
   void setProjectPaletteEntries(QList<CPaletteEntry> *pProjectPaletteEntries);

   void setSaveStateDoc(QDomDocument doc) { m_saveStateDoc = doc; }

   // IXMLSerializable Interface Implementation
   virtual bool serialize(QDomDocument& doc, QDomNode& node);
   virtual bool deserialize(QDomDocument& doc, QDomNode& node, QString& errors);

   // IProjectTreeViewItem Interface Implmentation
   QString caption() const;
   virtual void contextMenuEvent(QContextMenuEvent*, QTreeView*) {}
   virtual void openItemEvent(CProjectTabWidget*) {}
   virtual bool onCloseQuery()
   {
      return true;
   }
   virtual void onClose() {}
   virtual void saveItemEvent() {}
   virtual bool canChangeName()
   {
      return false;
   }
   virtual bool onNameChanged(QString)
   {
      return true;
   }

private:
   // Is the project initialized?
   bool m_isInitialized;
   bool m_isDirty;

   // Attributes
   // The visible title of the project
   QString m_projectTitle;
   QString m_projectOutputBasePath;
   QString m_projectOutputName;
   QString m_projectLinkerOutputBasePath;
   QString m_projectLinkerOutputName;
   QString m_projectDebugInfoName;
   QString m_projectCHRROMOutputBasePath;
   QString m_projectCHRROMOutputName;
   bool    m_projectUsesCHRROM;
   QString m_projectCartridgeOutputName;
   QString m_projectCartridgeSaveStateName;
   // The toolchain argument strings
   QString m_compilerDefinedSymbols;
   QString m_compilerIncludePaths;
   QString m_compilerAdditionalOptions;
   QString m_assemblerDefinedSymbols;
   QString m_assemblerIncludePaths;
   QString m_assemblerAdditionalOptions;
   QString m_linkerConfigFile;
   QString m_linkerAdditionalOptions;

   // Save state information
   QDomDocument m_saveStateDoc;

   // Contained children
   QList<CPaletteEntry> m_projectPaletteEntries;                 // List of palette entries for the emulator.
   CProject*            m_pProject;
   CCartridge*          m_pCartridge;
};

#endif // CNESICIDEPROJECT_H
