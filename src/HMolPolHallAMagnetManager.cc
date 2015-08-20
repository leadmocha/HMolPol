//Geant4 specific includes
#include <G4SystemOfUnits.hh>
#include <G4VPhysicalVolume.hh>
#include <G4QuadrupoleMagField.hh>
#include <G4LogicalVolume.hh>
#include <G4Tubs.hh>
#include <G4FieldManager.hh>

//HMolPol specific includes
#include "HMolPolHallAMagnetManager.hh"

static G4RotationMatrix gIdentityMatrix;

HMolPolHallAMagnetManager* HMolPolHallAMagnetManager::gHallAMagnetManager = 0;

HMolPolHallAMagnetManager::HMolPolHallAMagnetManager() :
  fBeamEnergy(0.0),fQuad1(0),fQuad2(0),fQuad3(0)
{
}

HMolPolHallAMagnetManager::~HMolPolHallAMagnetManager()
{

}

void HMolPolHallAMagnetManager::AddMagnet(G4String name, G4VPhysicalVolume *vol)
{
  if(name == "Q1")
  {
    fQuad1Volume = vol;
    return;
  }else if(name == "Q2")
  {
    fQuad2Volume = vol;
  }else if(name == "Q3")
  {
    fQuad3Volume = vol;
  }
}

void HMolPolHallAMagnetManager::SetBeamEnergy(G4double beamEnergy)
{
  fBeamEnergy = beamEnergy;
}


void HMolPolHallAMagnetManager::InitializeMagneticFields()
{
  G4cout << " In HMolPolHallAMagnetManager::InitializeMagneticFields():"
    << G4endl;

  // First, we work on Quad 1 and Quad3 (Quad2 will not be used for now)

  // This magnet configuration requires at minimum that Q1 and Q3 be defined
  // otherwise it doesn't work.
  if( !fQuad1Volume || !fQuad3Volume) {
    G4cerr << "   Cannot initialize Hall A magnetic configuration."
      << "    we need Quad 1 and 3 to be defined!." << G4endl;
    return;
  }

  // The quadrupoles are defined as cylinders
  G4Tubs *tubeQ1 = dynamic_cast<G4Tubs*>(
      fQuad1Volume->GetLogicalVolume()->GetSolid());

  G4Tubs *tubeQ3 = dynamic_cast<G4Tubs*>(
      fQuad3Volume->GetLogicalVolume()->GetSolid());

  // If Q1 and Q3 are not tubes, then we complain!
  if( !tubeQ1 || !tubeQ3)
  {
    G4cerr << "   Cannot initialize Hall A magnetic configuration."
      << "    Quad1 and Quad3 have to be defined as solids." << G4endl;
    return;
  }

  // To setup the Quads

  // We use the document at
  // http://hallaweb.jlab.org/equipment/moller/magnets/Moller_quads1.pdf
  // to calculate the necessary field gradient for the quadrupoles. Where
  // the max magnetic field is given by:
  //
  // B(Q1) =  0.506 + 1.224*BeamEnergy - 0.2167*(BeamEnergy^2)
  // B(Q3) = -0.125 + 0.461*BeamEnergy + 0.0285*(BeamEnergy^2)
  //
  // I think the units are given in Tesla
  //
  // To get the gradient we divide by the radius of the quadrupole.
  G4double E = fBeamEnergy/CLHEP::GeV;

  //G4double gradQ1 = ( 0.506 + 1.224*E - 0.2167*E*E)/tubeQ1->GetOuterRadius();
  //G4double gradQ3 = (-0.125 + 0.461*E + 0.0285*E*E)/tubeQ3->GetOuterRadius();
  G4double gradQ1 = CLHEP::kilogauss*( 0.506 + 1.224*E - 0.2167*E*E)/
    tubeQ1->GetOuterRadius();
  G4double gradQ3 = CLHEP::kilogauss*(-0.125 + 0.461*E + 0.0285*E*E)/
    tubeQ3->GetOuterRadius();
  G4double gradQ2 = gradQ3;

  // Apparently the equations above give me the wrong sign!
  gradQ1 *= -1.0;
  gradQ2 *= -1.0;
  gradQ3 *= -1.0;

  G4cout << "Computed gradients for beam energy " << E
    << " Q1 = " << gradQ1/(CLHEP::kilogauss/(2.54*CLHEP::cm));
  if(fQuad2Volume)
  {
    G4cout << " Q2 = " << gradQ2/(CLHEP::kilogauss/(2.54*CLHEP::cm));
  }
  G4cout << " Q3 = " << gradQ3/(CLHEP::kilogauss/(2.54*CLHEP::cm)) <<  G4endl;

  // Alright, now configure the magnetic fields for the quads
  ConfigureQuadMagneticField(gradQ1,fQuad1Volume);
  ConfigureQuadMagneticField(gradQ3,fQuad3Volume);

  // If Quad2 is defined, set it up the same as Quad3
  if(fQuad2Volume)
  {
    ConfigureQuadMagneticField(gradQ2,fQuad2Volume);
  }

}

void HMolPolHallAMagnetManager::ConfigureQuadMagneticField(G4double grad,
    G4VPhysicalVolume *vol)
{
  // First get the rotation matrix
  G4RotationMatrix *matrix = vol->GetRotation();
  if(!matrix)
    matrix = &gIdentityMatrix;

  G4QuadrupoleMagField *magField = new G4QuadrupoleMagField(grad,
      vol->GetTranslation(),matrix);

  G4FieldManager *fieldMan = vol->GetLogicalVolume()->GetFieldManager();
  if(!fieldMan)
  {
    fieldMan = new G4FieldManager(magField);
    vol->GetLogicalVolume()->SetFieldManager(fieldMan,true);
  }

  fieldMan->CreateChordFinder(magField);
}
