from tkinter import *

def StringVarGrid(x,y,master):

    varGrid = []
    for j in range(y):
        row = []
        for i in range(x):
            row.append(StringVar(master))
            row[-1].set('0')
        varGrid.append(row)
    return varGrid

    
def CreateSet(frame,values):

    setOut = Frame(frame)
    fsb = []
    val1 = []
    val2 = []
    for i in range(8):
        fsb.append(Spinbox(setOut,from_=0,to=3,width=3,state='readonly',textvariable=values[i][0]))
        fsb[-1].grid(row=i,column=0)
        val1.append(Entry(setOut,width=7,textvariable=values[i][1]))
        val1[-1].grid(row=i,column=1)
        val2.append(Entry(setOut,width=7,textvariable=values[i][2]))
        val2[-1].grid(row=i,column=2)

    return setOut
            

class app:

    def __init__(self,master):

        frame = Frame(master)
        frame.pack()

        panel1 = LabelFrame(frame,text='Start',padx=5,pady=5)
        panel1.pack(side=LEFT,padx=5,pady=5)

        self.set1frm = LabelFrame(panel1,text='Feeding',relief=FLAT)
        self.set1frm.pack(side=LEFT)
        self.val1 = StringVarGrid(3,8,master)
        self.set1 = CreateSet(self.set1frm,self.val1)
        self.set1.pack(padx=5,pady=5)

        self.set2frm = LabelFrame(panel1,text='Lifting',relief=FLAT)
        self.set2frm.pack(side=LEFT)
        self.val2 = StringVarGrid(3,8,master)
        self.set2 = CreateSet(self.set2frm,self.val2)
        self.set2.pack(padx=5,pady=5,side=LEFT)

        panel2 = LabelFrame(frame,text='Stop',padx=5,pady=5)
        panel2.pack(side=LEFT,padx=5,pady=5)

        self.set3frm = LabelFrame(panel2,text='Feeding',relief=FLAT)
        self.set3frm.pack(side=LEFT)
        self.val3 = StringVarGrid(3,8,master)
        self.set3 = CreateSet(self.set3frm,self.val3)
        self.set3.pack(padx=5,pady=5,side=LEFT)

        self.set4frm = LabelFrame(panel2,text='Lifting',relief=FLAT)
        self.set4frm.pack(side=LEFT)
        self.val4 = StringVarGrid(3,8,master)
        self.set4 = CreateSet(self.set4frm,self.val4)
        self.set4.pack(padx=5,pady=5,side=LEFT)

        self.button = Button(frame, text="KONEC", fg="red", command=master.destroy)
        self.button.pack(side=LEFT)

        self.hi_there = Button(frame, text="Pozdrav!", command=self.rekni_ahoj)
        self.hi_there.pack(side=LEFT)

    def rekni_ahoj(self):

        for i in range(8):
            for j in range(3):
                print('{}{}'.format(self.val1[i][j].get(),','if j!=2 else ' '),end='')
            for j in range(3):
                print('{}{}'.format(self.val2[i][j].get(),','if j!=2 else ' '),end='')
            for j in range(3):
                print('{}{}'.format(self.val3[i][j].get(),','if j!=2 else ' '),end='')
            for j in range(3):
                print('{}{}'.format(self.val4[i][j].get(),','if j!=2 else '\n'),end='')

        print('Ahoj všici!')


if __name__=="__main__":

    root = Tk()
    app = app(root)
    root.mainloop()
