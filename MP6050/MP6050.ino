    #define RX 2      // Arduino RX line on pin D2
    #define TX 3      // Arduino TX line on pin D3

    #define MPU 0x68  // I2C address of the MPU-6050

    #define Accl_x_Offset +800
    #define Accl_y_Offset -180
    #define Accl_z_Offset -1800

    #define Gyro_x_Offset -306
    #define Gyro_y_Offset -96
    #define Gyro_z_Offset +12

    #define MPU6050_D3 3
    #define MPU6050_D4 4
    #define MPU6050_AFS_SEL0   MPU6050_D3
    #define MPU6050_AFS_SEL1   MPU6050_D4     
    #define MPU6050_AFS_SEL_3 (bit(MPU6050_AFS_SEL1)|bit(MPU6050_AFS_SEL0))
    #define MPU6050_AFS_SEL_16G MPU6050_AFS_SEL_3

    #include <Wire.h>

    unsigned long New_Time = 0, Old_Time = 0;
    float Loop_Time;

    int Temp_Raw;
    int Accl_x_Raw, Accl_y_Raw, Accl_z_Raw;
    int Gyro_x_Raw, Gyro_y_Raw, Gyro_z_Raw;

    float Temp;
    float Accl_x, Accl_y, Accl_z;
    float Gyro_x, Gyro_y, Gyro_z;
    float Angle_x, Angle_y, Angle_z;

    float New_Accl_x = 0.0, New_Accl_y = 0.0, New_Accl_z = 0.0;
    float Old_Accl_x = 0.0, Old_Accl_y = 0.0, Old_Accl_z = 0.0;

    float New_Velo_x = 0.0, New_Velo_y = 0.0, New_Velo_z = 0.0;
    float Old_Velo_x = 0.0, Old_Velo_y = 0.0, Old_Velo_z = 0.0;

    int meas_type = 0;

    float square(float n) {
      return n*n;
    }

    void setup()
    {
      Serial.begin(9600);

      Wire.begin(0,2);
      Wire.beginTransmission(MPU);
      Wire.write(0x6B);  // PWR_MGMT_1 register
      Wire.write(0);     // set to zero (wakes up the MPU-6050)
      Wire.endTransmission();      
      //Wire.beginTransmission(MPU);
      //Wire.write(0x1C);
      //Wire.write(0x03);
      //Wire.endTransmission();      
    }

    void loop()
    {  
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);                         // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission();
      Wire.requestFrom(MPU, 14);                // request a total of 14 registers
      
      // Acquisition of raw values from MPU 6050
      Accl_x_Raw = Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
      Accl_y_Raw = Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
      Accl_z_Raw = Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

      Temp_Raw   = Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H)   & 0x42 (TEMP_OUT_L)

      Gyro_x_Raw = Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H)  & 0x44 (GYRO_XOUT_L)
      Gyro_y_Raw = Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H)  & 0x46 (GYRO_YOUT_L)
      Gyro_z_Raw = Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H)  & 0x48 (GYRO_ZOUT_L)

      // Offset Elimination
      Accl_x_Raw -= Accl_x_Offset;
      Accl_y_Raw -= Accl_y_Offset;
      Accl_z_Raw -= Accl_z_Offset;
      
      Gyro_x_Raw -= Gyro_x_Offset;
      Gyro_y_Raw -= Gyro_y_Offset;
      Gyro_z_Raw -= Gyro_z_Offset;

      // Conversion from raw values to Accelerations [g],
      // Temperature [°C] and Rotation Speed [°/sec]
      Accl_x = (float) Accl_x_Raw * 4.0 / 65536.0;
      Accl_y = (float) Accl_y_Raw * 4.0 / 65536.0;
      Accl_z = (float) Accl_z_Raw * 4.0 / 65536.0;
      
      Temp = ((float)Temp_Raw + 521.0) / 340.0 + 29.1;  // Default value = + 35
      
      Gyro_x = (float) Gyro_x_Raw * 500.0 / 65536.0;
      Gyro_y = (float) Gyro_y_Raw * 500.0 / 65536.0;
      Gyro_z = (float) Gyro_z_Raw * 500.0 / 65536.0;
      
      // Conversion from acceleration to angle
      Angle_x = atan (Accl_x / sqrt (square (Accl_y) + square (Accl_z))) * 180/PI;
      Angle_y = atan (Accl_y / sqrt (square (Accl_x) + square (Accl_z))) * 180/PI;
      Angle_z = atan (Accl_z / sqrt (square (Accl_x) + square (Accl_y))) * 180/PI;
      
      // Conversion of Accelerations from g to (m/s)/s
      
      Accl_x *= 9.81;
      Accl_y *= 9.81;
      Accl_z *= 9.81;


      //Serial.print("Accl x: ");
      //Serial.println(Accl_x_Raw);
      //Serial.print("Accl_y: ");
      Serial.println(Accl_y_Raw);
      //Serial.print("Accl_z: ");
      //Serial.println(Accl_z_Raw);      
      
      // Computation of velocity in mm/s

      New_Accl_x = Accl_x;
      New_Accl_y = Accl_y;
      New_Accl_z = Accl_z;

      New_Velo_x = Old_Velo_x + Old_Accl_x * Loop_Time * 1000.0;
      New_Velo_y = Old_Velo_y + Old_Accl_y * Loop_Time * 1000.0;
      New_Velo_z = Old_Velo_z + Old_Accl_z * Loop_Time * 1000.0;

      Old_Accl_x = New_Accl_x;
      Old_Accl_y = New_Accl_y;
      Old_Accl_z = New_Accl_z;

      Old_Velo_x = New_Velo_x;
      Old_Velo_y = New_Velo_y;
      Old_Velo_z = New_Velo_z;

      //delay(500);

    }


