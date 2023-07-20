# https://www.piesocket.com/blog/python-websocket/ 

import asyncio
import websockets
import pathlib
import ssl
import time
import threading 
import random 
import serial
import os
import sys
import glob


#Global variables
sensor=0
connected = False
Ts=1
RX_cnt=0
MCUBuffer_RX=0
MCUBuffer_TX=0
ser=0 #Serival variable



def handle_data():
    global connected,MCUBuffer_RX
    MCUBuffer="MCU: "+ MCUBuffer_RX

def read_from_port(ser):
    global connected,MCUBuffer_RX,RX_cnt,sensor
    while not connected:
        #serin = ser.read()
        connected = True

        while True:
           MCUBuffer_RX= ser.readline().decode() #Read data from ARM
           print('MCU: '+MCUBuffer_RX)       
           #handle_data() #Write on screen
           #buffer_tx="ARM" + str(sensor) + "&"
           #ser.write(buffer_tx.encode()) #Send data BACK
        


#Timer function for random number
def time_function():
    global sensor #Gloabal variable

    #Random number
    sensor=random.uniform(10.5,40.0)
    #print("Random number: %.2f" %sensor )#Formating float number
    threading.Timer(Ts, time_function).start() #update 1s


# Webscoket Handler function
async def WebSocket_function(websocket, path):
    global MCUBuffer_RX
    t = time.time()
    data = await websocket.recv()
    data1=MCUBuffer_RX.split()
    print("Data IN: " + data)
    if data=='SERVER':
        #print("Time: "+str(elapsed))
        if data1[0]=='DS':
            reply=MCUBuffer_RX
        else:
            reply="ECHO: "+MCUBuffer_RX #Send data from MCU

        await websocket.send(reply)
        elapsed = time.time() - t
 #   elif data=='SD &':
 #      ser.write(data.encode())
        #reply="ECHO: "+MCUBuffer_RX #Send data from MCU
        #await websocket.send(reply)
        #elapsed = time.time() - t
    else:
        ser.write(data.encode())





#Function: MAIN
if __name__ == '__main__': 

    #Serial port initialization
    try:
        ser = serial.Serial("COM15", 115200)
        thread = threading.Thread(target=read_from_port, args=(ser,))
        thread.start()
        print(" Serial-Adapter OK!")
    except:
        print("No Serial-Adapter")

    #Run timer function
    #time_function()

    # WebSocket Server 
    print('Websocket-Serial Example')
    #start_server = websockets.serve(WebSocket_function, "", 8000) #192.168.1.104
    start_server = websockets.serve(WebSocket_function, "localhost", 8800)    #192.168.1.104


    #ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    #ssl_context.load_cert_chain(pathlib.Path(__file__).with_name('localhost.pem'))
    #start_server = websockets.serve(WebSocket_function, 'localhost', 8443, ssl=ssl_context)

    #ssl_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
    #ssl_context.load_cert_chain(os.path.join('/Users/Matej/Desktop/Praksa/Praksa1/Python/', "tls.crt"), 
    #                            os.path.join('/Users/Matej/Desktop/Praksa/Praksa1/Python/', "tls.key"))
    #print(ssl_context)
    #start_server = websockets.serve(WebSocket_function, 'localhost', '8800', ssl=ssl_context)

    print('Server running')
    
    # Run server
    asyncio.get_event_loop().run_until_complete(start_server)
    asyncio.get_event_loop().run_forever()


