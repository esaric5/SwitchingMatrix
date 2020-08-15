# importing libraries 
from PyQt5.QtWidgets import * 
from PyQt5 import QtCore, QtGui 
from PyQt5.QtGui import * 
from PyQt5.QtCore import * 
import sys 
  
states = {}

class Window(QWidget): 
  
    def __init__(self): 
        super().__init__() 

        self.setWindowTitle("Switching Matrix") 
        self.counter = 1
        self.element = {}
        #self.mapper = QSignalMapper(self)
        
        self.setGeometry(100, 100, 500, 400) 
        self.setStyleSheet("background-color: white")
        self.UiComponents() 

        self.show() 

    def UiComponents(self): 
    
        self.layout = QGridLayout()

        for i in range(16):
            for j in range(16):
                states[i, j] = -1
                self.element[i, j]=QPushButton('')
                self.layout.addWidget(self.element[i, j], i, j)
                self.element[i, j].setFixedWidth(20)
                self.element[i, j].setFixedHeight(20)
                #self.mapper.setMapping(self.element[i, j], i, j)
                self.element[i, j].setStyleSheet("border-radius: 9px; background-color: #96BB7C")
                # self.element[i, j].clicked.connect(self.mapper.map)
                self.element[i, j].clicked.connect(lambda i, j: self.onClick, i, j)
                
        # self.mapper.mapped['int'].connect(self.clicked)
        self.setLayout(self.layout)
    
    # @pyqtSlot(int)
    # def clicked(self, i, j):
        # print(str(i)+" " + str(j))
    @pyqtSlot(int)   
    def onClick(self, i, j):
        print(str(i)+ " " + str(j))
        
        button.setText(str(self.counter))
        
        color = "#B22222"
        if states[i, j] == 0:
            states[i, j] = -1
        elif states[i, j] == 5:
            states[i, j] = 0
            self.counter += 1
            color = "#222831"
        else:
            states[i, j] = 5
            
        if states[i, j] == -1:
            button.setStyleSheet("border-radius: 9px; background-color: #96BB7C")
            button.setText('')
            self.counter -= 1
        else:
            button.setStyleSheet("border-radius : 9px; border: 1px; background-color: " + color +"; color: white; font-weight: bold;")
  
  
# create pyqt5 app 
App = QApplication(sys.argv) 
  
# create the instance of our Window 
window = Window() 
  
# start the app 
sys.exit(App.exec()) 
