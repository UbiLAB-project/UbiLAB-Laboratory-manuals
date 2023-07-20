# https://www.piesocket.com/blog/python-websocket/ 

import asyncio
import websockets
import time
import threading 
import random 
import serial

#Global variables
sensor=0
connected = False
mes=0
Ts=1
RX_cnt=0
MCUBuffer_RX=0
MCUBuffer_TX=0
ser=0 #Serival variable
wachdog_flag=1;

def handle_data():
    global connected,MCUBuffer_RX
    MCUBuffer="MCU: "+ MCUBuffer_RX 
    print(MCUBuffer)

def read_from_port(ser):
    global connected,MCUBuffer_RX,RX_cnt,sensor
    while not connected:
        #serin = ser.read()
        connected = True

        while True:
           MCUBuffer_RX= ser.readline().decode() #Read data from ARM
           #handle_data() #Write on screen
           #buffer_tx="ARM" + str(sensor) + "&"
           #ser.write(buffer_tx.encode()) #Send data BACK
        



#Timer function for random number
def time_function():
    global sensor,wachdog_flag,mes #Gloabal variable
    
    
    if wachdog_flag==1:       #Wachdog timer
        mes="stop&"
        ser.write(mes.encode()) #Stop the system



    wachdog_flag=1;
    #Random number
    sensor=random.uniform(10.5,40.0)
    #print("Random number: %.2f" %sensor )#Formating float number
    threading.Timer(Ts, time_function).start() #update 1s


# Webscoket Handler function
async def WebSocket_function(websocket, path):
    global MCUBuffer_RX,wachdog_flag
    t = time.time()
    data = await websocket.recv()
    print("Data IN: " + data)
    wachdog_flag=0;
    if data=='SERVER':
        reply=MCUBuffer_RX #Send data from MCU
        await websocket.send(reply)
        elapsed = time.time() - t
        print("Time: "+str(elapsed))
    else:
        ser.write(data.encode()) #Provide data to the ARM-mcu
        #MCUBuffer_RX= ser.readline().decode() #Wait for response
        #reply=MCUBuffer_RX          #Send data over WEBsocket
        #await websocket.send(reply)#Send data over WEBsocket




#Function: MAIN
if __name__ == '__main__': 

    #Serial port initialization
    try:
        ser = serial.Serial("COM7", 115200)
        thread = threading.Thread(target=read_from_port, args=(ser,))
        thread.start()
        print(" Serial-Adapter OK!")
    except:
        print("No Serial-Adapter")

    #Run timer function
    time_function()

    # WebSocket Server 
    print('Websocket-Serial Example')
    #start_server = websockets.serve(WebSocket_function, "localhost", 8000) #192.168.1.104
    start_server = websockets.serve(WebSocket_function, "192.168.1.104", 8000)
    print('Server running') 
    
    # Run server
    asyncio.get_event_loop().run_until_complete(start_server)
    asyncio.get_event_loop().run_forever()


