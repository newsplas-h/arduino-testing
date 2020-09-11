#pypcmonitor
#program for sending openhardwaremonitor.dll data to serial devices

import atexit #to close serial port
import signal
import sys
import serial
import psutil
import time
import clr #from pythonnet, reads dlls
import os
#import wmi -- only for cpu temp on windows, psutil handles linux, doesn't really work though since openhardwaremonitor needs to be running as well.

hwtypes = ['Mainboard','SuperIO','CPU','CPU Package','RAM','GpuNvidia','GpuAti','TBalancer','Heatmaster','HDD']
senstypes = ['Voltage','Clock','Temperature','Load','Fan','Flow','Control','Level','Factor','Power','Data','SmallData']

def initialize_openhardwaremonitor():
    #needs 32 bit python to run, .dll is 32bit
    file = 'OpenHardwareMonitorLib'
    clr.AddReference(file)

    from OpenHardwareMonitor import Hardware

    handle = Hardware.Computer()
    handle.MainboardEnabled = True
    handle.CPUEnabled = True
    handle.RAMEnabled = True
    handle.GPUEnabled = True
    handle.HDDEnabled = True
    handle.Open()
    return handle

def write_gpu(handle):
    for i in handle.Hardware:
        i.Update()
        for sensor in i.Sensors:
            #gpu load/temp
            if hwtypes[sensor.Hardware.HardwareType] == 'GpuNvidia' and sensor.SensorType == senstypes.index('Load'):
                ser.write(bytes(str(sensor.Value) + '^', 'utf-8'))
                
            elif hwtypes[sensor.Hardware.HardwareType] == 'GpuNvidia' and sensor.SensorType == senstypes.index('Temperature'):
                ser.write(bytes(str(sensor.Value) + ',', 'utf-8'))

def write_cpu(handle):
    for i in handle.Hardware:
        i.Update()
        for sensor in i.Sensors:
            #cpu slightly different, adapted from c++ code (gives temp as null? -- to fix, run as administrator)
            if hwtypes[sensor.Hardware.HardwareType] == 'CPU' and sensor.SensorType == senstypes.index('Temperature'):
                if sensor.Name == 'CPU Package':
                    ser.write(bytes(str(sensor.Value) + ',', 'utf-8'))
                    print(sensor.Name)
                    print(sensor.Value)
            
                

def handle_exit():
    ser.close()



if __name__ == "__main__":
    atexit.register(handle_exit)
    signal.signal(signal.SIGTERM, handle_exit)
    signal.signal(signal.SIGINT, handle_exit)

    ser = serial.Serial('COM3', 9600, timeout=None)

    HardwareHandle = initialize_openhardwaremonitor()

    while(True):
        #update speed
        time.sleep(1)
        
        
        #unsupported on windows
        #print(psutil.sensors_temperatures(fahrenheit=False))
        cpuUsage = psutil.cpu_percent(None, False)
        ser.write(bytes('*' + str(cpuUsage) + ',', 'utf-8'))

        ramUsage = psutil.virtual_memory()
        ramUsed = ramUsage.used / (2**30)
        ramUsed = round(ramUsed, 1)
        ramTotal = ramUsage.total / (2**30)
        ramTotal = round(ramTotal, 1)
        ser.write(bytes(str(ramUsed) + ',', 'utf-8'))
        ser.write(bytes(str(ramTotal) + ',', 'utf-8'))
        write_gpu(HardwareHandle)
        write_cpu(HardwareHandle)
        
