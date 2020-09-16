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
#import wmi -- only for cpu temp on windows, psutil handles linux, doesn't really work though since openhardwaremonito needs to be running as well.

hwtypes = ['Mainboard','SuperIO','CPU','RAM','GpuNvidia','GpuAti','TBalancer','Heatmaster','HDD']
senstypes = ['Voltage','Clock','Temperature','Load','Fan','Flow','Control','Level','Factor','Power','Data','SmallData']

def space_pad(number,length):
    number_length = len(str(number))
    spaces_to_add = length - number_length
    return (' ' * spaces_to_add) + str(number)

def initialize_openhardwaremonitor():
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

def write_data(handle):
    global gpuTemp
    global cpuTemp
    global gpuLoad
    for i in handle.Hardware:
        i.Update()
        for sensor in i.Sensors:                
            if hwtypes[sensor.Hardware.HardwareType] == 'GpuNvidia' and sensor.SensorType == senstypes.index('Temperature'):
                gpuTemp=int(sensor.Value)
            elif hwtypes[sensor.Hardware.HardwareType] == 'GpuNvidia' and sensor.SensorType == senstypes.index('Load'):
                if sensor.Name == 'GPU Core':
                    gpuLoad=int(sensor.Value)
            elif hwtypes[sensor.Hardware.HardwareType] == 'CPU' and sensor.SensorType == senstypes.index('Temperature'):
                if sensor.Name == 'CPU Package':
                    cpuTemp=int(sensor.Value)

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

        #cpu data
        cpuLoad = int(psutil.cpu_percent(None, False))

        #run data
        write_data(HardwareHandle)
       
        #cpu strings
        c_info= str(cpuTemp) + 'C ' + \
                str(cpuLoad) + '%'

        #gpu strings
        g_info= str(gpuTemp) + 'C ' + \
                str(gpuLoad) + '%'

        #arduino string
        #print(c_info)
        #print(g_info)


        ard_info="C "+(c_info) +"| G "+ (g_info)+"^ /"
        print(ard_info)
        ser.write(ard_info.encode())
        
        
