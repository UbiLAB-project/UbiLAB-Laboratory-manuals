# https://www.piesocket.com/blog/python-websocket/ 

import asyncio
import websockets
import time

print('Websocket Example')


# create handler for each connection
print('Server running')
async def handler(websocket, path):
    t = time.time()
    data = await websocket.recv()
    print("Data IN: " + data)
    #reply = f"ECHO:  {data}!"
    #reply = "ECHO: "+str(23.43)+" "+str(72312.43)
    reply="ECHO: "+str(1+float(data))
    await websocket.send(reply)
    elapsed = time.time() - t
    print("Time: "+str(elapsed))

# Server object
start_server = websockets.serve(handler, "localhost", 8000)

# Run server
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()