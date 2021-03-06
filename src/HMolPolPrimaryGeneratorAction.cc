/*!*******************************************
 * \author <b>Programmer:</b> Valerie Gray
 * \author <b>Assisted By:</b> Wouter Deconinck
 *
 * \brief <b>Purpose:</b> This file is to create the primary generator action.
 * This means that the this generates the initial incoming particle.
 *
 * \details This is invoked at the beginning of each event.  This it generates the
 * primary particles, and its properties (momentum, energy directions, etc.)
 *
 * Also the passing of ariables
 *
 * \date 0<b>Date:</b> 6-25-2013
 * \date <b>Modified:</b> 07-15-2015
 *
 * \note <b>Entry Conditions:</b>
 *
 * \todo add in actual <b> incoming electrons </b> not just the ones after
 * the interaction
 ********************************************/

//Geant4 includes
#include <G4Event.hh>
#include <G4PhysicalConstants.hh>
#include <G4ParticleGun.hh>
#include <G4ThreeVector.hh>
#include <Randomize.hh>
#include <CLHEP/Units/SystemOfUnits.h>

//HMolPol includes
#include "HMolPolPrimaryGeneratorAction.hh"
#include "HMolPolAnalysis.hh"
#include "HMolPolEventPrimary.hh"

/********************************************
 * Programmer: Valerie Gray
 * Function: HMolPolPrimaryGeneratorAction
 *
 * Purpose:
 * Construction of the HMolPolPrimaryGeneratorAction
 *
 * Global:
 * Entry Conditions: HMolPolAnalysis
 * Return: Nothing
 * Called By:
 * Date: 06-25-2013
 * Modified: 07-15-2015
 ********************************************/
/// \todo have someone (Wouter) help me figure what this all does
// Initialize the generator action with the pointer to the analysis object (so
// we have access to the ROOT file if needed), with some initial raster size
// settings, and with an initial beam energy. Commands by the user could still
// change these values from within geant4 through the messenger class.
HMolPolPrimaryGeneratorAction::HMolPolPrimaryGeneratorAction(HMolPolAnalysis* a) :
    fAnalysis(a), fRasterX(4.0 * CLHEP::mm), fRasterY(4.0 * CLHEP::mm),
        fBeamE(200 * CLHEP::MeV)  // initialization
{
  //set number of particles getting fired at a time
  G4int NubofParticles = 1;

  //set a particle gun with n number of particles
  fParticleGun = new G4ParticleGun(NubofParticles);

  /// set the min and max theta angles in the CM frame
  fTheta_com_min = 0.0 * CLHEP::degree;
  fTheta_com_max = 180.0 * CLHEP::degree;
  //set particle type
  // \todo *ADD* in functionality for other particles as incoming ones later
  //SetParticleType("e-");

  //set the beam energy

  return;
}

/********************************************
 * Programmer: Valerie Gray
 * Function: ~HMolPolPrimaryGeneratorAction
 *
 * Purpose:
 * Destruction of the HMolPolPrimaryGeneratorAction
 *
 * Global:
 * Entry Conditions: none
 * Return: nothing
 * Called By:
 * Date: 06-25-2013
 * Modified: 07-06-2013
 ********************************************/
HMolPolPrimaryGeneratorAction::~HMolPolPrimaryGeneratorAction()
{
  if (fParticleGun)
    delete fParticleGun;
}

/********************************************
 * Programmer: Valerie Gray
 * Function: GeneratePrimaries
 *
 * Purpose:
 *
 * Sets up and generates the primaries via Moller scattering right now
 *
 * Global:
 * Entry Conditions: G4Event
 * Return:
 * Called By:
 * Date: 06-25-2013
 * Modified: 07-15-2015
 ********************************************/
void HMolPolPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  /*
   //debugging
   G4cout << G4endl << "#### Entering HMolPolPrimaryGeneratorAction::GeneratePrimaries ####"<< G4endl;
   */

  //All the following is from remoll - I have no idea why I am using it or what
  // it does the following
  /// \todo have someone (Wouter) help me figure what this all does
  // Based on a random value for theta and phi (chosen from a flat distribution
  // with CLHEP::RandFlat::shoot), we calculate the kinematics, the asymmetry
  // and the cross section for this event.
  /**********
   * Generate Moller event
   ***********/
  // TODO \todo Make it so the beam of energy is passed to this - not hard coded
  // This needs to get passed globally and be a variable in the GUI
  //beam energy
  double beamE = fBeamE;
  // mass of Electron
  double m_e = electron_mass_c2;  // MeV

  /* get rid of unused parameter warning
   double Ecm = sqrt(2*pow(m_e,2) + 2*m_e*beamE); //Energy in the CM frame
   double Pcm = sqrt(pow(Ecm,2) - 4*pow(m_e,2));  // mometum in the CM frame
   */

  //find the beta and gamma value in Center of Mass
  double beta_com = std::sqrt((beamE - m_e) / (beamE + m_e)); //unitless
  double gamma_com = 1.0 / std::sqrt(1.0 - beta_com * beta_com);  //unitless

  //CM energy and random theta, and phi angle
  double e_com = m_e * gamma_com;  //MeV
  double theta_com = acos(
      CLHEP::RandFlat::shoot(cos(fTheta_com_max), cos(fTheta_com_min)));  //rad
  double phi_com = CLHEP::RandFlat::shoot(0.0, 2.0*pi);  //rad
  //debugging
  //double phi_com = 0;

  /*******
   * the cross section
   * this is *NOT* the from
   * (formula (2) and (7) in J. Arrington et al, 1992,
   * A Moller Polarimeter for the MIT-Bates Storage Ring)
   * double D_sigma = fine_structure_const*fine_structure_const *
   *     pow(3.0+cos(theta_com)*cos(theta_com),2.0) *
   *     hbarc*hbarc/pow(sin(theta_com),4.0)/(2.0*m_e*beamE); // units of mbarn
   *
   * This is more like Qweak... but they have a odd factor in it
   * / \see Using the formula (2) from J. Arrington et al, 1992,
   *   A Moller Polarimeter for the MIT-Bates Storage Ring
   * This is the cross section in the lab fram not the CM frame
   * This D_sigma is really d_sigma/d_omega
   ********/
  double D_sigma = pow(hbarc * fine_structure_const / (2 * m_e), 2)
      * pow((1 + cos(theta_com)) * (3.0 + pow(cos(theta_com), 2.0)), 2.0)
                   / pow(sin(theta_com), 4.0);  // units of mbarns

  /* get rid of unused parameter warning
  //This is the Detla sigma one needs to get the delta_sigma (cross section)
  // for a Certain range of theta covered
  double Delta_Omega = 2.0*pi*(cos(fTheta_com_min) - cos(fTheta_com_max));
  */

  //More of I have no idea what this is doing
  //  Multiply by Z because we have Z electrons
  //  here we must also divide by two because we are double covering
  //  phasespace because of identical particles

  //evt->SetEffCrossSection(sigma*V*vert->GetMaterial()->GetZ()/2.0);

//  if( vert->GetMaterial()->GetNumberOfElements() != 1 ){
//    G4cerr << __FILE__ << " line " << __LINE__ <<
//        ": Error!  Some lazy remoll programmer didn't account for "
//        "complex materials in the moller process!" << G4endl;
//    exit(1);
//  }

  /*! Using the formula (3) from
   \see J. Arrington et al, 1992,
   //  A Moller Polarimeter for the MIT-Bates Storage Ring */
  // the A_zz is the analyzing power
//  double A_zz = - ( (pow(sin(theta_com),2.0), 2.0)
//      * ( 7 + pow(cos(theta_com),2.0)) )
//          / pow(3 +pow(cos(theta_com),2.0),2.0);
  //G4double APV = m_e *beamE*GF*4.0*sin(thcom)*sin(thcom)*
  //  QWe/(sqrt(2.0)*pi*alpha*pow(3.0+cos(thcom)*cos(thcom),2.0));
  //from remoll we are not use APV
  //evt->SetAsymmetry(APV);
//  evt->SetAsymmetry(A_zz);
//  evt->SetThCoM(thcom);
  //what is all of this for?? I have no idea
  // Q2 is 0 - What??
  //evt->SetQ2( 2.0*e_com*e_com*(1.0-cos(thcom)) );
  // Q2 is not actually well defined
//  evt->SetQ2( 0.0 );
  // Parallel (ppar) and perpendicular (pperp) momentum components
  // in the lab frame
  double pperp = e_com * sin(theta_com);  //MeV
  double ppar = e_com * cos(theta_com);   //MeV

  // \todo *CHANGE* this right now, right now it is HARDCODED which is BAD :(

  // 1. uniform z distribution between zmin = -20 cm and zmax = + 20 cm
  //    (can be changed by messenger class) use G4UniformRand()
  // 2. uniform x,y distribution for raster
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName =
      "e-");
  fParticleGun->SetParticleDefinition(particle);

// **ALL** of these need better variable names and ability to get passed
  // determine x,y position of event (ie raster??)
  G4double interaction_vertex_x_min = -fRasterX / 2;  //*MAKE GLOBAL*
  G4double interaction_vertex_x_max = +fRasterX / 2;  //*MAKE GLOBAL*
  G4double interaction_vertex_x = interaction_vertex_x_min
      + (interaction_vertex_x_max - interaction_vertex_x_min) * G4UniformRand();

  G4double interaction_vertex_y_min = -fRasterY / 2;  //*MAKE GLOBAL*
  G4double interaction_vertex_y_max = +fRasterY / 2;  //*MAKE GLOBAL*
  G4double interaction_vertex_y = interaction_vertex_y_min
      + (interaction_vertex_y_max - interaction_vertex_y_min) * G4UniformRand();

  // determine z position of event
  G4double interaction_vertex_z_min = -22.5 * CLHEP::cm;  //half length of H target - *MAKE GLOBAL*
  G4double interaction_vertex_z_max = +22.5 * CLHEP::cm;  //half length of H target - *MAKE GLOBAL*
  G4double interaction_vertex_z = interaction_vertex_z_min
      + (interaction_vertex_z_max - interaction_vertex_z_min) * G4UniformRand();
  fParticleGun->SetParticlePosition(
      G4ThreeVector(interaction_vertex_x, interaction_vertex_y,
                    interaction_vertex_z));

  // First Moller electron
  double interaction_vertex_mom_x1 = pperp * cos(phi_com);  //MeV
  double interaction_vertex_mom_y1 = pperp * sin(phi_com);  //MeV
  double interaction_vertex_mom_z1 = gamma_com * (ppar + e_com * beta_com);  // boosted MeV
  double theta_lab1 = std::atan2(interaction_vertex_mom_z1, pperp);  //rad
  fParticleGun->SetParticleMomentumDirection(
      G4ThreeVector(interaction_vertex_mom_x1, interaction_vertex_mom_y1,
                    interaction_vertex_mom_z1));

  // Kinetic energy of the particle = total energy - m = sqrt(p^2 + m^2) - m
  double E_kin1 =
      sqrt(
          interaction_vertex_mom_x1 * interaction_vertex_mom_x1 + interaction_vertex_mom_y1
              * interaction_vertex_mom_y1
          + interaction_vertex_mom_z1 * interaction_vertex_mom_z1 + m_e * m_e)
      - m_e; //MeV
  fParticleGun->SetParticleEnergy(E_kin1);
  // finally : fire in the hole!!!
  fParticleGun->GeneratePrimaryVertex(anEvent);

  // Second Moller electron
  double interaction_vertex_mom_x2 = -pperp * cos(phi_com);  //MeV
  double interaction_vertex_mom_y2 = -pperp * sin(phi_com);  //MeV
  double interaction_vertex_mom_z2 = gamma_com * (-ppar + e_com * beta_com);  // boosted MeV
  double theta_lab2 = std::atan2(interaction_vertex_mom_z2, pperp);  //rad
  fParticleGun->SetParticleMomentumDirection(
      G4ThreeVector(interaction_vertex_mom_x2, interaction_vertex_mom_y2,
                    interaction_vertex_mom_z2));
  // Kinetic energy of the particle = total energy - m = sqrt(p^2 + m^2) - m
  double E_kin2 =
      sqrt(
          interaction_vertex_mom_x2 * interaction_vertex_mom_x2 + interaction_vertex_mom_y2
              * interaction_vertex_mom_y2
          + interaction_vertex_mom_z2 * interaction_vertex_mom_z2 + m_e * m_e)
      - m_e; //MeV
  fParticleGun->SetParticleEnergy(E_kin2);

  // finally : fire in the hole!!!
  fParticleGun->GeneratePrimaryVertex(anEvent);

  // Store cross section in ROOT file

  //Store the interaction point info to the ROOT file
  fAnalysis->fPrimary->fInteractionVertexPositionX = interaction_vertex_x;
  fAnalysis->fPrimary->fInteractionVertexPositionY = interaction_vertex_y;
  fAnalysis->fPrimary->fInteractionVertexPositionZ = interaction_vertex_z;
  fAnalysis->fPrimary->fInteractionVertexMomentumX = interaction_vertex_mom_x1;  //MeV
  fAnalysis->fPrimary->fInteractionVertexMomentumY = interaction_vertex_mom_y1;  //MeV
  fAnalysis->fPrimary->fInteractionVertexMomentumZ = interaction_vertex_mom_z1;  //MeV
  fAnalysis->fPrimary->fInteractionVertexMomentumX = interaction_vertex_mom_x2;  //MeV
  fAnalysis->fPrimary->fInteractionVertexMomentumY = interaction_vertex_mom_y2;  //MeV
  fAnalysis->fPrimary->fInteractionVertexMomentumZ = interaction_vertex_mom_z2;  //Mev

  //Store the scattering angles for the interaction
  //in CM and Lab frame in ROOT file
  fAnalysis->fPrimary->fThetaCenterOfMass = theta_com;  //rad
  fAnalysis->fPrimary->fPhiCenterOfMass = phi_com;  //rad
  fAnalysis->fPrimary->fThetaLab1 = theta_lab1;  //rad
  fAnalysis->fPrimary->fThetaLab2 = theta_lab2;  //rad
  fAnalysis->fPrimary->fPhiLab1 = phi_com;  //rad
  fAnalysis->fPrimary->fPhiLab2 = (phi_com + pi < 2*pi) ? (phi_com + pi) : (phi_com - pi);  //rad

  //store cross section info in the ROOT file
  /// \todo actually write the cross section
  fAnalysis->fPrimary->fCrossSection = D_sigma;

  G4cout << "#### Leaving HMolPolPrimaryGeneratorAction::GeneratePrimaries ####"
         << G4endl;

  return;
}
