#Force Mat python script Spring 2021
#worked on by Rhys Cornelius and Shawn Steven
#please note that there are unused variables and redundencies in the update_data function


from pyqtgraph.graphicsItems.ViewBox.ViewBox import ViewBox
import serial
from PyQt5 import QtWidgets, QtCore
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import numpy
import numpy as np
import sys  # We need sys so that we can pass argv to QApplication
import os
import time


class MainWindow(QtWidgets.QMainWindow):

    #defining the function that is run initially
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)

        #here we set the COM# and the baudrate + some variables
        ser = serial.Serial('COM13', 74880)
        #128 by 64 matrix of zeros is made
        force = numpy.zeros((128, 64))
        matList = numpy.zeros(8192)

        #we set heatmap to be an image item which will be what we display
        #we initially set the heatmap to be a blank matrix
        heatmap = pg.ImageItem()
        heatmap.setImage(force)

        #Create a box to store images&Set image object
        self.view_box = pg.ViewBox()
        self.view_box.addItem(heatmap)

        #Plot object creation&View created above_set box
        self.graphWidget = pg.PlotWidget(viewBox = self.view_box)
        self.setCentralWidget(self.graphWidget)

        #here we start a timer that lasts 10milliseconds
        #when the timer runs out, we run update_data
        self.timer = QtCore.QTimer()
        self.timer.setInterval(10)
        self.timer.timeout.connect(lambda: self.update_data(ser, force, matList))
        #print("test")
        self.timer.start()
        
    #function to display data coming from the serial
    def update_data(self, ser, force, matList):
        editList = matList
        #we need to decode the data that comes from the serial
        decoded = ""
        
        #f1 is used with f2 to determine how long a particular step took
        #feel free to move f1 and f2 to different steps at the code to see how long it takes to execute
        f1 = time.time()

        #we wait until the arduino sends an "M", which signifies that it is about to start sending data about the current mat
        while(decoded != "M"):
            num = ""
            #force = numpy.zeros((128, 64))
            firstInput = ser.read()

            #we use "try" instead of just decoding because when the mat is first started, some weird characters are sent
            #into the serial that cannot be decoded and consequently crash the script.
            try:
                decoded = firstInput.decode("utf-8")
            except:
                decoded = ""

        #once the beginning of the new mat data is found
        #go through all the points and decode them and add it to a list
        for i in range(8192):
            #the data coming from the is sent one character at a time so we need to discern values from each other
            sameNum = True
            while(sameNum):
                input = ser.read()
                try:
                    final = input.decode("utf-8")
                except:
                    final = ""

                #there is a new line after every data value sent from the arduino
                if(final != '\n'):
                    num += final
                else:
                    if(num == ''):
                        number = 0
                    else:
                        #sometimes we got an error here and the following try fixed it
                        try:
                            number = float(num)
                        except:
                            number = 0
                    #edit list will contain the value at each point
                    editList[i] = number
                    num = ""
                    sameNum = False
        #note that we reshape matList instead of editList by accident, it still works and we're not too sure why
        #it should be "force = editList.reshape(128, 64)". we don't have time to test whether it works though
        # we must reshape the list into a matrix because that is the datatype that is accepted by the heatmap
        force = matList.reshape(128, 64)     
        newForce = force #quite redundant
        newHeatmap = pg.ImageItem()
        newHeatmap.setImage(newForce)
        self.view_box.clear()
        self.view_box.addItem(newHeatmap)

        f2 = time.time()
        #this print statement is used to see how long certain steps take
        print("Time: ", abs(f2-f1), "s")


def main():
    app = QtWidgets.QApplication(sys.argv)
    main = MainWindow()
    main.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

