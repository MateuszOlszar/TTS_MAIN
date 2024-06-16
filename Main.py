import serial
import time 
import matplotlib.pyplot as plt
from matplotlib.widgets import TextBox
from matplotlib.widgets import Button
from matplotlib.animation import FuncAnimation
from matplotlib.artist import Artist
import numpy as np

CALIB_ID_BASE = "729" #0x729

calib_region = "none"

tyre_inner = []
tyre_middle = []
tyre_outer = []

def start_calibration(event):
    frame = "t" + CALIB_ID_BASE + "8" + "0000000000000000\r"
    can.write(frame.encode(encoding='ASCII'))

def toint16(data):
    temp_l = int(data[0:2].decode("ASCII"),16)
    temp_h = int(data[3:5].decode("ASCII"),16)
    return temp_l | (temp_h<<8)

def inner_click(event):
    global calib_region
    calib_region = "inner"
    print("Inner")

def middle_click(event):
    global calib_region
    calib_region = "middle"
    print("middle")

def outer_click(event):
    global calib_region
    calib_region = "outer"
    print("outer")

def prepare_string(tyre_points, tyre_region:str):
    #przygotuj punkty do wysyłki (zamień je na indeks pixela)
    points = []
    for point in tyre_points:
        points.append(3-point[0] + point[1]*4)
    if(len(points)>14):
        print("TOO MANY POINTS" + tyre_region)
        return None
    if(len(points) == 0):
        print("NO POINTS "+tyre_region)
        return None
    if(len(points) <= 7):
        frame = "t" + CALIB_ID_BASE + "8" + tyre_region + "".join(format(i, '02x') for i in points) + "\r"
        return frame
    else:
        frame = "t" + CALIB_ID_BASE + "8" + tyre_region + "".join(format(points[i], '02x') for i in range(7))+ "\r"
        frame = frame +"\n"
        frame = frame + "t" + CALIB_ID_BASE + "8" + tyre_region + "".join(format(points[i], '02x') for i in range(7, len(points)))+ "\r"
        return frame

def calibrate_click(event):
    #inner
    ret = prepare_string(tyre_inner, "01")
    if(ret is not None):
        print(ret)
        can.write(ret.encode(encoding='ASCII'))
        time.sleep(0.5)
    #middle
    ret = prepare_string(tyre_middle, "02")
    if(ret is not None):
        print(ret)
        can.write(ret.encode(encoding='ASCII'))
        time.sleep(0.5)
    #outer
    ret = prepare_string(tyre_outer, "03")
    if(ret is not None):
        print(ret)
        can.write(ret.encode(encoding='ASCII'))
        time.sleep(0.5)
    done_frame = "t" + CALIB_ID_BASE + "8" + "0000000000000000\r"
    can.write(done_frame.encode(encoding="ASCII"))
    print("Skalibrowane")

def update(frame):
    global temps
    try:
        data = can.read_all()
        if data != b'':
            id_beg = data.find(b't')+1
            id_end = id_beg + 3
            id = int(data[id_beg:id_end].decode("ASCII"), 16)
            temp1 = toint16(data[id_end+1:id_end+5])
            temp2 = toint16(data[id_end+5:id_end+9])
            temp3 = toint16(data[id_end+9:id_end+13])
            temp4 = toint16(data[id_end+13:id_end+17])
            tempz = [temp1, temp2, temp3, temp4]

            for i in range(4):
                temps[(id-0x69)*4+i] = tempz[i]/10

            for point in tyre_inner:
                temps[3-point[0] + point[1]*4] = -100

            for point in tyre_middle:
                temps[3-point[0] + point[1]*4] = 100

            for point in tyre_outer:
                temps[3-point[0] + point[1]*4] = 0

            flipped_temps = np.flip(temps.reshape((16, 4)), axis=1)
            image.set_data(flipped_temps)
        return image
    except:
        print("BRAK POŁĄCZENIA Z CZUJNIKIEM!")

 
def onpick(event):
    global calib_region
    # artist = event.artist
    clicked_point = (int(event.xdata), int(event.ydata))
    if(clicked_point == (0,0)):
        return
    if calib_region == "inner":
        if clicked_point not in tyre_inner:
            tyre_inner.append(clicked_point)
        else:
            tyre_inner.remove(clicked_point)
        print(tyre_inner)
    
    if calib_region == "middle":
        if clicked_point not in tyre_middle:
            tyre_middle.append(clicked_point)
        else:
            tyre_middle.remove(clicked_point)
        print(tyre_middle)
    
    if calib_region == "outer":
        if clicked_point not in tyre_outer:
            tyre_outer.append(clicked_point)
        else:
            tyre_outer.remove(clicked_point)
        print(tyre_outer)

def submit(expression):
    global CALIB_ID_BASE
    CALIB_ID_BASE = expression

    print(CALIB_ID_BASE)
 
if __name__ == "__main__":
    can = serial.Serial(port="COM4", baudrate=115200)
    can.write('C\r'.encode(encoding='ASCII'))
    time.sleep(0.1)
    can.write("S8\r".encode(encoding='ASCII')) #S8 - 1Mbps
    time.sleep(0.1)
    can.write("O\r".encode(encoding='ASCII'))
    time.sleep(0.1)

    fig, ax = plt.subplots()
    temps = np.zeros(64)

    #--------------Dodanie przycisków--------------
    plt.subplots_adjust(bottom=0.2)
    inner_btn = plt.axes([0.45, 0.05, 0.1, 0.075])  # Pozycja i wielkość przycisku [left, bottom, width, height]
    button_inner = Button(inner_btn, 'Inner')

    middle_btn = plt.axes([0.3, 0.05, 0.1, 0.075], )
    button_middle =  Button(middle_btn, 'Middle')

    outer_btn = plt.axes([0.15, 0.05, 0.1, 0.075], )
    button_outer =  Button(outer_btn, 'Outer')
    
    calbirate_btn = plt.axes([0.35, 0.15, 0.15, 0.075], )
    button_calibrate =  Button(calbirate_btn, 'Calibrate')
    
    start_calib_btn = plt.axes([0.15, 0.15, 0.15, 0.075], )
    button_start_calib =  Button(start_calib_btn, 'Start')

    button_inner.on_clicked(inner_click)
    button_middle.on_clicked(middle_click)
    button_outer.on_clicked(outer_click)
    button_calibrate.on_clicked(calibrate_click)
    button_start_calib.on_clicked(start_calibration)
    #----------------------------------------------

    graphBox = fig.add_axes([0.3, 0.25, 0.1, 0.075])
    txtBox = TextBox(graphBox, "ID Ramki: ")
    txtBox.set_val("729")
    txtBox.on_submit(submit)


    fig.canvas.mpl_connect('button_press_event', onpick)
    image = (ax.imshow(temps.reshape((16, 4)), cmap='hot', vmin=-10 , vmax=40, picker=True))

    plt.colorbar(image, ax=ax)

    ani = FuncAnimation(fig, update, interval=10)
    plt.show()