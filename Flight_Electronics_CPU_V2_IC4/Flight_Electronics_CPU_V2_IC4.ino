#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_10DOF.h>

/* Assign a unique ID to the sensors */
Adafruit_10DOF                dof   = Adafruit_10DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);

/* Update this with the correct SLP for accurate altitude measurements */
float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

/**************************************************************************/
/*!
 @brief  Initialises all the sensors used by this example
 */
/**************************************************************************/
void initSensors()
{
  if(!accel.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP180 ... check your connections */
    Serial.println("Ooops, no BMP180 detected ... Check your wiring!");
    while(1);
  }
}

/**************************************************************************/
/*!
 
 */
/**************************************************************************/
void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("Adafruit 10 DOF Pitch/Roll/Heading Example")); 
  Serial.println("");
  
  pinMode(4,OUTPUT);

  /* Initialise the sensors */
  initSensors();
}

/**************************************************************************/
/*!
 @brief  Constantly check the roll/pitch/heading/altitude/temperature
 */
/**************************************************************************/
void loop(void)
{
  Serial.print("{");
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_event_t bmp_event;
  sensors_vec_t   orientation;

  /* Calculate pitch and roll from the raw accelerometer data */
  accel.getEvent(&accel_event);
  mag.getEvent(&mag_event);
  if (dof.fusionGetOrientation(&accel_event, &mag_event, &orientation))
  {
    /* 'orientation' should have valid .roll and .pitch fields */
    Serial.print("\"roll\": ");
    Serial.print(orientation.roll);
    Serial.print(", \"pitch\": ");
    Serial.print(orientation.pitch);
    Serial.print(", \"heading\": ");
    Serial.print(orientation.heading);
    Serial.print(", \"xLin\": ");
    Serial.print(accel_event.acceleration.x);
    Serial.print(", \"yLin\": ");
    Serial.print(accel_event.acceleration.y);
    Serial.print(", \"zLin\": ");
    Serial.print(accel_event.acceleration.z);
  }
  
  /* Calculate the altitude using the barometric pressure sensor */
  bmp.getEvent(&bmp_event);
  if (bmp_event.pressure)
  {
    /* Get ambient temperature in C */
    float temperature;
    bmp.getTemperature(&temperature);
    /* Convert atmospheric pressure, SLP and temp to altitude    */
    Serial.print(", \"alt\": ");
    Serial.print(bmp.pressureToAltitude(seaLevelPressure, bmp_event.pressure, temperature)); //in meters

    /* Display the temperature */
    Serial.print(", \"temp\": ");
    Serial.print(temperature); //in deg C
  }

  Serial.println("}");
  delay(100);
  digitalWrite(4,millis()%1000<100);
}

