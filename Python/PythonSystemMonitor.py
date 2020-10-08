import platform
import psutil
import serial
import serial.tools.list_ports
from datetime import datetime
import threading
from collections import deque
import time
import psutil
svmem = psutil.virtual_memory()

def get_size(bytes, suffix="B"):
    """
    Scale bytes to its proper format
    e.g:
        1253656 => '1.20MB'
        1253656678 => '1.17GB'
    """
    factor = 1024
    for unit in ["", "K", "M", "G", "T", "P"]:
        if bytes < factor:
            return f"{bytes:.2f}{unit}{suffix}"
        bytes /= factor


# GPU information
import GPUtil
from tabulate import tabulate
print("="*40, "GPU Details", "="*40)
gpus = GPUtil.getGPUs()
list_gpus = []
for gpu in gpus:
    # get the GPU id
    gpu_id = gpu.id
    # name of GPU
    gpu_name = gpu.name
    # get % percentage of GPU usage of that GPU
    gpu_load = f"{gpu.load*100}%"
    # get free memory in MB format
    gpu_free_memory = f"{gpu.memoryFree}MB"
    # get used memory
    gpu_used_memory = f"{gpu.memoryUsed}MB"
    # get total memory
    gpu_total_memory = f"{gpu.memoryTotal}MB"
    # get GPU temperature in Celsius
    gpu_temperature = f"{gpu.temperature} °C"
    gpu_uuid = gpu.uuid
    list_gpus.append((
        gpu_id, gpu_name, gpu_load, gpu_free_memory, gpu_used_memory,
        gpu_total_memory, gpu_temperature, gpu_uuid
    ))

def calc_ul_dl(rate, dt=3, interface='Ethernet'):
    t0 = time.time()
    counter = psutil.net_io_counters(pernic=True)[interface]
    tot = (counter.bytes_sent, counter.bytes_recv)

    while True:
        last_tot = tot
        time.sleep(dt)
        counter = psutil.net_io_counters(pernic=True)[interface]
        t1 = time.time()
        tot = (counter.bytes_sent, counter.bytes_recv)
        ul, dl = [(now - last) / (t1 - t0) / 1000.0
                  for now, last in zip(tot, last_tot)]
        rate.append((ul, dl))
        t0 = time.time()


def sendData(rate):
    try:
        cpuPer = psutil.cpu_percent();
        print ('UL: {0:.0f} kB/s / DL: {1:.0f} kB/s'.format(*rate[-1]))
        print(f"Total CPU Usage: {cpuPer}%")
        print(f"Mem perc: {get_size(svmem.used)}")
        
        data = '{0:.0f},{1:.0f}'.format(*rate[-1]);
        data = data + ',' + str(cpuPer);
        data = data + ',' + get_size(svmem.percent);
        #Upload(kB/s), Download(kB/s), CPU-percentage, Mem-Percentage
        #print(data);
        arduino.write(str.encode(data));
    except IndexError:
        'Could not send data'


# Create the ul/dl thread and a deque of length 1 to hold the ul/dl- values
transfer_rate = deque(maxlen=1)
t = threading.Thread(target=calc_ul_dl, args=(transfer_rate,))

# The program will exit if there are only daemonic threads left.
t.daemon = True
t.start()

# The rest of your program, emulated by me using a while True loop
ports = list(serial.tools.list_ports.comports())
for p in ports:
	print (p);
	if "SparkFun" in p.description:
		arduino = serial.Serial(p.device, 115200, timeout=.1)

time.sleep(1) #give the connection a second to settle
while True:
    sendData(transfer_rate)
    time.sleep(1)
    arduino.flush() #flush the buffer