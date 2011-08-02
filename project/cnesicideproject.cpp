#include "cnesicideproject.h"
#include "main.h"

#include "cnessystempalette.h"
#include "cdockwidgetregistry.h"

CNesicideProject::CNesicideProject()
{
   // Add node to tree as root
   InitTreeItem("");

   // Allocate children
   m_pProject = new CProject(this);
   m_pCartridge = new CCartridge(this);

   m_isInitialized = false;
   m_isDirty = false;

   m_projectTitle = "(No project loaded)";
   setProjectOutputBasePath(".");
   setProjectOutputName("");
   m_projectUsesCHRROM = true;
}

CNesicideProject::~CNesicideProject()
{
   if ( m_pProject )
   {
      delete m_pProject;
   }
   if ( m_pCartridge )
   {
      delete m_pCartridge;
   }
}

int CNesicideProject::findSource ( char* objname, char** objdata, int* size )
{
   IProjectTreeViewItemIterator iter(nesicideProject->getProject()->getSources());
   CSourceItem* source;

   (*objdata) = NULL;
   (*size) = 0;

   while ( iter.current() != NULL )
   {
      if ( iter.current()->caption() == objname )
      {
         source = dynamic_cast<CSourceItem*>(iter.current());
         if ( source )
         {
            (*objdata) = strdup(source->sourceCode().toAscii().constData());
            (*size) = strlen((*objdata));
         }
         break;
      }

      iter.next();
   }
   return (*size);
}

void CNesicideProject::initializeProject()
{
   // Initialize this node's attributes
   m_projectPaletteEntries.clear();

   for (int col=0; col <= NUM_PALETTES; col++)
   {
      m_projectPaletteEntries.append(QColor(nesGetPaletteRedComponent(col),
                                            nesGetPaletteGreenComponent(col),
                                            nesGetPaletteBlueComponent(col)));
   }

   // Notify the fact that the project data has been initialized properly
   m_projectTitle = "(No project loaded)";
   setProjectOutputBasePath(".");
   setProjectOutputName("");
   m_compilerDefinedSymbols = "";
   m_compilerIncludePaths = "";
   m_compilerAdditionalOptions = "";
   m_assemblerDefinedSymbols = "";
   m_assemblerIncludePaths = "";
   m_assemblerAdditionalOptions = "";
   m_linkerConfigFile = "";
   m_linkerAdditionalOptions = "";

   m_saveStateDoc.clear();

   m_isInitialized = true;
   m_isDirty = false;

   // Initialize child nodes
   m_pProject->initializeProject();
   m_pCartridge->initializeProject();

   // Add child nodes to tree
   appendChild(m_pProject);
   appendChild(m_pCartridge);
}

void CNesicideProject::terminateProject()
{
   // Terminate child nodes
   m_pProject->terminateProject();
   m_pCartridge->terminateProject();

   // Remove child nodes from tree
   removeChild(m_pCartridge);
   removeChild(m_pProject);

   // Notify the fact that the project data is no longer valid
   m_projectTitle = "(No project loaded)";
   setProjectOutputBasePath(".");
   setProjectOutputName("");
   m_compilerDefinedSymbols = "";
   m_compilerIncludePaths = "";
   m_compilerAdditionalOptions = "";
   m_assemblerDefinedSymbols = "";
   m_assemblerIncludePaths = "";
   m_assemblerAdditionalOptions = "";
   m_linkerConfigFile = "";
   m_linkerAdditionalOptions = "";

   m_saveStateDoc.clear();

   m_isInitialized = false;
   m_isDirty = false;
}

bool CNesicideProject::serialize(QDomDocument& doc, QDomNode& node)
{
   // Create a DOM element to define the node this object represents, and add it to the
   // parent node assigned in from the caller.
   QDomElement projectElement = addElement( doc, node, "nesicideproject" );

   // Set some variables as tags to this node.
   projectElement.setAttribute("version", 0.3);
   projectElement.setAttribute("title", m_projectTitle);

   // Create the project configuration node.
   QDomElement propertiesElement = addElement(doc,projectElement,"properties");

   propertiesElement.setAttribute("outputbasepath",m_projectOutputBasePath);
   propertiesElement.setAttribute("outputname",m_projectOutputName);
   propertiesElement.setAttribute("linkeroutputbasepath",m_projectLinkerOutputBasePath);
   propertiesElement.setAttribute("linkeroutputname",m_projectLinkerOutputName);
   propertiesElement.setAttribute("debuginfoname",m_projectDebugInfoName);
   propertiesElement.setAttribute("chrromoutputbasepath",m_projectCHRROMOutputBasePath);
   propertiesElement.setAttribute("chrromoutputname",m_projectCHRROMOutputName);
   propertiesElement.setAttribute("chrrom",m_projectUsesCHRROM);
   propertiesElement.setAttribute("cartridgeoutputname",m_projectCartridgeOutputName);
   propertiesElement.setAttribute("cartridgesavestatename",m_projectCartridgeSaveStateName);
   propertiesElement.setAttribute("compilerdefinedsymbols",m_compilerDefinedSymbols);
   propertiesElement.setAttribute("compilerincludepaths",m_compilerIncludePaths);
   propertiesElement.setAttribute("compileradditionaloptions",m_compilerAdditionalOptions);
   propertiesElement.setAttribute("assemblerdefinedsymbols",m_assemblerDefinedSymbols);
   propertiesElement.setAttribute("assemblerincludepaths",m_assemblerIncludePaths);
   propertiesElement.setAttribute("assembleradditionaloptions",m_assemblerAdditionalOptions);
   propertiesElement.setAttribute("linkerconfigfile",m_linkerConfigFile);
   propertiesElement.setAttribute("linkeradditionaloptions",m_linkerAdditionalOptions);

   QDomElement inspectorsElement = addElement(doc,projectElement,"inspectors");

   SymbolWatchDockWidget* pSymbolInspector = dynamic_cast<SymbolWatchDockWidget*>(CDockWidgetRegistry::getWidget("Symbol Inspector"));
   pSymbolInspector->serialize(doc,inspectorsElement);

   BreakpointDockWidget* pBreakpointInspector = dynamic_cast<BreakpointDockWidget*>(CDockWidgetRegistry::getWidget("Breakpoints"));
   pBreakpointInspector->serialize(doc,inspectorsElement);

   ExecutionVisualizerDockWidget* pExecutionVisualizer = dynamic_cast<ExecutionVisualizerDockWidget*>(CDockWidgetRegistry::getWidget("Execution Visualizer"));
   pExecutionVisualizer->serialize(doc,inspectorsElement);

   // Create the root palette element, and give it a version attribute
   QDomElement rootPaletteElement = addElement( doc, propertiesElement, "palette" );

   // Loop through all palette entries, and for each entry add an <entry /> tag that has the
   // index, as well as the RGB properties of the palette.
   for (int indexOfCurrentPaletteEntry=0; indexOfCurrentPaletteEntry <= NUM_PALETTES; indexOfCurrentPaletteEntry++)
   {
      QDomElement elm = addElement( doc, rootPaletteElement, "entry");
      elm.setAttribute("index", indexOfCurrentPaletteEntry);
      elm.setAttribute("r", m_projectPaletteEntries.at(indexOfCurrentPaletteEntry).red());
      elm.setAttribute("g", m_projectPaletteEntries.at(indexOfCurrentPaletteEntry).green());
      elm.setAttribute("b", m_projectPaletteEntries.at(indexOfCurrentPaletteEntry).blue());
   }

   // Now serialize all child objects
   if (!m_pCartridge->serialize(doc, projectElement))
   {
      return false;
   }

   if (!m_pProject->serialize(doc, projectElement))
   {
      return false;
   }

   return true;
}

bool CNesicideProject::deserialize(QDomDocument& doc, QDomNode& node, QString& errors)
{
   // Read in the DOM element
   QDomElement projectElement = doc.documentElement();

   m_isInitialized = false;

   if (projectElement.isNull())
   {
      return false;
   }

   // For now, error out if the file version is not what we expect it to be. Eventually
   // we need to split up the loader into versions for backwards compatibility.
   if (projectElement.attribute("version", "") != "0.3")
   {
      errors.append("NESICIDE Project files must be version 0.3\n");
      return false;
   }

   // Load our properties. Note that the default value is returned if an attribute is missing.
   // This is the expected behavior.
   m_projectTitle = projectElement.attribute("title","Untitled Project");

   // Initialize the palette.
   for (int color = 0; color < 64; color++)
   {
      m_projectPaletteEntries.append(QColor(nesGetPaletteRedComponent(color),
                                            nesGetPaletteGreenComponent(color),
                                            nesGetPaletteBlueComponent(color)));
   }

   // Now loop through the child elements and process the ones we find
   QDomNode child = projectElement.firstChild();

   do
   {
      if (child.nodeName() == "inspectors")
      {
         SymbolWatchDockWidget* pSymbolInspector = dynamic_cast<SymbolWatchDockWidget*>(CDockWidgetRegistry::getWidget("Symbol Inspector"));
         pSymbolInspector->deserialize(doc,child,errors);

         BreakpointDockWidget* pBreakpointInspector = dynamic_cast<BreakpointDockWidget*>(CDockWidgetRegistry::getWidget("Breakpoints"));
         pBreakpointInspector->deserialize(doc,child,errors);

         ExecutionVisualizerDockWidget* pExecutionVisualizer = dynamic_cast<ExecutionVisualizerDockWidget*>(CDockWidgetRegistry::getWidget("Execution Visualizer"));
         pExecutionVisualizer->deserialize(doc,child,errors);
      }
      else if (child.nodeName() == "properties")
      {
         // Get the properties that are just attributes of the main node.
         QDomElement propertiesElement = child.toElement();

         m_projectOutputBasePath = propertiesElement.attribute("outputbasepath");
         m_projectOutputName = propertiesElement.attribute("outputname");
         m_projectLinkerOutputBasePath = propertiesElement.attribute("linkeroutputbasepath");
         m_projectLinkerOutputName = propertiesElement.attribute("linkeroutputname");
         m_projectDebugInfoName = propertiesElement.attribute("debuginfoname");
         m_projectCHRROMOutputBasePath = propertiesElement.attribute("chrromoutputbasepath");
         m_projectCHRROMOutputName = propertiesElement.attribute("chrromoutputname");
         m_projectUsesCHRROM = propertiesElement.attribute("chrrom").toInt();
         m_projectCartridgeOutputName = propertiesElement.attribute("cartridgeoutputname");
         m_projectCartridgeSaveStateName = propertiesElement.attribute("cartridgesavestatename");
         m_compilerDefinedSymbols = propertiesElement.attribute("compilerdefinedsymbols");
         m_compilerIncludePaths = propertiesElement.attribute("compilerincludepaths");
         m_compilerAdditionalOptions = propertiesElement.attribute("compileradditionaloptions");
         m_assemblerDefinedSymbols = propertiesElement.attribute("assemblerdefinedsymbols");
         m_assemblerIncludePaths = propertiesElement.attribute("assemblerincludepaths");
         m_assemblerAdditionalOptions = propertiesElement.attribute("assembleradditionaloptions");
         m_linkerConfigFile = propertiesElement.attribute("linkerconfigfile");
         m_linkerAdditionalOptions = propertiesElement.attribute("linkeradditionaloptions");

         // Loop through the properties nodes.
         QDomNode property = child.firstChild();
         do
         {
            if ( property.nodeName() == "palette" )
            {
               QDomNode paletteNode = property.firstChild();
               do
               {
                  QDomElement paletteItem = paletteNode.toElement();

                  if (paletteItem.isNull())
                  {
                     return false;
                  }

                  if ((!paletteItem.hasAttribute("index"))
                        || (!paletteItem.hasAttribute("r"))
                        || (!paletteItem.hasAttribute("g"))
                        || (!paletteItem.hasAttribute("b")))
                  {
                     errors.append("Error parsing <nesicidepalette> element.\n");
                     return false;
                  }

                  int nodeIndex = paletteItem.attribute("index").toInt();

                  if ((nodeIndex < 0) || (nodeIndex > NUM_PALETTES))
                  {
                     return false;
                  }

                  m_projectPaletteEntries.replace(nodeIndex,
                                                  QColor(paletteItem.attribute("r").toInt(),
                                                         paletteItem.attribute("g").toInt(),
                                                         paletteItem.attribute("b").toInt()));

               }
               while (!(paletteNode = paletteNode.nextSibling()).isNull());
            }
         } while (!(property = property.nextSibling()).isNull());
      }
      else if (child.nodeName() == "project")
      {
         if (!m_pProject->deserialize(doc,child,errors))
         {
            return false;
         }
      }
      else if (child.nodeName() == "cartridge")
      {
         if (!m_pCartridge->deserialize(doc,child,errors))
         {
            return false;
         }
      }
   }
   while (!(child = child.nextSibling()).isNull());

   m_isInitialized = true;
   return true;
}

QString CNesicideProject::caption() const
{
   return QString("NESICIDE");
}

bool CNesicideProject::createProjectFromRom(QString fileName,bool silent)
{
   CCHRROMBanks* chrRomBanks = getCartridge()->getChrRomBanks();
   CPRGROMBanks* prgRomBanks = getCartridge()->getPrgRomBanks();
   int oldBanks;
   int bankIdx;
   QString str;

   QFile fileIn (fileName);

   if (fileIn.exists() && fileIn.open(QIODevice::ReadOnly))
   {
      QDataStream fs(&fileIn);

      // Check the NES header
      char nesHeader[4] = {'N', 'E', 'S', 0x1A};
      char nesTest[4] = {0, 0, 0, 0};
      fs.readRawData(nesTest,4);

      if (memcmp(nesHeader, nesTest,4))
      {
         // Header check failed, quit
         fileIn.close();
         if (!silent)
         {
            QMessageBox::information(0, "Error", "Invalid ROM format.\nCannot create project.");
         }
         return false;
      }

      // Number of 16 KB PRG-ROM banks
      qint8 numPrgRomBanks;
      fs >> numPrgRomBanks;

      // Convert to 8 KB banks
      numPrgRomBanks <<= 1;

      // Get the number of 8 KB CHR-ROM / VROM banks
      qint8 numChrRomBanks;
      fs >> numChrRomBanks;

      // ROM Control Byte 1:
      // - Bit 0 - Indicates the type of mirroring used by the game
      //   where 0 indicates horizontal mirroring, 1 indicates
      //   vertical mirroring.
      //
      // - Bit 1 - Indicates the presence of battery-backed RAM at
      //   memory locations $6000-$7FFF.
      //
      // - Bit 2 - Indicates the presence of a 512-byte trainer at
      //   memory locations $7000-$71FF.
      //
      // - Bit 3 - If this bit is set it overrides bit 0 to indicate fourscreen
      //   mirroring should be used.
      //
      // - Bits 4-7 - Four lower bits of the mapper number.
      qint8 romCB1;
      fs >> romCB1;

      // First extract the mirror mode
      if ((romCB1&FLAG_MIRROR) == FLAG_MIRROR_VERT)
      {
         m_pCartridge->setMirrorMode(VerticalMirroring);
      }
      else
      {
         m_pCartridge->setMirrorMode(HorizontalMirroring);
      }

      // Now extract the two flags (battery backed ram and trainer)
      m_pCartridge->setBatteryBackedRam(romCB1 & 0x02);
      bool hasTrainer = (romCB1 & 0x04);

      // ROM Control Byte 2:
      //  Bits 0-3 - Reserved for future usage and should all be 0.
      //  Bits 4-7 - Four upper bits of the mapper number.
      qint8 romCB2;
      fs >> romCB2;

      if ( romCB2&0x0F )
      {
         romCB2 = 0x00;
         if (!silent)
         {
            QMessageBox::information(0, "Warning", "Invalid iNES header format.\nSave the project to fix.");
         }
      }

      // Extract the upper four bits of the mapper number
      m_pCartridge->setMapperNumber(((romCB1>>4)&0x0F)|(romCB2&0xF0));

      // Number of 8 KB RAM banks. For compatibility with previous
      // versions of the iNES format, assume 1 page of RAM when
      // this is 0.
      qint8 numRamBanks;
      fs >> numRamBanks;

      if (numRamBanks == 0)
      {
         numRamBanks = 1;
      }

      // Skip the 7 reserved bytes
      qint8 skip;

      for (int i=0; i<7; i++)
      {
         fs >> skip;
      }

      // Extract the trainer (if it exists)
      if (hasTrainer)
      {
         // TODO: Handle trainer. Skipping for now.
         for (int i=0; i<512; i++)
         {
            fs >> skip;
         }
      }

      // Load the PRG-ROM banks (16KB each)
      oldBanks = prgRomBanks->getPrgRomBanks().count();
      bankIdx = 0;
      for (int bank=0; bank<numPrgRomBanks; bank++)
      {
         // Grab either a previously used bank, or a new one
         CPRGROMBank* curBank;
         bool doAppend = (--oldBanks < 0);

         // Initialize the bank into the project banks
         if (doAppend)
         {
            curBank = new CPRGROMBank(nesicideProject->getCartridge()->getPrgRomBanks());
            // This is a new bank
            curBank->setBankIndex(prgRomBanks->getPrgRomBanks().count());
            prgRomBanks->appendChild(curBank);
            prgRomBanks->getPrgRomBanks().append(curBank);
         }
         else
         {
            curBank = prgRomBanks->getPrgRomBanks().at(bankIdx++);
         }

         fs.readRawData((char*)curBank->getBankData(),MEM_8KB);
      }

      // Load the CHR-ROM banks (8KB each)
      oldBanks = chrRomBanks->getChrRomBanks().count();
      bankIdx = 0;
      for (int bank=0; bank<numChrRomBanks; bank++)
      {
         // Grab either a previously used bank, or a new one
         CCHRROMBank* curBank;
         bool doAppend = (--oldBanks < 0);

         // Initialize the bank into the project banks
         if (doAppend)
         {
            curBank = new CCHRROMBank(nesicideProject->getCartridge()->getChrRomBanks());
            // This is a new bank
            curBank->setBankIndex(chrRomBanks->getChrRomBanks().count());
            chrRomBanks->appendChild(curBank);
            chrRomBanks->getChrRomBanks().append(curBank);
         }
         else
         {
            curBank = chrRomBanks->getChrRomBanks().at(bankIdx++);
         }

         fs.readRawData((char*)curBank->getBankData(),MEM_8KB);
      }

      str = "<b>Searcing internal game database: ";
      str += gameDatabase.getGameDBAuthor();
      str += ", ";
      str += gameDatabase.getGameDBTimestamp();
      str += "...</b>";
      generalTextLogger->write(str);
      str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
      str += fileName;
      generalTextLogger->write(str);

      bool gameFoundInDB = gameDatabase.find(m_pCartridge);

      if ( gameFoundInDB )
      {
         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SHA1: ";
         str += gameDatabase.getSHA1();
         generalTextLogger->write(str);

         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Name: ";
         str += gameDatabase.getName();
         generalTextLogger->write(str);

         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Publisher (date): ";
         str += gameDatabase.getPublisher();
         str += " (";
         str += gameDatabase.getDate();
         str += ")";
         generalTextLogger->write(str);

         // Set project title...
         if ( !silent )
         {
            m_projectTitle = gameDatabase.getName();
         }

         // Do NTSC/PAL autodetecting

         // Check cartridge validity?
      }
      else
      {
         str = "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i><font color=\"red\">Not found.</font></i>";
         generalTextLogger->write(str);

         // Set project title...
         if ( !silent )
         {
            m_projectTitle = fileName;
         }
      }

      str = "<b>Game loaded.</b>";
      generalTextLogger->write(str);

      fileIn.close();

      return true;
   }

   return false;
}
