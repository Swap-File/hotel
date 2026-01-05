#pragma once

/* AM2302 library by Alex Brick
 */

// This will load the definition for common Particle variable types
#include "Particle.h"

#define HIGH_TIME_FOR_0 30 //microseconds
#define HIGH_TIME_FOR_1 70 //microseconds
#define BITS_TO_READ 40

class AM2302 {
  public:
    /**
     * AM2302
     * Constructs a new AM2302
     * Params: dataPin - I/O pin used for communicaiton with the device
     * Returns: new AM2302
     **/
    AM2302(int dataPin);
    /**
     * sample
     * Takes a reading from the attached AM2302
     * Params: void
     * Returns: bool - success/failure of the reading
     **/
    bool sample(void);
    /**
     * getHumidity
     * Returns the most recent humidity reading
     * Params: void
     * Returns: double - Relative humidity as a percentage
     **/
    double getHumidity(void);
    /**
     * getTemp
     * Returns the most recent temperature reading
     * Params: void
     * Returns: double - Temperature in degress Celsius
     **/
    double getTemp(void);
    /**
     * dataIsGood
     * Tells you if the last reading was good or not
     * Params: void
     * Returns: bool - True if data matches the checksum, false otherwise
     **/
    bool dataIsGood(void);
  private:
    int DATA_PIN;
    int humidity;
    int temp;
    int checksum;
};
