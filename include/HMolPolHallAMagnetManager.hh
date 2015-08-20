#ifndef HMOLPOLHALLAMAGNETMANAGER_HH
#define HMOLPOLHALLAMAGNETMANAGER_HH

class G4QuadrupoleMagField;
class G4VPhysicalVolume;

#include <G4String.hh>

class HMolPolHallAMagnetManager {

  public:

    HMolPolHallAMagnetManager();
    virtual ~HMolPolHallAMagnetManager();

    void AddMagnet(G4String name, G4VPhysicalVolume *vol);

    static bool ManagerExists() {
      return gHallAMagnetManager;
    }

    void SetBeamEnergy(G4double beamEnergy);
    void InitializeMagneticFields();

    static HMolPolHallAMagnetManager* GetManager() {
      if(!gHallAMagnetManager)
        gHallAMagnetManager = new HMolPolHallAMagnetManager();

      return gHallAMagnetManager;
    }

  private:
    // Global singleton instance of Hall A magnet manager
    static HMolPolHallAMagnetManager *gHallAMagnetManager;

    G4double fBeamEnergy;
    G4QuadrupoleMagField *fQuad1;
    G4QuadrupoleMagField *fQuad2;
    G4QuadrupoleMagField *fQuad3;
    G4VPhysicalVolume *fQuad1Volume;
    G4VPhysicalVolume *fQuad2Volume;
    G4VPhysicalVolume *fQuad3Volume;

    // Configure Quad Magnetic Field
    void ConfigureQuadMagneticField(G4double gradiant, G4VPhysicalVolume *vol);
};

#endif // HMOLPOLHALLAMAGNETMANAGER_HH
