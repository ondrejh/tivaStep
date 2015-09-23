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

TIME_TURN_S = 86400
STEPS_TURN = 2304000

configfilename = 'setup.cfg'

frameFont = ("Arial",12)
labelFont = ("Arial",10)
entryFont = ("Courier",12)
spinFont = ("Courier",24)
buttonFont = ("Courier",14)

def get_signed_32bit(v16bM,v16bL):

    v32b = (v16bM<<16)|v16bL;
    if (v32b&0x80000000)!=0:
        v32b=-(v32b^0xFFFFFFFF)-1
    return v32b

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

        #time
        timefrm = LabelFrame(leftpan,text='Time',pady=5,padx=5,font=frameFont)
        timefrm.pack(side=TOP,padx=5,pady=5,fill=X,expand=1)

        getTimeBtn = Button(timefrm, text="Read", command=self.get_time,font=buttonFont)
        getTimeBtn.grid(row=0,column=0,sticky=W+E,pady=10)
        setTimeBtn = Button(timefrm, text="Set", command=self.set_time,font=buttonFont)
        setTimeBtn.grid(row=0,column=1,columnspan=2,sticky=W+E,pady=10)        
        self.TimeVar = StringVar(master,'-')
        self.timeEntry = Entry(timefrm,width=8,textvariable=self.TimeVar,font=entryFont,justify=CENTER)
        self.timeEntry.grid(row=1,column=0,columnspan=2)
        timeunlab = Label(timefrm,text='s',font=labelFont)
        timeunlab.grid(row=1,column=2)

        #fastscroll
        fastscr = LabelFrame(leftpan,text='Scroll X',pady=5,padx=5,font=frameFont)
        fastscr.pack(side=TOP,padx=5,pady=5,fill=X,expand=1)

        btnStartScr = Button(fastscr,text='Start',command=self.start_scroll,font=buttonFont)
        btnStartScr.grid(row=0,column=0,sticky=W+E)
        btnStopScr = Button(fastscr,text='Stop',command=self.stop_scroll,font=buttonFont)
        btnStopScr.grid(row=0,column=1,sticky=W+E)
                
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

        #position
        position = LabelFrame(frame,text='Position',pady=5,padx=5,font=frameFont)
        position.pack(side=LEFT,padx=5,pady=5,fill=Y,expand=1)

        actlabel = Label(position,text='Actual:',font=labelFont)
        actlabel.grid(row=0,column=0,columnspan=2,sticky=W,pady=10)

        m1poslab = Label(position,text='X',font=labelFont)
        m1poslab.grid(row=1,column=0)
        m1posunlab = Label(position,text='Steps',font=labelFont)
        m1posunlab.grid(row=1,column=2)
        self.m1PosVar = StringVar(master,'-')
        self.m1posentry = Entry(position,width=8,textvariable=self.m1PosVar,font=entryFont,justify=CENTER,state=DISABLED)
        self.m1posentry.grid(row=1,column=1)

        m2poslab = Label(position,text='Y',font=labelFont)
        m2poslab.grid(row=2,column=0)
        m2posunlab = Label(position,text='Steps',font=labelFont)
        m2posunlab.grid(row=2,column=2)
        self.m2PosVar = StringVar(master,'-')
        self.m2posentry = Entry(position,width=8,textvariable=self.m2PosVar,font=entryFont,justify=CENTER,state=DISABLED)
        self.m2posentry.grid(row=2,column=1)

        self.getPosBtn = Button(position, text="Read", command=self.get_position,font=buttonFont)
        self.getPosBtn.grid(row=0,column=2,pady=5)

        sep1 = Frame(position,height=2,bd=1,relief=SUNKEN)
        sep1.grid(row=3,column=0,columnspan=3,sticky=W+E,padx=3,pady=10)

        gotlabel = Label(position,text='Desired:',font=labelFont)
        gotlabel.grid(row=4,column=0,columnspan=2,sticky=W,pady=10)

        m1gotlab = Label(position,text='X',font=labelFont)
        m1gotlab.grid(row=5,column=0)
        m1gotunlab = Label(position,text='Steps',font=labelFont)
        m1gotunlab.grid(row=5,column=2)
        self.m1GotVar = StringVar(master,'-')
        self.m1gotentry = Entry(position,width=8,textvariable=self.m1GotVar,font=entryFont,justify=CENTER,state=DISABLED)
        self.m1gotentry.grid(row=5,column=1)

        m2gotlab = Label(position,text='Y',font=labelFont)
        m2gotlab.grid(row=6,column=0)
        m2gotunlab = Label(position,text='Steps',font=labelFont)
        m2gotunlab.grid(row=6,column=2)
        self.m2GotVar = StringVar(master,'-')
        self.m2gotentry = Entry(position,width=8,textvariable=self.m2GotVar,font=entryFont,justify=CENTER,state=DISABLED)
        self.m2gotentry.grid(row=6,column=1)

        sep2 = Frame(position,height=2,bd=1,relief=SUNKEN)
        sep2.grid(row=7,column=0,columnspan=3,sticky=W+E,padx=3,pady=10)

        tracklabel = Label(position,text='Tracking:',font=labelFont)
        tracklabel.grid(row=8,column=0,columnspan=2,sticky=W,pady=10)
        self.gotoBtn = Button(position, text="Set", command=self.goto_position,font=buttonFont)
        self.gotoBtn.grid(row=8,column=2,pady=5)

        m1lab = Label(position,text='X:',font=labelFont)
        m1lab.grid(row=9,column=0,sticky=W)
        m1tZeroLab = Label(position,text='tZero',font=labelFont)
        m1tZeroLab.grid(row=10,column=0,sticky=E)
        m1tZeroUnLab = Label(position,text='s',font=labelFont)
        m1tZeroUnLab.grid(row=10,column=2,sticky=W)
        self.m1tZeroVar = StringVar(master,'-')
        self.m1tZeroEntry = Entry(position,width=8,textvariable=self.m1tZeroVar,font=entryFont,justify=CENTER)
        self.m1tZeroEntry.grid(row=10,column=1)
        m1sZeroLab = Label(position,text='sZero',font=labelFont)
        m1sZeroLab.grid(row=11,column=0,sticky=E)
        m1sZeroUnLab = Label(position,text='steps',font=labelFont)
        m1sZeroUnLab.grid(row=11,column=2,sticky=W)
        self.m1sZeroVar = StringVar(master,'-')
        self.m1sZeroEntry = Entry(position,width=8,textvariable=self.m1sZeroVar,font=entryFont,justify=CENTER)
        self.m1sZeroEntry.grid(row=11,column=1)
        m1sTurnLab = Label(position,text='sTurn',font=labelFont)
        m1sTurnLab.grid(row=12,column=0,sticky=E)
        m1sTurnUnLab = Label(position,text='steps',font=labelFont)
        m1sTurnUnLab.grid(row=12,column=2,sticky=W)
        self.m1sTurnVar = StringVar(master,'-')
        self.m1sTurnEntry = Entry(position,width=8,textvariable=self.m1sTurnVar,font=entryFont,justify=CENTER)
        self.m1sTurnEntry.grid(row=12,column=1)
        m1tTurnLab = Label(position,text='tTurn',font=labelFont)
        m1tTurnLab.grid(row=13,column=0,sticky=E)
        m1tTurnUnLab = Label(position,text='s',font=labelFont)
        m1tTurnUnLab.grid(row=13,column=2,sticky=W)
        self.m1tTurnVar = StringVar(master,'-')
        self.m1tTurnEntry = Entry(position,width=8,textvariable=self.m1tTurnVar,font=entryFont,justify=CENTER)
        self.m1tTurnEntry.grid(row=13,column=1)
        
        m2lab = Label(position,text='Y:',font=labelFont)
        m2lab.grid(row=15,column=0,sticky=W)
        m2tZeroLab = Label(position,text='tZero',font=labelFont)
        m2tZeroLab.grid(row=16,column=0,sticky=E)
        m2tZeroUnLab = Label(position,text='s',font=labelFont)
        m2tZeroUnLab.grid(row=16,column=2,sticky=W)
        self.m2tZeroVar = StringVar(master,'-')
        self.m2tZeroEntry = Entry(position,width=8,textvariable=self.m2tZeroVar,font=entryFont,justify=CENTER)
        self.m2tZeroEntry.grid(row=16,column=1)
        m2sZeroLab = Label(position,text='sZero',font=labelFont)
        m2sZeroLab.grid(row=17,column=0,sticky=E)
        m2sZeroUnLab = Label(position,text='steps',font=labelFont)
        m2sZeroUnLab.grid(row=17,column=2,sticky=W)
        self.m2sZeroVar = StringVar(master,'-')
        self.m2sZeroEntry = Entry(position,width=8,textvariable=self.m2sZeroVar,font=entryFont,justify=CENTER)
        self.m2sZeroEntry.grid(row=17,column=1)
        m2sTurnLab = Label(position,text='sTurn',font=labelFont)
        m2sTurnLab.grid(row=18,column=0,sticky=E)
        m2sTurnUnLab = Label(position,text='steps',font=labelFont)
        m2sTurnUnLab.grid(row=18,column=2,sticky=W)
        self.m2sTurnVar = StringVar(master,'-')
        self.m2sTurnEntry = Entry(position,width=8,textvariable=self.m2sTurnVar,font=entryFont,justify=CENTER)
        self.m2sTurnEntry.grid(row=18,column=1)
        m2tTurnLab = Label(position,text='tTurn',font=labelFont)
        m2tTurnLab.grid(row=19,column=0,sticky=E)
        m2tTurnUnLab = Label(position,text='s',font=labelFont)
        m2tTurnUnLab.grid(row=19,column=2,sticky=W)
        self.m2tTurnVar = StringVar(master,'-')
        self.m2tTurnEntry = Entry(position,width=8,textvariable=self.m2tTurnVar,font=entryFont,justify=CENTER)
        self.m2tTurnEntry.grid(row=19,column=1)

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

        self.default_entry_bg = self.m2speedentry.cget('bg')
        self.default_frame_bg = leftpan.cget('bg')

        self.load_settings()

        if serial_present==False:
            self.comEntry.config(state='disabled')
            self.readButton.config(state='disabled')
            self.writeButton.config(state='disabled')
            messagebox.showwarning('Warning','No serial module found!\nProbably PYSERIAL not installed.')

    def start_scroll(self):

        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return
        pass

        tim = 0
        pos = 0
        
        try:
            with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                err = 0
                answ = comm.readParams(port,adr,10,4)
                if answ[0]!=4:
                    err+=1
                else:
                    tim = get_signed_32bit(answ[1][1],answ[1][0])
                    self.TimeVar.set('{}'.format(tim))
                    pos = get_signed_32bit(answ[1][3],answ[1][2])
                    self.m1PosVar.set('{}'.format(pos))
                if err>0:
                    messagebox.showerror('Error',"Can't read some necessary values")
                    return
                tZero = tim
                sZero = -pos
                tTurn = TIME_TURN_S
                sTurn = STEPS_TURN
                answ = comm.writeParams(port,adr,20,2,[tZero&0xFFFF,(tZero>>16)&0xFFFF])
                if answ!='OK':
                    err+=1
                else:
                    self.m1tZeroVar.set('{}'.format(tZero))
                answ = comm.writeParams(port,adr,24,2,[sZero&0xFFFF,(sZero>>16)&0xFFFF])
                if answ!='OK':
                    err+=1
                else:
                    self.m1sZeroVar.set('{}'.format(sZero))
                answ = comm.writeParams(port,adr,28,2,[sTurn&0xFFFF,(sTurn>>16)&0xFFFF])
                if answ!='OK':
                    err+=1
                else:
                    self.m1sTurnVar.set('{}'.format(sTurn))
                answ = comm.writeParams(port,adr,32,2,[tTurn&0xFFFF,(tTurn>>16)&0xFFFF])
                if answ!='OK':
                    err+=1
                else:
                    self.m1tTurnVar.set('{}'.format(tTurn))
                if err>0:
                    messagebox.showerror('Error',"Can't write some params")
        except:
            messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))
            return

    def stop_scroll(self):
        
        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return

        pos = 0
        
        try:
            with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                err = 0
                answ = comm.readParams(port,adr,12,2)
                if answ[0]!=2:
                    err+=1
                else:
                    pos = get_signed_32bit(answ[1][1],answ[1][0])
                    self.m1PosVar.set('{}'.format(pos))
                if err>0:
                    messagebox.showerror('Error',"Can't read some necessary values")
                    return
                sZero = pos
                tTurn = 0
                answ = comm.writeParams(port,adr,24,2,[sZero&0xFFFF,(sZero>>16)&0xFFFF])
                if answ!='OK':
                    err+=1
                else:
                    self.m1sZeroVar.set('{}'.format(sZero))
                answ = comm.writeParams(port,adr,32,2,[tTurn&0xFFFF,(tTurn>>16)&0xFFFF])
                if answ!='OK':
                    err+=1
                else:
                    self.m1tTurnVar.set('{}'.format(tTurn))
                if err>0:
                    messagebox.showerror('Error',"Can't write some params")
        except:
            messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))
            return
        
    def get_time(self):
        
        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return

        try:
            with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                err = 0
                answ = comm.readParams(port,adr,10,2)
                if answ[0]!=2:
                    err+=1
                else:
                    self.TimeVar.set('{}'.format(get_signed_32bit(answ[1][1],answ[1][0])))
                if err>0:
                    messagebox.showerror('Error',"Can't read time value")
        except:
            messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))

    def set_time(self):

        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return

        t = 0

        try:
            t = int(self.TimeVar.get())
        except:
            t = None

        if t!=None:
            try:
                if t==0:
                    t=-1 #setting 0 doesn't work
                with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                    err = 0
                    answ = comm.writeParams(port,adr,8,2,[t&0xFFFF,(t>>16)&0xFFFF])
                    if answ!='OK':
                        err+=1
                    if err>0:
                        messagebox.showerror('Error',"Can't set time value")
            except:
                messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))
        else:
            messagebox.showerror('Error',message="Incorrect time value format")

    def goto_position(self):
        
        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return

        t1zero = 0
        t2zero = 0
        s1zero = 0
        s2zero = 0
        s1turn = 0
        s2turn = 0
        t1turn = 0
        t2turn = 0

        try:
            t1zero = int(self.m1tZeroVar.get())
            t2zero = int(self.m2tZeroVar.get())
            s1zero = int(self.m1sZeroVar.get())
            s2zero = int(self.m2sZeroVar.get())
            s1turn = int(self.m1sTurnVar.get())
            s2turn = int(self.m2sTurnVar.get())
            t1turn = int(self.m1tTurnVar.get())
            t2turn = int(self.m2tTurnVar.get())
        except:
            t1zero = None

        if t1zero!=None:
            try:
                with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                    err = 0
                    answ = comm.writeParams(port,adr,20,16,[t1zero&0xFFFF,(t1zero>>16)&0xFFFF,
                                                            t2zero&0xFFFF,(t2zero>>16)&0xFFFF,
                                                            s1zero&0xFFFF,(s1zero>>16)&0xFFFF,
                                                            s2zero&0xFFFF,(s2zero>>16)&0xFFFF,
                                                            s1turn&0xFFFF,(s1turn>>16)&0xFFFF,
                                                            s2turn&0xFFFF,(s2turn>>16)&0xFFFF,
                                                            t1turn&0xFFFF,(t1turn>>16)&0xFFFF,
                                                            t2turn&0xFFFF,(t2turn>>16)&0xFFFF])
                    if answ!='OK':
                        err+=1
                    if err>0:
                        messagebox.showerror('Error',"Can't write some params")
            except:
                messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))
        else:
            messagebox.showerror('Error',message="The desired position syntax error")

    def get_position(self):

        adr = 1
        try:
            adr = int(self.adrVar.get())
        except:
            messagebox.showerror('Error',message="Incorrect address format ".format(self.adrVar.get()))
            return

        try:
            with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                err = 0
                answ = comm.readParams(port,adr,12,24)
                if answ[0]!=24:
                    err+=1
                else:
                    self.m1PosVar.set('{}'.format(get_signed_32bit(answ[1][1],answ[1][0])))
                    self.m2PosVar.set('{}'.format(get_signed_32bit(answ[1][3],answ[1][2])))
                    self.m1GotVar.set('{}'.format(get_signed_32bit(answ[1][5],answ[1][4])))
                    self.m2GotVar.set('{}'.format(get_signed_32bit(answ[1][7],answ[1][6])))
                    self.m1tZeroVar.set('{}'.format(get_signed_32bit(answ[1][9],answ[1][8])))
                    self.m2tZeroVar.set('{}'.format(get_signed_32bit(answ[1][11],answ[1][10])))
                    self.m1sZeroVar.set('{}'.format(get_signed_32bit(answ[1][13],answ[1][12])))
                    self.m2sZeroVar.set('{}'.format(get_signed_32bit(answ[1][15],answ[1][14])))
                    self.m1sTurnVar.set('{}'.format(get_signed_32bit(answ[1][17],answ[1][16])))
                    self.m2sTurnVar.set('{}'.format(get_signed_32bit(answ[1][19],answ[1][18])))
                    self.m1tTurnVar.set('{}'.format(get_signed_32bit(answ[1][21],answ[1][20])))
                    self.m2tTurnVar.set('{}'.format(get_signed_32bit(answ[1][23],answ[1][22])))
                if err>0:
                    messagebox.showerror('Error',"Can't read some params")
        except:
            messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))

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

    def test_signed32b_entry(self,entry,var):

        try:
            answ = int(var.get())
            if (answ>0x7FFFFFFF) or (answ<-0x80000000):
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
        err += self.test_signed32b_entry(self.m1tZeroEntry,self.m1tZeroVar)
        err += self.test_signed32b_entry(self.m2tZeroEntry,self.m2tZeroVar)
        err += self.test_signed32b_entry(self.m1sZeroEntry,self.m1sZeroVar)
        err += self.test_signed32b_entry(self.m2sZeroEntry,self.m2sZeroVar)
        err += self.test_signed32b_entry(self.m1sTurnEntry,self.m1sTurnVar)
        err += self.test_signed32b_entry(self.m2sTurnEntry,self.m2sTurnVar)
        err += self.test_signed32b_entry(self.m1tTurnEntry,self.m1tTurnVar)
        err += self.test_signed32b_entry(self.m2tTurnEntry,self.m2tTurnVar)
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

                #time
                answ = comm.readParams(port,adr,10,2)
                if answ[0]!=2:
                    err+=1
                else:
                    self.TimeVar.set('{}'.format(get_signed_32bit(answ[1][1],answ[1][0])))

                #speed & acceleration
                answ = comm.readParams(port,adr,4,4)
                if answ[0]!=4:
                    err+=1
                else:
                    self.m1SpeedVar.set('{}'.format(answ[1][0]))
                    self.m1AccelVar.set('{}'.format(answ[1][1]))
                    self.m2SpeedVar.set('{}'.format(answ[1][2]))
                    self.m2AccelVar.set('{}'.format(answ[1][3]))

                #position & tracking                    
                answ = comm.readParams(port,adr,12,24)
                if answ[0]!=24:
                    err+=1
                else:
                    self.m1PosVar.set('{}'.format(get_signed_32bit(answ[1][1],answ[1][0])))
                    self.m2PosVar.set('{}'.format(get_signed_32bit(answ[1][3],answ[1][2])))
                    self.m1GotVar.set('{}'.format(get_signed_32bit(answ[1][5],answ[1][4])))
                    self.m2GotVar.set('{}'.format(get_signed_32bit(answ[1][7],answ[1][6])))
                    self.m1tZeroVar.set('{}'.format(get_signed_32bit(answ[1][9],answ[1][8])))
                    self.m2tZeroVar.set('{}'.format(get_signed_32bit(answ[1][11],answ[1][10])))
                    self.m1sZeroVar.set('{}'.format(get_signed_32bit(answ[1][13],answ[1][12])))
                    self.m2sZeroVar.set('{}'.format(get_signed_32bit(answ[1][15],answ[1][14])))
                    self.m1sTurnVar.set('{}'.format(get_signed_32bit(answ[1][17],answ[1][16])))
                    self.m2sTurnVar.set('{}'.format(get_signed_32bit(answ[1][19],answ[1][18])))
                    self.m1tTurnVar.set('{}'.format(get_signed_32bit(answ[1][21],answ[1][20])))
                    self.m2tTurnVar.set('{}'.format(get_signed_32bit(answ[1][23],answ[1][22])))
                    
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
                t1zero = int(self.m1tZeroVar.get())
                t2zero = int(self.m2tZeroVar.get())
                s1zero = int(self.m1sZeroVar.get())
                s2zero = int(self.m2sZeroVar.get())
                s1turn = int(self.m1sTurnVar.get())
                s2turn = int(self.m2sTurnVar.get())
                t1turn = int(self.m1tTurnVar.get())
                t2turn = int(self.m2tTurnVar.get())

                with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                    err = 0
                    answ = comm.writeParams(port,adr,4,4,[int(self.m1SpeedVar.get())&0xFFFF,int(self.m1AccelVar.get())&0xFFFF,
                                                          int(self.m2SpeedVar.get())&0xFFFF,int(self.m2AccelVar.get())&0xFFFF])
                    if answ!='OK':
                        err+=1

                    answ = comm.writeParams(port,adr,20,16,[t1zero&0xFFFF,(t1zero>>16)&0xFFFF,
                                                            t2zero&0xFFFF,(t2zero>>16)&0xFFFF,
                                                            s1zero&0xFFFF,(s1zero>>16)&0xFFFF,
                                                            s2zero&0xFFFF,(s2zero>>16)&0xFFFF,
                                                            s1turn&0xFFFF,(s1turn>>16)&0xFFFF,
                                                            s2turn&0xFFFF,(s2turn>>16)&0xFFFF,
                                                            t1turn&0xFFFF,(t1turn>>16)&0xFFFF,
                                                            t2turn&0xFFFF,(t2turn>>16)&0xFFFF])
                    if answ!='OK':
                        err+=1

                    if err>0:
                        messagebox.showerror('Error',"Can't write some params")
            except:
                messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))      
        else:
            messagebox.showerror('Error',"Some params are set incorrectly")

    def check_data(self):

        self.data_test()

    def save_data_to_file(self):

        filename = filedialog.asksaveasfilename(filetypes = (("data files","*.dat"),))

        if filename:
            if filename.split('.')[-1]!='dat':
                filename = '{}.dat'.format(filename)
            f=open(filename,'w')
            f.write('[time]\n')
            f.write('time_val={}\n'.format(self.TimeVar.get()))
            f.write('[traverse]\n')
            f.write('m1_speed={}\n'.format(self.m1SpeedVar.get()))
            f.write('m1_accel={}\n'.format(self.m1AccelVar.get()))
            f.write('m2_speed={}\n'.format(self.m2SpeedVar.get()))
            f.write('m2_accel={}\n'.format(self.m2AccelVar.get()))
            f.write('[tracking]\n')
            f.write('m1_tzero={}\n'.format(self.m1tZeroVar.get()))
            f.write('m1_szero={}\n'.format(self.m1sZeroVar.get()))
            f.write('m1_sturn={}\n'.format(self.m1sTurnVar.get()))
            f.write('m1_tturn={}\n'.format(self.m1tTurnVar.get()))
            f.write('m2_tzero={}\n'.format(self.m2tZeroVar.get()))
            f.write('m2_szero={}\n'.format(self.m2sZeroVar.get()))
            f.write('m2_sturn={}\n'.format(self.m2sTurnVar.get()))
            f.write('m2_tturn={}\n'.format(self.m2tTurnVar.get()))
            f.close()
            

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
                    elif group=='time':
                        if line[:len('time_val=')]=='time_val=':
                                self.TimeVar.set(line[len('time_val='):].strip())
                    elif group=='tracking':
                        if line[:len('m1_tzero=')]=='m1_tzero=':
                                self.m1tZeroVar.set(line[len('m1_tzero='):].strip())
                        elif line[:len('m2_tzero=')]=='m2_tzero=':
                            self.m2tZeroVar.set(line[len('m2_tzero='):].strip())
                        elif line[:len('m1_szero=')]=='m1_szero=':
                            self.m1sZeroVar.set(line[len('m1_szero='):].strip())
                        elif line[:len('m2_szero=')]=='m2_szero=':
                            self.m2sZeroVar.set(line[len('m2_szero='):].strip())
                        elif line[:len('m1_sturn=')]=='m1_sturn=':
                            self.m1sTurnVar.set(line[len('m1_sturn='):].strip())
                        elif line[:len('m2_sturn=')]=='m2_sturn=':
                            self.m2sTurnVar.set(line[len('m2_sturn='):].strip())
                        elif line[:len('m1_tturn=')]=='m1_tturn=':
                            self.m1tTurnVar.set(line[len('m1_tturn='):].strip())
                        elif line[:len('m2_tturn=')]=='m2_tturn=':
                            self.m2tTurnVar.set(line[len('m2_tturn='):].strip())
            f.close()

if __name__=="__main__":

    root = Tk()
    app = app(root)
    root.mainloop()
