from tkinter import *
import tkinter.messagebox as messagebox
import tkinter.filedialog as filedialog
import test as comm
import os.path

serial_present = False
try:
    import serial
    serial_present = True
except:
    pass

configfilename = 'setup.cfg'

frameFont = ("Arial",12)
labelFont = ("Arial",10)
entryFont = ("Courier",12)
spinFont = ("Courier",24)
buttonFont = ("Courier",14)

class app:

    def __init__(self,master):

        root.title('Unit Tester')

        #build gui
        frame = Frame(master)
        frame.pack()

        leftpan = Frame(frame)
        leftpan.pack(side=LEFT,fill=Y)
        
        #comm settings
        settings = LabelFrame(leftpan,text='Settings',pady=5,padx=5,font=frameFont)
        settings.pack(fill=BOTH,padx=5,pady=5)
        
        companel = Frame(settings)
        companel.pack(side=TOP,fill=X)
        comlabel = Label(companel,text='Port:',font=labelFont)
        comlabel.grid(row=0,column=0,pady=3,sticky=W)
        self.comVar = StringVar(master,'COM1')
        self.comEntry = Entry(companel,width=10,textvariable=self.comVar,font=entryFont,justify=CENTER)
        self.comEntry.grid(row=0,column=1,columnspan=2,pady=3)
        adrlabel = Label(companel,text='Address:',font=labelFont)
        adrlabel.grid(row=1,column=0,columnspan=2,pady=3,sticky=W)
        self.adrVar = StringVar(master,'1')
        self.adrEntry = Entry(companel,width=4,textvariable=self.adrVar,font=entryFont,justify=CENTER,state=DISABLED)
        self.adrEntry.grid(row=1,column=2,pady=3,sticky=E)
                
        #traverse
        traverse = LabelFrame(leftpan,text='Traverse',pady=5,padx=5,font=frameFont)
        traverse.pack(side=TOP,padx=5,pady=5,fill=Y,expand=1)

        m1label = Label(traverse,text='Motor 1',font=labelFont)
        m1label.grid(row=0,column=0,columnspan=3,sticky=W,pady=10)

        m1speedlab = Label(traverse,text='Speed',font=labelFont)
        m1speedlab.grid(row=1,column=0)
        m1speedunlab = Label(traverse,text='Hz',font=labelFont)
        m1speedunlab.grid(row=1,column=2)
        self.m1SpeedVar = StringVar(master,'-')
        self.m1speedentry = Entry(traverse,width=8,textvariable=self.m1SpeedVar,font=entryFont,justify=CENTER)
        self.m1speedentry.grid(row=1,column=1)

        m1accellab = Label(traverse,text='Accel',font=labelFont)
        m1accellab.grid(row=2,column=0)
        m1accelunlab = Label(traverse,text='Hz/s',font=labelFont)
        m1accelunlab.grid(row=2,column=2)
        self.m1AccelVar = StringVar(master,'-')
        self.m1accelentry = Entry(traverse,width=8,textvariable=self.m1AccelVar,font=entryFont,justify=CENTER)
        self.m1accelentry.grid(row=2,column=1)

        m2label = Label(traverse,text='Motor 2',font=labelFont)
        m2label.grid(row=3,column=0,columnspan=3,sticky=W,pady=10)

        m2speedlab = Label(traverse,text='Speed',font=labelFont)
        m2speedlab.grid(row=4,column=0)
        m2speedunlab = Label(traverse,text='Hz',font=labelFont)
        m2speedunlab.grid(row=4,column=2)
        self.m2SpeedVar = StringVar(master,'-')
        self.m2speedentry = Entry(traverse,width=8,textvariable=self.m2SpeedVar,font=entryFont,justify=CENTER)
        self.m2speedentry.grid(row=4,column=1)

        m2accellab = Label(traverse,text='Accel',font=labelFont)
        m2accellab.grid(row=5,column=0)
        m2accelunlab = Label(traverse,text='Hz/s',font=labelFont)
        m2accelunlab.grid(row=5,column=2)
        self.m2AccelVar = StringVar(master,'-')
        self.m2accelentry = Entry(traverse,width=8,textvariable=self.m2AccelVar,font=entryFont,justify=CENTER)
        self.m2accelentry.grid(row=5,column=1)
        
        #right panel
        rightpan = LabelFrame(frame,text='Controll',padx=5,pady=5,font=frameFont)
        rightpan.pack(side=LEFT,fill=Y,padx=5,pady=5)

        self.saveCfgBtn = Button(rightpan, text="Save setup", command=self.save_settings,font=buttonFont)
        self.saveCfgBtn.pack(side=TOP,padx=2,pady=1,fill=X,expand=1)

        sep1 = Frame(rightpan,height=2,bd=1,relief=SUNKEN)
        sep1.pack(side=TOP,fill=X,padx=3,pady=3)

        self.checkBtn = Button(rightpan, text="Check data", command=self.check_data,font=buttonFont)
        self.checkBtn.pack(side=TOP,padx=2,pady=1,fill=X,expand=1)
        self.saveDataBtn = Button(rightpan, text="Save data", command=self.save_data_to_file,font=buttonFont)
        self.saveDataBtn.pack(side=TOP,padx=2,pady=1,fill=X,expand=1)
        self.loadDataBtn = Button(rightpan, text="Load data", command=self.load_data_from_file,font=buttonFont)
        self.loadDataBtn.pack(side=TOP,padx=2,pady=1,fill=X,expand=1)

        sep2 = Frame(rightpan,height=2,bd=1,relief=SUNKEN)
        sep2.pack(side=TOP,fill=X,padx=3,pady=3)

        self.readButton = Button(rightpan,text='Upload',command=self.data_read,font=buttonFont)
        self.readButton.pack(side=TOP,fill=X,expand=1,pady=1,padx=2)
        self.writeButton = Button(rightpan,text='Download',command=self.data_write,font=buttonFont)
        self.writeButton.pack(side=TOP,fill=X,expand=1,pady=1,padx=2)
        self.eesaveButton = Button(rightpan,text='Save EEPROM',command=self.data_eesave,font=buttonFont)
        self.eesaveButton.pack(side=TOP,fill=X,expand=1,pady=1,padx=2)

        self.default_entry_bg = self.m2speedentry.cget('bg')
        self.default_frame_bg = leftpan.cget('bg')

        self.load_settings()

        if serial_present==False:
            self.comEntry.config(state='disabled')
            self.readButton.config(state='disabled')
            self.writeButton.config(state='disabled')
            messagebox.showwarning('Warning','No serial module found!\nProbably PYSERIAL not installed.')


    def save_settings(self):

        try:
            f = open(configfilename,'w')
            f.write('port={}\n'.format(self.comVar.get()))
            f.write('address={}\n'.format(self.adrVar.get()))
            f.close()
            messagebox.showinfo('Info','Configuration saved to {} file'.format(configfilename))
        except:
            pass

    def load_settings(self):

        if os.path.isfile(configfilename):
            f = open(configfilename)
            for line in f:
                if line[:len('port=')]=='port=':
                    self.comVar.set(line[len('port='):].strip())
                if line[:len('address=')]=='address=':
                    self.adrVar.set(line[len('address='):].strip())


    def test_unsigned_entry(self,entry,var):

        try:
            answ = int(var.get())
            if (answ>0xFFFF) or (answ<0):
                entry.config(bg='red')
            else:
                entry.config(bg=self.default_entry_bg)
                return 0
        except:
            entry.config(bg='red')

        return 1

    def data_test(self):

        err = 0
        err += self.test_unsigned_entry(self.m1speedentry,self.m1SpeedVar)
        err += self.test_unsigned_entry(self.m1accelentry,self.m1AccelVar)
        err += self.test_unsigned_entry(self.m2speedentry,self.m2SpeedVar)
        err += self.test_unsigned_entry(self.m2accelentry,self.m2AccelVar)
        return err

    def data_read(self):

        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return
        
        try:
            with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                err = 0
                answ = comm.readParams(port,adr,4,4)
                if answ[0]!=4:
                    err+=1
                else:
                    self.m1SpeedVar.set('{}'.format(answ[1][0]))
                    self.m1AccelVar.set('{}'.format(answ[1][1]))
                    self.m2SpeedVar.set('{}'.format(answ[1][2]))
                    self.m2AccelVar.set('{}'.format(answ[1][3]))
                if err>0:
                    messagebox.showerror('Error',"Can't read some params")
                    
        except:
            messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))

        self.data_test()
        

    def data_write(self):

        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return
        
        if self.data_test()==0:

            try:
                with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                    err = 0
                    answ = comm.writeParams(port,adr,4,4,[int(self.m1SpeedVar.get())&0xFFFF,int(self.m1AccelVar.get())&0xFFFF,
                                                        int(self.m2SpeedVar.get())&0xFFFF,int(self.m2AccelVar.get())&0xFFFF])
                    if answ!='OK':
                        err+=1
                    if err>0:
                        messagebox.showerror('Error',"Can't write some params")
            except:
                messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))      
        else:
            messagebox.showerror('Error',"Some params are set incorrectly")

    def data_eesave(self):
        try:
            with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                answ = comm.writeParams(port,1,1,1,[0x0100,])
                if answ!='OK':
                    messagebox.showerror('Error',"No answer to Save EEPROM command")
        except:
            messagebox.showerror('Error',"Can't open serial port {}".format(self.comVar.get()))
        
    def check_data(self):

        self.data_test()
        
        print('{} {}'.format(self.m1SpeedVar.get(),self.m1AccelVar.get()))
        print('{} {}'.format(self.m2SpeedVar.get(),self.m2AccelVar.get()))

    def save_data_to_file(self):

        filename = filedialog.asksaveasfilename(filetypes = (("data files","*.dat"),))

        if filename:
            if filename.split('.')[-1]!='dat':
                filename = '{}.dat'.format(filename)
            f=open(filename,'w')
            f.write('[traverse]\n')
            f.write('m1_speed={}\n'.format(self.m1SpeedVar.get()))
            f.write('m1_accel={}\n'.format(self.m1AccelVar.get()))
            f.write('m2_speed={}\n'.format(self.m2SpeedVar.get()))
            f.write('m2_accel={}\n'.format(self.m2AccelVar.get()))

    def load_data_from_file(self):

        filename = filedialog.askopenfilename(filetypes = (("data files","*.dat"),))

        if filename:
            group = ''
            f = open(filename,'r')
            for line in f:
                gs1p = line.find('[')
                gs2p = line.find(']')
                if (gs1p>-1) and (gs2p>gs1p):
                    group = line[gs1p+1:gs2p]
                else:
                    if group=='traverse':
                        if line[:len('m1_speed=')]=='m1_speed=':
                                self.m1SpeedVar.set(line[len('m1_speed='):].strip())
                        elif line[:len('m1_accel=')]=='m1_accel=':
                                self.m1AccelVar.set(line[len('m1_accel='):].strip())
                        elif line[:len('m2_speed=')]=='m2_speed=':
                                self.m2SpeedVar.set(line[len('m2_speed='):].strip())
                        elif line[:len('m2_accel=')]=='m2_accel=':
                                self.m2AccelVar.set(line[len('m2_accel='):].strip())

if __name__=="__main__":

    root = Tk()
    app = app(root)
    root.mainloop()
