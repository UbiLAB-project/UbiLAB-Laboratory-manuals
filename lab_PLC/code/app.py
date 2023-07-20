from flask import Flask, request, jsonify, Response, render_template
from flask_cors import CORS
from pymcprotocol import Type1E
import cv2, time
app = Flask(__name__)
CORS(app)

pymc1e = Type1E()
# pymc1e.connect('192.168.110.24', 1025)


sensors = {
    "M0": False,
    "M1": False,
    "M2": False,
    "M3": False
}

buttons = {
    "M4": False,
    "M5": False,
    "M6": False,
    "M7": False,
}

potentiometer = 0
streamUrl = ''

tokens = {
    "qu5pvvUfbWuc3mZ": 'rtsp://admin:admin123456@camera.ubilab.feit.ukim.edu.mk:8554/profile0',
    "Nsdofnb48g4fvzI": 'stream2',
    "Dk7j1LOUFT15DoG": 'stream3',
    "IB0kApUUBy69VWf": 'stream4',
    "vhFZZMYOCS7gOAq": 'stream5',
    "vZl80RajPCT0h3K": 'stream6',
    "tWWjY1Z9jJ2n9yW": 'stream7',
    "jKj4M558tax6mcI": 'stream8',
    "P8iqKmOGiGwZpkD": 'stream9',
    "ufOevw63MKqsEpx": 'stream10',
}
camera = cv2.VideoCapture('rtsp://admin:admin123456@camera.ubilab.feit.ukim.edu.mk:8554/profile0')

def generate():
    while True:
        success, frame = camera.read()
        if not success:
            break
        ret, jpeg = cv2.imencode('.jpg', frame)
        frame = jpeg.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n'
               b'Content-Length: ' + str(len(frame)).encode() + b'\r\n'
               b'\r\n' + frame + b'\r\n')
        
        del jpeg

@app.route('/video_feed', methods=['GET'])
def video_feed():
    return Response(generate(),
        mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/camera_feed')
def camera_feed():
    return render_template('camera_feed.html')

@app.route('/api/update-buttons', methods=['POST'])
def update_buttons():
    initalTime = time.time()
    button = request.json['button']
    buttons[button] = not buttons[button]
    pymc1e.randomwrite_bitunits([button], [buttons[button]])

    return "Finished in: " + time.time() - initalTime

@app.route('/api/update-potentiometer', methods=['POST'])
def update_potentiometer():
    global potentiometer
    initalTime = time.time()
    temp = request.json['potentiometer']
    if potentiometer != temp:
        potentiometer = temp
        pymc1e.randomwrite_wordunits(["D0"], [400 * potentiometer])

    return "Finished in: " + time.time() - initalTime

@app.route('/api/update-sensors', methods=['POST'])
def update_sensors():
    initalTime = time.time()
    sensor = request.json['sensor'].upper()
    sensors[sensor] = not sensors[sensor]
    pymc1e.randomwrite_bitunits([sensor], [sensors[sensor]])

    return "Finished in: " + time.time() - initalTime

@app.route('/api/update-plc', methods=['POST'])
def update_plc():
    initalTime = time.time()
    plc = request.json['plc']

    if plc:
        pymc1e.remote_run()
    else:
        pymc1e.remote_stop()

    return "Finished in: " + time.time() - initalTime


@app.route('/api/initial-data', methods=['POST'])
def initial_data():
    token = request.json['token']
    global camera

    if token in tokens.keys():
        streamUrl = tokens[token]
        return streamUrl
    
    return jsonify('null')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
