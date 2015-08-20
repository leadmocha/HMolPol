/********************************************
 * \author <b>Programmer:</b> Valerie Gray
 * \author <b>Assisted By:</b> Wouter Deconinck
 *
 * \brief <b>Purpose:</b> This file is a messenger file for HMolPol.
 *
 * \details The messenger file is is the interface between the gui/user and the
 * simulation
 *
 * \date 0<b>Date:</b> 6-27-2013
 * \date <b>Modified:</b> 07-06-2013
 *
 * \note <b>Entry Conditions:</b>
 ********************************************/

//geant4 includes
#include <G4UImessenger.hh>
#include <G4UIcommand.hh>
#include <G4UImanager.hh>

#include <G4RunManager.hh>

#include <G4GDMLParser.hh>
#include <G4VPhysicalVolume.hh>

#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithoutParameter.hh>

//HMolPol includes
#include "HMolPolPrimaryGeneratorAction.hh"
#include "HMolPolAnalysis.hh"
#include "HMolPolMessenger.hh"
#include "HMolPolHallAMagnetManager.hh"

//standard includes
#include <iostream>

/********************************************
 * Programmer: Valerie Gray
 * Function: HMolPolMessenger
 *
 * Purpose:
 * Construction of the HMolPolMessenger
 *
 * Global:
 * Entry Conditions: HMolPolPrimaryGeneratorAction,
 *      HMolPolRunAction,
 *      HMolPolEventAction,
 *      HMolPolAnalysis
 * Return: Nothing
 * Called By:
 * Date: 06-27-2013
 * Modified:
 ********************************************/
HMolPolMessenger::HMolPolMessenger(
    HMolPolPrimaryGeneratorAction* primaryGeneratorAction,
    HMolPolRunAction* runAction,
    HMolPolEventAction* eventAction,
    HMolPolAnalysis* analysis)
: fPrimaryGeneratorAction(primaryGeneratorAction),  //initialize variables
  fRunAction(runAction),
  fEventAction(eventAction),
  fAnalysis(analysis)
{
  //------------------------------------------------------------------------------
  // create a new directory for all the analysis related stuff
  fAnalysisDir = new G4UIdirectory("/HMolPol/Analysis/");
  //What is this directory for
  fAnalysisDir->SetGuidance("Analysis control");

  //ROOT file Name
  fRootFileStemCmd = new G4UIcmdWithAString("/HMolPol/Analysis/RootFileStem",this);
  fRootFileStemCmd->SetGuidance("Set stem of file with output ROOT tree");
  fRootFileStemCmd->SetParameterName("RootFileStem",false);
  fRootFileStemCmd->SetDefaultValue("HMolPol");
  fRootFileStemCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fRootFileNameCmd = new G4UIcmdWithAString("/HMolPol/Analysis/RootFileName",this);
  fRootFileNameCmd->SetGuidance("Set file name of output ROOT tree");
  fRootFileNameCmd->SetParameterName("RootFileName",false);
  fRootFileNameCmd->SetDefaultValue("HMolPol.root");
  fRootFileNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  //------------------------------------------------------------------------------
  // create a new directory for all the Beam related properties
  fBeamDir = new G4UIdirectory("/HMolPol/Beam/");
  //What is this directory for
  fBeamDir->SetGuidance("Beam control");

  //Raster information
  fRasXCmd = new G4UIcmdWithADoubleAndUnit("/HMolPol/Beam/SetRasterSizeX",this);
  fRasXCmd->SetGuidance("Square raster width in x (horizontal)");
  fRasXCmd->SetParameterName("RasterX", false);

  fRasYCmd = new G4UIcmdWithADoubleAndUnit("/HMolPol/Beam/SetRasterSizeY",this);
  fRasYCmd->SetGuidance("Square raster width y (vertical)");
  fRasYCmd->SetParameterName("RasterY", false);

  // Beam Energy
  fBeamECmd = new G4UIcmdWithADoubleAndUnit("/HMolPol/Beam/SetBeamEnergy",this);
  fBeamECmd->SetGuidance("Beam Energy");
  fBeamECmd->SetParameterName("BeamEnergy", false);

  //---------------------------------------------------------------------------
  // Create a new directory for all the magnetic field related properties
  fMagneticDir = new G4UIdirectory("/HMolPol/Magnetic/");
  fMagneticDir->SetGuidance("Control magnetic elements");

  // Initialize magnets
  fInitializeMagFields = new G4UIcmdWithoutParameter(
      "/HMolPol/Magnetic/InitializeMagneticFields",this);
  fInitializeMagFields->SetGuidance("Initializes Magnetic Field");
}

/********************************************
 * Programmer: Valerie Gray
 * Function: ~HMolPolMessenger
 *
 * Purpose:
 * Destructor of the HMolPolMessenger
 *
 * Global:
 * Entry Conditions: none
 * Return: Nothing
 * Called By:
 * Date: 06-27-2013
 * Modified:
 ********************************************/
HMolPolMessenger::~HMolPolMessenger()
{
  //delete naming of Root file stuff
  if (fRootFileStemCmd) delete fRootFileStemCmd;
  if (fRootFileNameCmd) delete fRootFileNameCmd;

  //delete any directories one created
  if (fAnalysisDir)     delete fAnalysisDir;
  if (fBeamDir)         delete fBeamDir;

  //delete the raster info
  if(fRasXCmd)          delete fRasXCmd;
  if(fRasYCmd)          delete fRasYCmd;

  //Delete beam energy info
  if(fBeamECmd)         delete fBeamECmd;

  // Delete magnetic field related commands
  if(fInitializeMagFields)
    delete fInitializeMagFields;
  if(fMagneticDir)      delete fMagneticDir;
}

/********************************************
 * Programmer: Valerie Gray
 * Function: SetNewValue
 *
 * Purpose:
 * To set that new value for a certain command
 *
 * Global:
 * Entry Conditions: G4UIcommand,
 *      G4String
 * Return: Nothing
 * Called By:
 * Date: 06-27-2013
 * Modified:
 ********************************************/
/// \todo have someone (Wouter) help me figure what this all does

// Whenever a command from this messenger is called, for example the command
// /HMolPol/Analysis/RootFileName, then this function is called with as first
// argument the command and as second argument a string with the value that
// was passed, for example SetNewValue(fRootFileNameCmd,"run.root"). It is
// the job of this function to figure out what to do for this command, to
// convert the value to its proper data type (for example string to int), and
// to test whether the value was even an allowed value.

void HMolPolMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{

// change the different possible variables allowed in the messenger

  //------------------------------------------------------------------------------
  //change name of ROOT file stuff - if given
  if (command == fRootFileStemCmd)
  {
    G4cout << "#### Messenger: Setting Analysis "
        "ROOT file stem to " << newValue << G4endl;
    fAnalysis->SetRootFileStem(newValue);
  }
  if (command == fRootFileNameCmd)
  {
    G4cout << "#### Messenger: Setting Analysis "
        "ROOT file name to " << newValue << G4endl;
    fAnalysis->SetRootFileName(newValue);
  }

  //------------------------------------------------------------------------------
  //raster size
  if( command == fRasXCmd )
  {
    G4cout << "#### Messenger: Setting Beam X Raster Size to "
        << newValue << G4endl;
    fPrimaryGeneratorAction->SetRasterX(fRasXCmd->GetNewDoubleValue(newValue));
  }

  if( command == fRasYCmd )
  {
    G4cout << "#### Messenger: Setting Beam Y Raster Size to "
        << newValue << G4endl;
    fPrimaryGeneratorAction->SetRasterY(fRasYCmd->GetNewDoubleValue(newValue));
  }

  //beam Energy
  if( command == fBeamECmd )
  {
    G4cout << "#### Messenger: Setting Beam Energy to "
        << newValue << G4endl;
    fPrimaryGeneratorAction->SetBeamE(fBeamECmd->GetNewDoubleValue(newValue));
    if(HMolPolHallAMagnetManager::ManagerExists())
      HMolPolHallAMagnetManager::GetManager()->SetBeamEnergy(
          fBeamECmd->GetNewDoubleValue(newValue));
  }

  // Initialize Magnetic field
  if( command == fInitializeMagFields )
  {
    G4cout << "#### Messenger: Initializing Magnetic Fields " << G4endl;

    // If Hall A magnets are being used, initialize those.
    if(HMolPolHallAMagnetManager::ManagerExists())
      HMolPolHallAMagnetManager::GetManager()->InitializeMagneticFields();
  }

  return;
}
