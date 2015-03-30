/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <unistd.h>
#include <QDebug>
#include <QCoreApplication>

#include <hardware/sensors.h>
#include <android-version.h>

char const* getSensorName(int type) {
    switch(type) {
    case SENSOR_TYPE_ACCELEROMETER:
        return "Acc";
    case SENSOR_TYPE_MAGNETIC_FIELD:
        return "Mag";
    case SENSOR_TYPE_ORIENTATION:
        return "Ori";
    case SENSOR_TYPE_PROXIMITY:
        return "Prx";
    case SENSOR_TYPE_TEMPERATURE:
        return "Tmp";
    case SENSOR_TYPE_LIGHT:
        return "Lux";
    case SENSOR_TYPE_GYROSCOPE:
        return "gyro";
    case SENSOR_TYPE_LINEAR_ACCELERATION:
        return "linear accel";
    case SENSOR_TYPE_GRAVITY:
        return "gravity";
    case SENSOR_TYPE_ROTATION_VECTOR:
        return "rotation";
    case SENSOR_TYPE_RELATIVE_HUMIDITY:
        return "humidity";
    case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        return "temp";
    }
    qDebug() <<"sensor type"<< type;
    return "ukn";
}

int main(int argc, char *argv[])
{
    int err;
    struct sensors_poll_device_t* device;
    struct sensors_module_t* module;
  //  const hw_module_t *hwModule;
    int c;
    int selectedSensorType;

    qDebug() << "Android version" << ANDROID_VERSION_MAJOR << ANDROID_VERSION_MINOR ;

    err = hw_get_module(SENSORS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (err != 0) {
        qDebug() << "hw_get_module() failed" << strerror(-err);
        return 0;
    }

    err = sensors_open(&module->common, &device);
    if (err != 0) {
        qDebug() << "sensors_open() failed"<< strerror(-err);
        return 0;
    }

    struct sensor_t const* list;
    int count = module->get_sensors_list(module, &list);
    qDebug() << Q_FUNC_INFO << "number of sensors found:" << count;

    for (int i=0 ; i < count ; i++) {
        qDebug() << list[i].name
                << "\n\tvendor:" << list[i].vendor
                << "\n\tversion:"<< list[i].version
                << "\n\thandle:"<< list[i].handle
                << "\n\ttype:"<< list[i].type
                << "\n\tmaxRange:"<< list[i].maxRange
                << "\n\tresolution:"<< list[i].resolution
                << "\n\tpower:" << list[i].power <<"mA";
            }

    sensors_event_t buffer[16];
    while ((c = getopt(argc, argv, "amoptlg")) != -1) {
        switch (c) {
        case 'a':
            qDebug() << "Use accelerometer";
            selectedSensorType = SENSOR_TYPE_ACCELEROMETER;
            break;
        case 'm':
            qDebug() << "Use magnetometer";
            selectedSensorType = SENSOR_TYPE_MAGNETIC_FIELD;
            break;
        case 'o':
            qDebug() << "Use orientation";
            selectedSensorType = SENSOR_TYPE_ORIENTATION;
            break;
        case 'p':
            qDebug() << "Use proximity";
            selectedSensorType = SENSOR_TYPE_PROXIMITY;
            break;
        case 't':
            qDebug() << "Use temperature";
            selectedSensorType = SENSOR_TYPE_TEMPERATURE;
            break;
        case 'l':
            qDebug() << "Use light";
            selectedSensorType = SENSOR_TYPE_LIGHT;
            break;
        case 'g':
            qDebug() << "Use gyro";
            selectedSensorType = SENSOR_TYPE_GYROSCOPE;
            break;
        default:
            break;
        }
    }
    sleep(5);

     for (int i = 0; i < count; i++) {
         if (list[i].type == selectedSensorType) {
             err = device->activate(device, list[i].handle, 1);
             if (err != 0) {
                 qDebug() << "activate() failed for"<<
                             list[i].name << strerror(-err);
                 return 0;
             }
             device->setDelay(device, list[i].handle, 10000000);
         }
     }

     do {
         int n = device->poll(device, buffer, 16);
         if (n < 0) {
             qDebug() << "poll() failed" << strerror(-err);
             break;
         }

         //  qDebug() << "read events:" << n;
         for (int i = 0 ; i < n ; i++) {
             const sensors_event_t& data = buffer[i];

             if (data.version != sizeof(sensors_event_t)) {
                 qDebug() << "incorrect event version (version="<< data.version
                          <<", expected=" << sizeof(sensors_event_t);
                 break;
             }

             switch(data.type) {
             case SENSOR_TYPE_ACCELEROMETER:
                 if (selectedSensorType == data.type)
                     printf("sensor=%s, time=%lld, value=<%5.1f,%5.1f,%5.1f, status=%i>\n",
                            getSensorName(data.type),
                            data.timestamp,
                            data.acceleration.x,
                            data.acceleration.y,
                            data.acceleration.z,
                            data.magnetic.status);
                 break;
             case SENSOR_TYPE_GYROSCOPE:
                 if (selectedSensorType == data.type)
                     printf("sensor=%s, time=%lld, value=<%5.1f,%5.1f,%5.1f, status=%i>\n",
                            getSensorName(data.type),
                            data.timestamp,
                            data.gyro.x,
                            data.gyro.y,
                            data.gyro.z,
                            data.gyro.status);
                 break;
             case SENSOR_TYPE_MAGNETIC_FIELD:
                 if (selectedSensorType == data.type)
                     printf("sensor=%s, time=%lld, value=<%5.1f,%5.1f,%5.1f, status=%i>\n",
                            getSensorName(data.type),
                            data.timestamp,
                            data.magnetic.x,
                            data.magnetic.y,
                            data.magnetic.z,
                            data.magnetic.status);
                 break;
             case SENSOR_TYPE_ORIENTATION:
                 if (selectedSensorType == data.type)
                     printf("sensor=%s, time=%lld, value=<%5.1f,%5.1f,%5.1f, status=%i>\n",
                            getSensorName(data.type),
                            data.timestamp,
                            data.orientation.azimuth,
                            data.orientation.pitch,
                            data.orientation.roll,
                            data.magnetic.status);
                 break;
             case SENSOR_TYPE_PROXIMITY:
                 if (selectedSensorType == data.type)
                     printf("sensor=%s, time=%lld, value=%f\n",
                            getSensorName(data.type),
                            data.timestamp,
                            data.distance);
                 break;
             case SENSOR_TYPE_TEMPERATURE:
                 if (selectedSensorType == data.type)
                     printf("sensor=%s, time=%lld, value=%f\n",
                            getSensorName(data.type),
                            data.timestamp,
                            data.temperature);
                 break;
             case SENSOR_TYPE_LIGHT:
                 if (selectedSensorType == data.type)
                     printf("sensor=%s, time=%lld, value=%f\n",
                            getSensorName(data.type),
                            data.timestamp,
                            data.light);
                 break;
             default:
                 //                    printf("sensor=%s, time=%lld, value=<%f,%f,%f>\n",
                 //                            getSensorName(data.type),
                 //                            data.timestamp,
                 //                            data.acceleration.x,
                 //                            data.acceleration.y,
                 //                            data.acceleration.z);
                 break;
             }
         }
     } while (1); // fix that


     for (int i = 0 ; i < count; i++) {
         if (list[i].type == selectedSensorType) {
             err = device->activate(device, list[i].handle, 0);
             if (err != 0) {
                 printf("deactivate() for '%s'failed (%s)\n",
                        list[i].name, strerror(-err));
                 return 0;
             }
         }
     }

     err = sensors_close(device);
     if (err != 0) {
         printf("sensors_close() failed (%s)\n", strerror(-err));
     }
     return 0;
}
