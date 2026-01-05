#ifndef AM2302_GUARD
    #define AM2302_GUARD
    #include "AM2302.h"
#endif

AM2302::AM2302(int dataPin)
{
    DATA_PIN = dataPin;
}

bool AM2302::sample (void)
{

    // Zero out data
    humidity = 0;
    temp = 0;
    checksum = 0;

    // Signal read start, 1-10ms low, 20-40us high
    pinMode(DATA_PIN, OUTPUT);
    digitalWrite(DATA_PIN, LOW);
    delay(8);
    digitalWrite(DATA_PIN, HIGH);
    delayMicroseconds(30);

    // Get start ack from sensor
    pinMode(DATA_PIN, INPUT);
    delayMicroseconds(40);
    if ( digitalRead(DATA_PIN) == 1 )
        return false;
    delayMicroseconds(80);
    if ( digitalRead(DATA_PIN) == 0 )
        return false;

    //Read the data
    for (int i = 0; i < BITS_TO_READ; i ++){
        //Eat remaning high signal
        while (digitalRead(DATA_PIN) > 0){/*omm nom nom*/}
        //Eat low signal
        while (digitalRead(DATA_PIN) < 1){/*omm nom nom*/}
        //Wait for a spell...
        delayMicroseconds(HIGH_TIME_FOR_0);

        uint16_t reading = digitalRead(DATA_PIN);

        if ( i < 16 ){
            humidity  = humidity << 1;
            humidity |= reading;
        } else if ( i < 32 ) {
            temp  = temp << 1;
            temp |= reading;
        } else {
            checksum  = checksum << 1;
            checksum |= reading;
        }
    }

    //Success!
    return dataIsGood();
}


//=====================================================================================
// helpers
//=====================================================================================
bool AM2302::dataIsGood (void)
{
    return (((temp >> 8) + (0x00FF & temp) + (humidity >> 8) + (0x00FF & humidity)) & 0x00FF) == checksum;
}

//=====================================================================================
// getters and setters
//=====================================================================================
double AM2302::getHumidity(void)
{
    return humidity / 10.0;
}

double AM2302::getTemp(void)
{
    if ( 0x8000 & temp ){
        return (temp & 0x7FFF) * -1.0;
    }
    return temp / 10.0;
}
