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
entryFont = ("Courier",14)
spinFont = ("Courier",14,"bold")
buttonFont = ("Courier",10,"bold")

class app:

    def __init__(self,master):

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
        comlabel.grid(row=0,column=0,pady=3)
        self.comVar = StringVar(master,'COM1')
        self.comEntry = Entry(companel,width=10,textvariable=self.comVar,font=entryFont,justify=CENTER)
        self.comEntry.grid(row=0,column=1,pady=3)
        
        #traverse
        traverse = LabelFrame(leftpan,text='Traverse',pady=5,padx=5,font=frameFont)
        traverse.pack(side=TOP,padx=5,pady=5,fill=Y,expand=1)

        fdlabel = Label(traverse,text='Feeding',font=labelFont)
        fdlabel.grid(row=0,column=0,columnspan=3,sticky=W)

        fdspeedlab = Label(traverse,text='Speed',font=labelFont)
        fdspeedlab.grid(row=1,column=0)
        fdspeedunlab = Label(traverse,text='Hz',font=labelFont)
        fdspeedunlab.grid(row=1,column=2)
        self.fdSpeedVar = StringVar(master,'-')
        self.fdspeedentry = Entry(traverse,width=8,textvariable=self.fdSpeedVar,font=entryFont,justify=CENTER)
        self.fdspeedentry.grid(row=1,column=1)

        fdaccellab = Label(traverse,text='Accel',font=labelFont)
        fdaccellab.grid(row=2,column=0)
        fdaccelunlab = Label(traverse,text='Hz/s',font=labelFont)
        fdaccelunlab.grid(row=2,column=2)
        self.fdAccelVar = StringVar(master,'-')
        self.fdaccelentry = Entry(traverse,width=8,textvariable=self.fdAccelVar,font=entryFont,justify=CENTER)
        self.fdaccelentry.grid(row=2,column=1)

        lflabel = Label(traverse,text='Lifting',font=labelFont)
        lflabel.grid(row=3,column=0,columnspan=3,sticky=W)

        lfspeedlab = Label(traverse,text='Speed',font=labelFont)
        lfspeedlab.grid(row=4,column=0)
        lfspeedunlab = Label(traverse,text='Hz',font=labelFont)
        lfspeedunlab.grid(row=4,column=2)
        self.lfSpeedVar = StringVar(master,'-')
        self.lfspeedentry = Entry(traverse,width=8,textvariable=self.lfSpeedVar,font=entryFont,justify=CENTER)
        self.lfspeedentry.grid(row=4,column=1)

        lfaccellab = Label(traverse,text='Accel',font=labelFont)
        lfaccellab.grid(row=5,column=0)
        lfaccelunlab = Label(traverse,text='Hz/s',font=labelFont)
        lfaccelunlab.grid(row=5,column=2)
        self.lfAccelVar = StringVar(master,'-')
        self.lfaccelentry = Entry(traverse,width=8,textvariable=self.lfAccelVar,font=entryFont,justify=CENTER)
        self.lfaccelentry.grid(row=5,column=1)
        
        #start
        panel1 = LabelFrame(frame,text='Start programme',padx=5,pady=5,font=frameFont)
        panel1.pack(side=LEFT,fill=Y,padx=5,pady=5)

        self.set1frm = LabelFrame(panel1,text='Feeding',relief=FLAT,font=frameFont)
        self.set1frm.pack(side=LEFT)
        self.val1 = self.gui_create_var_grid(3,8,master)
        self.set1 = self.gui_create_set(self.set1frm,self.val1)
        self.set1.pack(padx=5,pady=5)

        self.set2frm = LabelFrame(panel1,text='Lifting',relief=FLAT,font=frameFont)
        self.set2frm.pack(side=LEFT)
        self.val2 = self.gui_create_var_grid(3,8,master)
        self.set2 = self.gui_create_set(self.set2frm,self.val2)
        self.set2.pack(padx=5,pady=5,side=LEFT)

        #stop
        panel2 = LabelFrame(frame,text='Stop programme',padx=5,pady=5,font=frameFont)
        panel2.pack(side=LEFT,fill=Y,padx=5,pady=5)

        self.set3frm = LabelFrame(panel2,text='Feeding',relief=FLAT,font=frameFont)
        self.set3frm.pack(side=LEFT)
        self.val3 = self.gui_create_var_grid(3,8,master)
        self.set3 = self.gui_create_set(self.set3frm,self.val3)
        self.set3.pack(padx=5,pady=5,side=LEFT)

        self.set4frm = LabelFrame(panel2,text='Lifting',relief=FLAT,font=frameFont)
        self.set4frm.pack(side=LEFT)
        self.val4 = self.gui_create_var_grid(3,8,master)
        self.set4 = self.gui_create_set(self.set4frm,self.val4)
        self.set4.pack(padx=5,pady=5,side=LEFT)

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

        self.default_entry_bg = self.lfspeedentry.cget('bg')
        self.default_frame_bg = self.set1frm.cget('bg')

        self.load_settings()

        if serial_present==False:
            self.comEntry.config(state='disabled')
            self.readButton.config(state='disabled')
            self.writeButton.config(state='disabled')
            messagebox.showwarning('Warning','No serial module found!\nProbably PYSERIAL not installed.')


    def gui_create_set(self,frame,values):

        setOut = Frame(frame)
        fsb = []
        val1 = []
        val2 = []
        
        for i in range(8):
            fsb.append(Spinbox(setOut,from_=0,to=3,width=3,state='readonly',textvariable=values[i][0],font=spinFont,justify=CENTER))
            fsb[-1].grid(row=i,column=0)
            val1.append(Entry(setOut,width=7,textvariable=values[i][1],font=entryFont,justify=CENTER))
            val1[-1].grid(row=i,column=1)
            val2.append(Entry(setOut,width=7,textvariable=values[i][2],font=entryFont,justify=CENTER))
            val2[-1].grid(row=i,column=2)

        return setOut


    def gui_create_var_grid(self,x,y,master):

        varGrid = []

        for j in range(y):
            row = []
            for i in range(x):
                row.append(StringVar(master))
                if i==0:
                    row[-1].set('0')
                else:
                    row[-1].set('-')
            varGrid.append(row)

        return varGrid

    def save_settings(self):

        try:
            f = open(configfilename,'w')
            f.write('port={}'.format(self.comVar.get()))
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

    def test_data_grid(self,data_grid,data_var):

        err = 0
        for row in data_var:
            try:
                answ = int(row[0].get())
                if (answ>3) or (answ<0):
                    err+=1
                if (answ==0):
                    break
                answ = int(row[1].get())
                if (answ>0x3FFF) or (answ<0):
                    err+=1
                answ = int(row[2].get())
                if (answ>0x7FFF) or (answ<-0x8000):
                    err+=1
            except:
                err+=1

        if err==0:
            data_grid.config(bg=self.default_frame_bg)
        else:
            data_grid.config(bg='red')

        return err

    def data_test(self):

        err = 0
        err += self.test_unsigned_entry(self.fdspeedentry,self.fdSpeedVar)
        err += self.test_unsigned_entry(self.fdaccelentry,self.fdAccelVar)
        err += self.test_unsigned_entry(self.lfspeedentry,self.lfSpeedVar)
        err += self.test_unsigned_entry(self.lfaccelentry,self.lfAccelVar)
        err += self.test_data_grid(self.set1frm,self.val1)
        err += self.test_data_grid(self.set2frm,self.val2)
        err += self.test_data_grid(self.set3frm,self.val3)
        err += self.test_data_grid(self.set4frm,self.val4)
        return err

    def data_read(self):

        try:
            with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                err = 0
                answ = comm.readParams(port,1,4,4)
                if answ[0]!=4:
                    err+=1
                else:
                    self.fdSpeedVar.set('{}'.format(answ[1][0]))
                    self.fdAccelVar.set('{}'.format(answ[1][1]))
                    self.lfSpeedVar.set('{}'.format(answ[1][2]))
                    self.lfAccelVar.set('{}'.format(answ[1][3]))
                answ = comm.readParams(port,1,8,16)
                if answ[0]!=16:
                    err+=1
                else:
                    for i in range(8):
                        v0 = answ[1][i*2]
                        v1 = answ[1][i*2+1]
                        if v1>0x7FFF:
                            v1 -= 0x10000
                        self.val2[i][0].set('{}'.format((v0&0xFFFF)>>14))
                        self.val2[i][1].set('{}'.format(v0&0x3FFF))
                        self.val2[i][2].set('{}'.format(v1))
                answ = comm.readParams(port,1,26,16)
                if answ[0]!=16:
                    err+=1
                else:
                    for i in range(8):
                        v0 = answ[1][i*2]
                        v1 = answ[1][i*2+1]
                        if v1>0x7FFF:
                            v1 -= 0x10000
                        self.val1[i][0].set('{}'.format((v0&0xFFFF)>>14))
                        self.val1[i][1].set('{}'.format(v0&0x3FFF))
                        self.val1[i][2].set('{}'.format(v1))
                answ = comm.readParams(port,1,44,16)
                if answ[0]!=16:
                    err+=1
                else:
                    for i in range(8):
                        v0 = answ[1][i*2]
                        v1 = answ[1][i*2+1]
                        if v1>0x7FFF:
                            v1 -= 0x10000
                        self.val4[i][0].set('{}'.format((v0&0xFFFF)>>14))
                        self.val4[i][1].set('{}'.format(v0&0x3FFF))
                        self.val4[i][2].set('{}'.format(v1))
                answ = comm.readParams(port,1,62,16)
                if answ[0]!=16:
                    err+=1
                else:
                    for i in range(8):
                        v0 = answ[1][i*2]
                        v1 = answ[1][i*2+1]
                        if v1>0x7FFF:
                            v1 -= 0x10000
                        self.val3[i][0].set('{}'.format((v0&0xFFFF)>>14))
                        self.val3[i][1].set('{}'.format(v0&0x3FFF))
                        self.val3[i][2].set('{}'.format(v1))
                if err>0:
                    messagebox.showerror('Error',"Can't read some params")
                    
        except:
            messagebox.showerror('Error',message="Can't open serial port {}".format(self.comVar.get()))

        self.data_test()
        

    def get_patern(self,varGrid):

        patern=[]
        for d in varGrid:
            a=int(d[0].get())
            if a==0:
                patern.append([0,0,0])
                break
            b=int(d[1].get())
            c=int(d[2].get())
            patern.append([a,b,c])
        return patern
    

    def data_write(self):

        if self.data_test()==0:

            try:
                with serial.Serial(self.comVar.get(),comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
                    err = 0
                    answ = comm.writeParams(port,1,4,4,[int(self.fdSpeedVar.get())&0xFFFF,int(self.fdAccelVar.get())&0xFFFF,
                                                        int(self.lfSpeedVar.get())&0xFFFF,int(self.lfAccelVar.get())&0xFFFF])
                    if answ!='OK':
                        err+=1
                    patern=self.get_patern(self.val1)
                    patern=comm.recalc_patern(patern)
                    answ = comm.writeParams(port,1,26,len(patern),patern)
                    if answ!='OK':
                        err+=1
                    patern=self.get_patern(self.val2)
                    patern=comm.recalc_patern(patern)
                    answ = comm.writeParams(port,1,8,len(patern),patern)
                    if answ!='OK':
                        err+=1
                    patern=self.get_patern(self.val3)
                    patern=comm.recalc_patern(patern)
                    answ = comm.writeParams(port,1,62,len(patern),patern)
                    if answ!='OK':
                        err+=1
                    patern=self.get_patern(self.val4)
                    patern=comm.recalc_patern(patern)
                    answ = comm.writeParams(port,1,44,len(patern),patern)
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
        
        print('{} {}'.format(self.fdSpeedVar.get(),self.fdAccelVar.get()))
        print('{} {}'.format(self.lfSpeedVar.get(),self.lfAccelVar.get()))

        for i in range(8):
            for j in range(3):
                print('{}{}'.format(self.val1[i][j].get(),','if j!=2 else ' '),end='')
            for j in range(3):
                print('{}{}'.format(self.val2[i][j].get(),','if j!=2 else ' '),end='')
            for j in range(3):
                print('{}{}'.format(self.val3[i][j].get(),','if j!=2 else ' '),end='')
            for j in range(3):
                print('{}{}'.format(self.val4[i][j].get(),','if j!=2 else '\n'),end='')

    def save_data_to_file(self):

        filename = filedialog.asksaveasfilename(filetypes = (("data files","*.dat"),))

        if filename:
            if filename.split('.')[-1]!='dat':
                filename = '{}.dat'.format(filename)
            f=open(filename,'w')
            f.write('[traverse]\n')
            f.write('fd_speed={}\n'.format(self.fdSpeedVar.get()))
            f.write('fd_accel={}\n'.format(self.fdAccelVar.get()))
            f.write('lf_speed={}\n'.format(self.lfSpeedVar.get()))
            f.write('lf_accel={}\n'.format(self.lfAccelVar.get()))
            f.write('[set1]\n')
            for i in range(8):
                f.write('row{}={};{};{}\n'.format(i,self.val1[i][0].get(),self.val1[i][1].get(),self.val1[i][2].get()))
            f.write('[set2]\n')
            for i in range(8):
                f.write('row{}={};{};{}\n'.format(i,self.val2[i][0].get(),self.val2[i][1].get(),self.val2[i][2].get()))
            f.write('[set3]\n')
            for i in range(8):
                f.write('row{}={};{};{}\n'.format(i,self.val3[i][0].get(),self.val3[i][1].get(),self.val3[i][2].get()))
            f.write('[set4]\n')
            for i in range(8):
                f.write('row{}={};{};{}\n'.format(i,self.val4[i][0].get(),self.val4[i][1].get(),self.val4[i][2].get()))

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
                        if line[:len('fd_speed=')]=='fd_speed=':
                                self.fdSpeedVar.set(line[len('fd_speed='):].strip())
                        elif line[:len('fd_accel=')]=='fd_accel=':
                                self.fdAccelVar.set(line[len('fd_accel='):].strip())
                        elif line[:len('lf_speed=')]=='lf_speed=':
                                self.lfSpeedVar.set(line[len('lf_speed='):].strip())
                        elif line[:len('lf_accel=')]=='lf_accel=':
                                self.lfAccelVar.set(line[len('lf_accel='):].strip())
                    elif group=='set1':
                        for i in range(8):
                            name = 'row{}='.format(i)
                            if line[:len(name)]==name:
                                [a,b,c] = line[len(name):].strip().split(';')
                                self.val1[i][0].set(a)
                                self.val1[i][1].set(b)
                                self.val1[i][2].set(c)
                    elif group=='set2':
                        for i in range(8):
                            name = 'row{}='.format(i)
                            if line[:len(name)]==name:
                                [a,b,c] = line[len(name):].strip().split(';')
                                self.val2[i][0].set(a)
                                self.val2[i][1].set(b)
                                self.val2[i][2].set(c)
                    elif group=='set3':
                        for i in range(8):
                            name = 'row{}='.format(i)
                            if line[:len(name)]==name:
                                [a,b,c] = line[len(name):].strip().split(';')
                                self.val3[i][0].set(a)
                                self.val3[i][1].set(b)
                                self.val3[i][2].set(c)
                    elif group=='set4':
                        for i in range(8):
                            name = 'row{}='.format(i)
                            if line[:len(name)]==name:
                                [a,b,c] = line[len(name):].strip().split(';')
                                self.val4[i][0].set(a)
                                self.val4[i][1].set(b)
                                self.val4[i][2].set(c)

if __name__=="__main__":

    root = Tk()
    app = app(root)
    root.mainloop()
