from tkinter import *
from tkinter import messagebox
from tkinter.ttk import *
import serial
import time
import struct
#from reader.__main__ import main
#from polyglot_turtle import PolyglotTurtleXiao, PinDirection

"""
TODO:
Use list of entry boxes instead of current method
"""
from time import strftime
#pt = PolyglotTurtleXiao()
port = ""
baudrate = 9600
parity = serial.PARITY_NONE
no = serial.EIGHTBITS
stopbits = serial.STOPBITS_ONE






root = Tk()
root.configure(background = 'purple')
root.title('RollTimer Configuration')
root.geometry('450x440')
freq = ["","","",""]
alarmTimes = ["","","",""]
durations = ["","","",""]
#['0', '0', '0', '0']


ser=serial.Serial()
connected = 0

port = "\\.\COM5"    #+
#ser.port = port
#ser.baudrate = baudrate
#ser.timeout = 0
#ser.parity = parity
#ser.bytesize = no
#ser.stopbits = stopbits
#ser.open()
#
#time.sleep(1)
#time.sleep(1)
#ser.setDTR()
#time.sleep(1)



def openSerial(event):
    global connected

    port = "\\.\COM" + setPorte.get()
    #print(port)
    ser.port = port
    ser.baudrate = baudrate
    ser.timeout = 0
    ser.parity = parity
    ser.bytesize = no
    ser.stopbits = stopbits
    try:
        ser.open()
    except:
        if connected == 0:
            messagebox.showinfo("Error", "Please connect device")
        else:
            messagebox.showinfo("Error", "Device already connected")
        return
    time.sleep(1)
    time.sleep(1)
    ser.setDTR()
    time.sleep(1)
    connected = 1





degreeSign = u"\N{DEGREE SIGN}"
def timestr():
    string = strftime('%H:%M:%S %p')
    lbl.config(text = string)

    lbl.after(1000, timestr)

def readser():
    try:
        print(connected)
        if connected == 1:
            bytes = ser.read(5)
            print(bytes)
            mode = bytes[0] - 176
            if(mode == 1):
                mode = "Stopwatch"
            if mode == 2:
                mode = "Temp and Humidity"
            if mode == 3:
                mode = "Battery"
            if mode == 4:
                mode = "Clock"

            accel = "" + str(bytes[1] - 176) + "m/s^2"
            temp = "" + str(bytes[2] - 176) + degreeSign + "C"
            humid = "" + str(bytes[3] - 176) + "." + str(bytes[4] - 176) + "%RH"
            modelabel.config(text=mode)
            accelLabel.config(text=accel)
            TempLabel.config(text=temp)
            humidlbl.config(text=humid)
    except IndexError:
        pass
    except:
        messagebox.showinfo("Error", "Device has been disconnected, please reconnect device and press ok")
    root.after(1000, readser)




def popUpTimeMsg():
    print('Time is set to:', strftime('%H:%M:%S %p'))
    hours = int(strftime("%H"))
    hours = hours.to_bytes(1, 'big')
    print(hours)
    mins = int(strftime("%M"))
    mins = mins.to_bytes(1, 'big')
    print(mins)
    secs = int(strftime("%S"))
    secs = secs.to_bytes(1, 'big')
    print(secs)
    data = 61
    print(data)
    data = data.to_bytes(1, 'big')

    ser.write(data)
    ser.flush()
    ser.write(hours)
    ser.flush()
    ser.write(mins)
    ser.flush()
    ser.write(secs)
    ser.flush()
   # bytes = ser.read(1)
    #print(str(bytes))


def printAlarm(en):

    print("Alarm %s is set to %s" %(dropDown.get(), entry.get()))
    alarmTimes[(int(dropDown.get()) - 1)] = entry.get()
   # alarmTimes.append(dropDown.get())

def setAlarmFreqs():
    freq[0] = entry1.get()
    freq[1] = (entry3.get())
    freq[2] = (entry5.get())
    freq[3] = (entry7.get())
    durations[0] = entry2.get()
    durations[1] = entry4.get()
    durations[2] = entry6.get()
    durations[3] = entry8.get()
    for i in range(0, len(freq)):
        if freq[i] == "" or len(freq[i]) != 4:
            freq[i] = "0"
    for i in range(0, len(durations)):
        if durations[i] == "" or len(durations[i]) != 1:
            durations[i] = "0"

    dur1 = int(durations[0]).to_bytes(1, 'big')
    dur2 = int(durations[1]).to_bytes(1, 'big')
    dur3 = int(durations[2]).to_bytes(1, 'big')
    dur4 = int(durations[3]).to_bytes(1, 'big')
    freq1 = int(freq[0]).to_bytes(2, 'big');
    freq2 = int(freq[1]).to_bytes(2, 'big');
    freq3 = int(freq[2]).to_bytes(2, 'big');
    freq4 = int(freq[3]).to_bytes(2, 'big');
    data = 62
    data = data.to_bytes(1, 'big')
    data1 = 70
    data1 = data1.to_bytes(1, 'big')
    ser.write(data)
    ser.write(freq1)
    ser.write(freq2)
    ser.write(freq3)
    ser.write(freq4)
    ser.flush()
    ser.write(data1)
    ser.write(dur1)
    ser.write(dur2)
    ser.write(dur3)
    ser.write(dur4)
    ser.flush()

    #print(freq1, freq2, freq3, freq4)


def setAlarmTimes():
    print(alarmTimes)
    for i in range(0, len(alarmTimes)):
        if (len(alarmTimes[i]) != 8):
            pass
        elif alarmTimes[i] == "":
            continue
        else:
            times = alarmTimes[i].split(':')
            hours = int(times[0]).to_bytes(1, 'big')
            mins = int(times[1]).to_bytes(1, 'big')
            secs = int(times[2]).to_bytes(1, 'big')
            data = 65 + i
            data = data.to_bytes(1, 'big')
            ser.write(data)
            ser.write(hours)
            ser.write(mins)
            ser.write(secs)
            ser.flush()
            print(times)
            #print("Hello")




def seeAlarms():
    message= ""
    count = 1
    for i in range(0, len(alarmTimes)):
        if alarmTimes[i] == "":
            alarmTimes[i] = "Alarm not Set"
        message = message + "Alarm: " + str(count) + " set to " + alarmTimes[i] + "\n"
        count += 1
    messagebox.showinfo("Current Alarm Times", message)

    """Send value from entry box to the AVR"""

"""Messy function, could fix using a for loop"""
def onClick(event):
    if entry.get() == 'HH:MM:SS':
       entry.delete(0, "end") # delete all the text in the entry
       entry.insert(0, '') #Insert blank for user input
       entry.config(foreground = 'black')
    if entry1.get() == 'Between 1kHz - 6kHz':
       entry1.delete(0, "end") # delete all the text in the entry
       entry1.insert(0, '') #Insert blank for user input
       entry1.config(foreground = 'black')
    if entry2.get() == 'Duration (ms)':
       entry2.delete(0, "end")  # delete all the text in the entry
       entry2.insert(0, '')  # Insert blank for user input
       entry2.config(foreground='black')
    if entry3.get() == 'Between 1kHz - 6kHz':
       entry3.delete(0, "end") # delete all the text in the entry
       entry3.insert(0, '') #Insert blank for user input
       entry3.config(foreground = 'black')
    if entry4.get() == 'Duration (ms)':
       entry4.delete(0, "end")  # delete all the text in the entry
       entry4.insert(0, '')  # Insert blank for user input
       entry4.config(foreground='black')
    if entry5.get() == 'Between 1kHz - 6kHz':
       entry5.delete(0, "end") # delete all the text in the entry
       entry5.insert(0, '') #Insert blank for user input
       entry5.config(foreground = 'black')
    if entry6.get() == 'Duration (ms)':
       entry6.delete(0, "end")  # delete all the text in the entry
       entry6.insert(0, '')  # Insert blank for user input
       entry6.config(foreground='black')
    if entry7.get() == 'Between 1kHz - 6kHz':
        entry7.delete(0, "end")  # delete all the text in the entry
        entry7.insert(0, '')  # Insert blank for user input
        entry7.config(foreground='black')
    if entry8.get() == 'Duration (ms)':
        entry8.delete(0, "end")  # delete all the text in the entry
        entry8.insert(0, '')  # Insert blank for user input
        entry8.config(foreground='black')
"""clean up using a list of entry boxes"""
def onFocusOut(event):
    if entry.get() == '':
       entry.insert(0, 'HH:MM:SS')
       entry.config(foreground = 'grey')
    if entry1.get() == '':
       entry1.insert(0, 'Between 1kHz - 6kHz')
       entry1.config(foreground='grey')
    if entry2.get() == '':
       entry2.insert(0, 'Duration (ms)')
       entry2.config(foreground='grey')
    if entry3.get() == '':
        entry3.insert(0, 'Between 1kHz - 6kHz')
        entry3.config(foreground='grey')
    if entry4.get() == '':
        entry4.insert(0, 'Duration (ms)')
        entry4.config(foreground='grey')
    if entry5.get() == '':
        entry5.insert(0, 'Between 1kHz - 6kHz')
        entry5.config(foreground='grey')
    if entry6.get() == '':
        entry6.insert(0, 'Duration (ms)')
        entry6.config(foreground='grey')
    if entry7.get() == '':
        entry7.insert(0, 'Between 1kHz - 6kHz')
        entry7.config(foreground='grey')
    if entry8.get() == '':
        entry8.insert(0, 'Duration (ms)')
        entry8.config(foreground='grey')
def printFreq(event):
    if (len(freq) == 8):
        print("4 freq already chosen")
        return
    if (entry2.get() == ''):
        print('Please enter a duration')
        #entry1.delete(0, 'end')
        return
    freq.append(entry1.get())
    freq.append(entry2.get())
    print(freq)

    entry1.delete(0,'end')


#pt.gpio_set_direction(0, PinDirection.INPUT)
#pt.gpio_set_direction(1, PinDirection.INPUT)
#pt.gpio_set_direction(2, PinDirection.INPUT)
#pt.gpio_set_direction(3, PinDirection.INPUT)

#--------------------- Setting all of the entry boxes --------------------------
setAlarm = Label(root, text = 'Set Alarm Times', background = 'purple', foreground = 'white').place(x=30, y=100)
entry = Entry(root)
entry.insert(0, 'HH:MM:SS')
entry.bind('<FocusIn>', onClick)
entry.bind('<FocusOut>', onFocusOut)
entry.config(foreground = 'grey')
entry.place(x = 200, y = 100)
entry.bind('<Return>', printAlarm)
setFrequency = Label(root, text = 'Set Tone Frequency 1', background = 'purple', foreground = 'white').place(x = 30, y = 130)
entry1 = Entry(root)
entry1.place(x = 200, y = 130)
entry1.insert(0, 'Between 1kHz - 6kHz')
entry1.bind('<FocusIn>', onClick)
entry1.bind('<FocusOut>', onFocusOut)
entry1.config(foreground = 'grey')
entry1.bind('<Return>', printFreq)
entry2 = Entry(root, width=15)
entry2.place(x=330, y=130)

entry2.insert(0, 'Duration (ms)')
entry2.bind('<FocusIn>', onClick)
entry2.bind('<FocusOut>', onFocusOut)
entry2.config(foreground = 'grey')
entry2.bind('<Return>', printFreq)

setFrequency = Label(root, text = 'Set Tone Frequency 2', background = 'purple', foreground = 'white').place(x = 30, y = 160)
entry3 = Entry(root)
entry3.place(x = 200, y = 160)
entry3.insert(0, 'Between 1kHz - 6kHz')
entry3.bind('<FocusIn>', onClick)
entry3.bind('<FocusOut>', onFocusOut)
entry3.config(foreground = 'grey')
entry3.bind('<Return>', printFreq)
entry4 = Entry(root, width=15)
entry4.place(x=330, y=160)

entry4.insert(0, 'Duration (ms)')
entry4.bind('<FocusIn>', onClick)
entry4.bind('<FocusOut>', onFocusOut)
entry4.config(foreground = 'grey')
entry4.bind('<Return>', printFreq)

setFrequency = Label(root, text = 'Set Tone Frequency 3', background = 'purple', foreground = 'white').place(x = 30, y = 190)
entry5 = Entry(root)
entry5.place(x = 200, y = 190)
entry5.insert(0, 'Between 1kHz - 6kHz')
entry5.bind('<FocusIn>', onClick)
entry5.bind('<FocusOut>', onFocusOut)
entry5.config(foreground = 'grey')
entry5.bind('<Return>', printFreq)
entry6 = Entry(root, width=15)
entry6.place(x=330, y=190)

entry6.insert(0, 'Duration (ms)')
entry6.bind('<FocusIn>', onClick)
entry6.bind('<FocusOut>', onFocusOut)
entry6.config(foreground = 'grey')
entry6.bind('<Return>', printFreq)

setFrequency = Label(root, text = 'Set Tone Frequency 4', background = 'purple', foreground = 'white').place(x = 30, y = 220)
entry7 = Entry(root)
entry7.place(x = 200, y = 220)
entry7.insert(0, 'Between 1kHz - 6kHz')
entry7.bind('<FocusIn>', onClick)
entry7.bind('<FocusOut>', onFocusOut)
entry7.config(foreground = 'grey')
entry7.bind('<Return>', printFreq)
entry8 = Entry(root, width=15)
entry8.place(x=330, y=220)

entry8.insert(0, 'Duration (ms)')
entry8.bind('<FocusIn>', onClick)
entry8.bind('<FocusOut>', onFocusOut)
entry8.config(foreground = 'grey')
entry8.bind('<Return>', printFreq)

setPort = Label(root, text = 'Select COM port', background = 'purple', foreground = 'white').place(x = 30, y = 70)
setPorte = Entry(root)
setPorte.place(x = 200, y = 70)
setPorte.bind('<Return>', openSerial)


optlist = ["1", "2", "3", "4"]

dropDown = StringVar(root)
dropDown.set("Alarm No.")


opt = OptionMenu(root, dropDown, "Alarm No.", *optlist, command=printAlarm)

opt.config(width = 10)
opt.place(x=330, y=100)

#----------------------- Clock Labels ------------------------------------------
lbl1 = Label(root, text = 'Current Mode: ', background = 'purple', foreground = 'white').place(x=30, y = 250)
modelabel = Label(root, text='', background='purple', foreground='white')
modelabel.place(x=200, y=250)

#------------------------ Acceleromter Labels ----------------------------------
lbl2 = Label(root, text = 'Accelerometer: ', background = 'purple', foreground = 'white').place(x=30, y = 280)
accelLabel = Label(root, text = '', background = 'purple', foreground = 'white')
accelLabel.place(x=200, y = 280)

#------------------------ Temperature Labels -----------------------------------
lbl3 = Label(root, text = 'Temperature: ', background = 'purple', foreground = 'white').place(x=30, y = 310)
TempLabel = Label(root, background = 'purple', foreground = 'white')
TempLabel.place(x=200, y = 310)

#------------------------ Humidity Labels --------------------------------------
humidlbl = Label(root, text = 'Humidity: ', background = 'purple', foreground = 'white').place(x=30, y = 340)
humidlbl = Label(root, text = '', background = 'purple', foreground = 'white')
humidlbl.place(x=200, y = 340)

#------------------------ Set Time Button --------------------------------------
setTime = Button(root, text='Set Time', command = popUpTimeMsg)
setTime.place(x=400, y = 410, anchor = 's')

seeAlarmButton = Button(root, text='See Alarms', command = seeAlarms)
seeAlarmButton.place(x=310, y = 410, anchor = 's')

setAlarmButton = Button(root, text='Set Alarms Frequencies', command = setAlarmFreqs)
setAlarmButton.place(x=80, y = 410, anchor = 's')

setAlarmTimesB = Button(root, text='Set Alarm Times', command = setAlarmTimes)
setAlarmTimesB.place(x=210, y = 410, anchor = 's')


lbl = Label(root, font = ('calibri', 40, 'bold'),
                background = 'purple',
                foreground = 'white')
lbl.pack(anchor = 'center')





def main():
    readser()


    timestr()


    mainloop()


if __name__ == '__main__':
    main()
