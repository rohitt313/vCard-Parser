import sys
import sqlite3
from ctypes import *
import os
import asciimatics.widgets as widgets
import asciimatics.scene as scene
import asciimatics.screen as screen
import asciimatics.exceptions as exceptions
import mysql.connector
from mysql.connector import Error
from datetime import datetime
from asciimatics.event import KeyboardEvent
from pathlib import Path

testLibPath='./libvcparser.so'

# here we are referncing all the C function we use
try:
    testlib=CDLL(testLibPath)
    testlib.dispValid.restype=POINTER(c_char_p)
    testlib.freeDispValid.argtypes=[POINTER(c_char_p)]
    testlib.validCards.restype=POINTER(c_void_p)
    testlib.freeValidCards.argtypes=[POINTER(c_void_p)]
    testlib.Contact.restype=c_char_p
    testlib.Contact.argtypes=[c_void_p]
    testlib.otherProps.restype=c_char
    testlib.otherProps.argtypes=[c_void_p]
    testlib.dispBday.restype=POINTER(c_char_p)
    testlib.dispAnni.restype=POINTER(c_char_p)
    testlib.fileNameChange.argtypes=[c_char_p,c_char_p]  
    testlib.fileNameChange.restype=c_int
    testlib.contactChange.argtypes=[POINTER(c_void_p),c_char_p,c_char_p]
    testlib.contactChange.restype=c_int
    testlib.sqlDispBday.restype=POINTER(c_char_p)
    testlib.sqlDispAnni.restype=POINTER(c_char_p)
    testlib.createCardSql.argtypes=[c_char_p,c_char_p]
    testlib.createCardSql.restype=c_int
        
except Exception as error:
    print("some error loading shared libray:",error)
    sys.exit(1)


class VCardModel():
    def __init__(self):
        self._db=sqlite3.connect(':memory:')
        self._db.row_factory=sqlite3.Row

        self._db.cursor().execute('''
            CREATE TABLE vCards(
                id INTEGER PRIMARY KEY,
                "File name" TEXT,
                Contact TEXT,
                Birthday TEXT,
                Anniversary TEXT,
                "Other properties" TEXT)
        ''')
        self._db.commit()
        self.presId=None
        self.connectStat=False
        self.conn=None
        #print("loading")
        self.dispValid=self.validList()
        
        #print()
        
        #print()
        self.fillDb()
        count=self._db.cursor().execute("SELECT COUNT(*) FROM vCards").fetchone()[0]
        #print(")
     
    #this connects to the sql DB
    def connect(self,uName,dbN,pWord):
        try:
            self.conn=mysql.connector.connect(
                host="dursley.socs.uoguelph.ca",
                user=uName,
                password=pWord,
                database=dbN
            )
            
            if self.conn.is_connected()==True:  
                cursor=self.conn.cursor()
                q1='''
                    CREATE TABLE IF NOT EXISTS FILE (
                        file_id INT AUTO_INCREMENT PRIMARY KEY,
                        file_name VARCHAR(60) NOT NULL,
                        last_modified DATETIME,
                        creation_time DATETIME NOT NULL
                    )
                '''
                
                cursor.execute(q1)

                q2='''
                    CREATE TABLE IF NOT EXISTS CONTACT (
                        contact_id INT AUTO_INCREMENT PRIMARY KEY,
                        name VARCHAR(256) NOT NULL,
                        birthday DATETIME,
                        anniversary DATETIME,
                        file_id INT NOT NULL,
                        FOREIGN KEY (file_id) REFERENCES FILE(file_id) ON DELETE CASCADE
                    )
                '''
                
                cursor.execute(q2)
                
                self.conn.commit()
                self.connectStat=True
                return True
            
        except Error as e:
            return False
            
        return False
    
    #this returns a list of valid card files
    def validList(self):
        index=0
        dispValidP=[]
        try:
            dispValidC=testlib.dispValid()
            if not dispValidC:
                print("Something is wrong")
                return dispValidP
                
            while dispValidC[index]:
                tempStr=dispValidC[index].decode('utf-8')
                dispValidP.append(tempStr)
                index=index+1
                
            testlib.freeDispValid(dispValidC)
        except Exception as error:
            print("Error in validList:",error)
        
        return dispValidP

    #this fills the local DB
    def fillDb(self):
        try:
            validCa=testlib.validCards()
            if not validCa:
                print("validCards returned NULL value")
                return
                
            fileN=testlib.dispValid()
            if not fileN:
                print("dispValid returned NULL value")
                return

            bDayptr=testlib.dispBday()
            anniPtr=testlib.dispAnni()
                
            index=0
            while fileN[index]:
                try:
                    fileName=fileN[index].decode('utf-8')                    
                    contactName=testlib.Contact(validCa[index])

                    if contactName:
                        contactName=contactName.decode('utf-8')
                    else:
                        contactName="Something went wrong"
                    
                    try:
                        optPropC=testlib.otherProps(validCa[index])
                        optPropC=str(ord(testlib.otherProps(validCa[index])))
                    except Exception as error:
                        print("error in optProps: "+str(error))
                        optPropC="0"
                    
                    bDay=""
                    anni=""
                    if bDayptr and bDayptr[index]:
                        try:
                            bDay=bDayptr[index].decode('utf-8')
                        except Exception as error:
                            print("erro in bday: "+str(error))
                
                    if anniPtr and anniPtr[index]:
                        try:
                            anni=anniPtr[index].decode('utf-8')
                        except Exception as error:
                            print("error in anni: "+str(error))
                    
                    self.create({
                        "File name": fileName,
                        "Contact": contactName,
                        "Birthday": bDay,
                        "Anniversary": anni,
                        "Other properties": optPropC
                    })
                    
                    #print()
                    
                except Exception as card_error:
                    print("er at index "+str(index)+": "+str(card_error))
                
                index=index+1
            
            try:
                testlib.freeDispValid(fileN)
                testlib.freeValidCards(validCa)
                testlib.freeDispValid(bDayptr)
                testlib.freeDispValid(anniPtr)
            except Exception as free_error:
                print("Error in free memory: " + str(free_error))
            
        except Exception as error:
            print("can't load caards to databse: "+str(error))

    #this updates a vcard in DB
    def updateF(self,fName,vId):
        self._db.cursor().execute('''UPDATE vCards SET "File name"=? WHERE id=?''',(fName,vId))
        self._db.commit()
            
    #this inserts a vcard into DB
    def create(self,vcard):
        q='''
            INSERT INTO vCards("File name", Contact, Birthday, Anniversary, "Other properties")
            VALUES(:file_name, :Contact, :Birthday, :Anniversary, :other_properties)'''

        try:
            self._db.cursor().execute(q,
                                  {"file_name": vcard["File name"], 
                                   "Contact": vcard["Contact"],
                                   "Birthday": vcard["Birthday"],
                                   "Anniversary": vcard["Anniversary"],
                                   "other_properties": vcard["Other properties"]})
            self._db.commit()
        except Exception as error:
            print("insert error: "+str(error))

    # this gets info from all vcards from DB
    def get_summary(self):
        try:
            q='SELECT "File name", id from vCards'
            res=self._db.cursor().execute(q).fetchall()
            #print()
            return res
        except Exception as error:
            print("Error in get_summary: "+str(error))
            return []

    
    #this returns a card record from DB
    def oneCard(self,vId):
        return self._db.cursor().execute("SELECT * from vCards WHERE id=:id",{"id": vId}).fetchone()

    #this returns a vcard's data
    def CurrCard(self):
        if self.presId is not None:
            return self.oneCard(self.presId)
        else:
            return {"File name": "","Contact": "","Birthday": "","Anniversary": "","Other properties": 0,"id": None}

    #this updates a specific vcard
    def updateCurrCard(self, details):
        if self.presId is None:
            self.create(details)
        else:
            details["id"] = self.presId
            q='''
                UPDATE vCards SET "File name"=?, Contact=?, Birthday=?,
                Anniversary=?, "Other properties"=? WHERE id=?'''

            self._db.cursor().execute(q,
                                  (details["File name"], details["Contact"], 
                                   details["Birthday"], details["Anniversary"], 
                                   details["Other properties"], details["id"]))
            self._db.commit()

    #this puts data in the sql DB from cards folder
    def popSql(self):
        if not self.connectStat or not self.conn:
            print("No connection")
            return False
            
        try:
            cursor=self.conn.cursor()
            q1="SELECT * FROM vCards"
            
            liteRec=self._db.cursor().execute(q1).fetchall()
            #print()

            currTime=datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            
            sqlBday=testlib.sqlDispBday()
            sqlAnni=testlib.sqlDispAnni()

            j=0
            for record in liteRec:
                fName=record["File name"]
                cName=record["Contact"]

                q2='SELECT file_id FROM FILE WHERE file_name = %s'
                cursor.execute(q2,(fName,))
                exiF=cursor.fetchone()

                if exiF:
                    #print()
                    continue
                    
                last_modified=currTime
                creation_time=currTime
                    
                q3='''
                    INSERT INTO FILE (file_name, last_modified, creation_time)
                    VALUES (%s, %s, %s)
                '''
                cursor.execute(q3,(fName,last_modified,creation_time))
                
                file_id=cursor.lastrowid
                
                bdayField=None
                anniField=None

                if sqlBday and sqlBday[j]:
                    bdayC=sqlBday[j].decode('utf-8')
                    if bdayC and bdayC.strip()!="":
                        bdayField=bdayC
                
                if sqlAnni and sqlAnni[j]:
                    anniC=sqlAnni[j].decode('utf-8')
                    if anniC and anniC.strip()!="":
                        anniField=anniC
                
                q4='''
                    INSERT INTO CONTACT (name, birthday, anniversary, file_id)
                    VALUES (%s, %s, %s, %s)
                '''
                cursor.execute(q4,(cName, bdayField, anniField, file_id))
                #print()
                j=j+1  
                
            if sqlBday:
                testlib.freeDispValid(sqlBday)
            if sqlAnni:
                testlib.freeDispValid(sqlAnni)
                
            self.conn.commit()
            #print()
            return True
            
        except Error as error:
            print("error putting data  in MySQL: "+str(error))
            if self.conn:
                self.conn.rollback()
            return False
        

    #this updates the DB if any changes are made to any file on disk
    def updateSql(self,fName,new=False,oldFname=None):
        if not self.connectStat or not self.conn:
            print("No database connection")
            return False
            
        try:
            cursor=self.conn.cursor()
            currTime=datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            
            last_modified=currTime
            
            sqlBday=testlib.sqlDispBday()
            sqlAnni=testlib.sqlDispAnni()
            
            if oldFname and oldFname != fName:
                #prit())
                q1='''
                    UPDATE FILE SET file_name = %s, last_modified = %s
                    WHERE file_name = %s
                '''
                cursor.execute(q1,(fName,last_modified,oldFname))
                
                if cursor.rowcount>0:
                    #print()
                    self.conn.commit()
                    if sqlBday:
                        testlib.freeDispValid(sqlBday)
                    if sqlAnni:
                        testlib.freeDispValid(sqlAnni)
                    return True
                else:
                    randomnumber=55
                    #print()
            
            q2="SELECT id, \"File name\" FROM vCards"
            liteRec=self._db.cursor().execute(q2).fetchall()
            fInd=-1
            j=0
            for rec in liteRec:
                if rec["File name"]==fName:
                    fInd=j
                    break
                j=j+1  
                    
            if fInd==-1:
                print("Cant find index" +str(fName)+ "in sql DB")
                if sqlBday:
                    testlib.freeDispValid(sqlBday)
                if sqlAnni:
                    testlib.freeDispValid(sqlAnni)
                return False
            
            bDayField=None
            anniField=None
            if sqlBday and sqlBday[fInd]:
                bDayC=sqlBday[fInd].decode('utf-8')
                if bDayC and bDayC.strip() != "":
                    bDayField=bDayC
            
            if sqlAnni and sqlAnni[fInd]:
                anniC=sqlAnni[fInd].decode('utf-8')
                if anniC and anniC.strip() != "":
                    anniField=anniC
            
            if new:
                q3='''
                    INSERT INTO FILE (file_name, last_modified, creation_time)
                    VALUES (%s, %s, %s)
                '''
                cursor.execute(q3,(fName,currTime,currTime))
                
                file_id=cursor.lastrowid
                
                q4="SELECT Contact FROM vCards WHERE \"File name\"=?"
                record=self._db.cursor().execute(q4,(fName,)).fetchone()
                    
                if record:
                    cName=record["Contact"]
                    
                    q5='''
                        INSERT INTO CONTACT (name, birthday, anniversary, file_id)
                        VALUES (%s, %s, %s, %s)
                    '''
                    cursor.execute(q5,(cName,bDayField,anniField,file_id))
                    
                    #print(")
            else:
                q6="SELECT file_id FROM FILE WHERE file_name = %s"
                cursor.execute(q6,(fName,))
                result=cursor.fetchone()
                
                if result:
                    file_id=result[0]
                    q7='''
                        UPDATE FILE SET last_modified = %s
                        WHERE file_id = %s
                    '''
                    cursor.execute(q7,(currTime,file_id))
                    
                    q8="SELECT Contact FROM vCards WHERE \"File name\"=?"
                    record=self._db.cursor().execute(q8,(fName,)).fetchone()
                        
                    if record:
                        cName=record["Contact"]
                        q9='''
                            UPDATE CONTACT SET name = %s, birthday = %s, anniversary = %s
                            WHERE file_id = %s
                        '''
                        cursor.execute(q9,(cName,bDayField,anniField,file_id))
                        
                        #print()

                else:
                    if sqlBday:
                        testlib.freeDispValid(sqlBday)
                    if sqlAnni:
                        testlib.freeDispValid(sqlAnni)
                    return self.updateSql(fName,new=True)     
            
            if sqlBday:
                testlib.freeDispValid(sqlBday)
            if sqlAnni:
                testlib.freeDispValid(sqlAnni)
                
            self.conn.commit()
            return True
            
        except Exception as error:
            print("error updating mysql DB: "+str(error))

            try:
                if sqlBday:
                    testlib.freeDispValid(sqlBday)
                if sqlAnni:
                    testlib.freeDispValid(sqlAnni)
            except:
                pass
                
            if self.conn:
                self.conn.rollback()
            return False

    def sync(self):                        #This deletes files from DB if removed from card folder
        if self.connectStat==False or not self.conn:
            print("No DB connection")
            return False
        
        try:
            deleted_files = []
            q1="SELECT \"File name\" FROM vCards"
            liteRec=self._db.cursor().execute(q1).fetchall()
            
            exeFi=set()
            for record in liteRec:
                exeFi.add(record["File name"])

            cursor=self.conn.cursor()
            q2="SELECT file_id, file_name FROM FILE"
            cursor.execute(q2)
            sqlFi=cursor.fetchall()

            for fId,fName in sqlFi:
                if fName not in exeFi:
                    deleted_files.append((fId, fName))

            for fId,fName in deleted_files:
                q3="DELETE FROM FILE WHERE file_id = %s"
                cursor.execute(q3,(fId,))

            self.conn.commit()
            return True
        
        except Error as error:
            print("Error synchronizing databases: "+str(error))
            if self.conn:
                self.conn.rollback()
            return False

class LoginView(widgets.Frame):
    def __init__(self, screen, model):
        super(LoginView, self).__init__(screen,
                                       screen.height,
                                       screen.width,
                                       hover_focus=True,
                                       can_scroll=False,
                                       title="Database Login",
                                       reduce_cpu=True)
        self._model=model
        
        layout=widgets.Layout([100])
        self.add_layout(layout)
        layout.add_widget(widgets.Text("Username:","username"))
        layout.add_widget(widgets.Text("Password:","password",hide_char=" "))
        layout.add_widget(widgets.Text("DB name:","db_name"))
        
        self._status_widget=widgets.Label("")
        layout.add_widget(self._status_widget)
        
        layout2=widgets.Layout([1, 1])
        self.add_layout(layout2)
        layout2.add_widget(widgets.Button("OK",self._ok),0)
        layout2.add_widget(widgets.Button("Cancel",self._cancel),1)
        
        self.fix()
        
    def reset(self):
        super(LoginView,self).reset()
        self._status_widget.text=""

    #connects to DB based on info in login view
    def _ok(self):
        self.save()
        
        uName=self.data["username"]
        pWord=self.data["password"]
        dbN=self.data["db_name"]
        
        if self._model.connect(uName,dbN,pWord)==True:
            self._model.popSql()
            self._status_widget.text="You successfully connected to the database."
            raise exceptions.NextScene("Main")
        else:
            self._status_widget.text="Please check your login info."
    
    def _cancel(self):
        self._model.connectStat=False
        raise exceptions.NextScene("Main")


class mainView(widgets.Frame):
    def __init__(self, screen, model):
        super(mainView, self).__init__(screen,
                                       screen.height,
                                       screen.width,
                                       on_load=self._reload_list,
                                       hover_focus=True,
                                       can_scroll=False,
                                       title="vCard List")
        self._model=model
        height=self._canvas.height-6  
        
        entr=model.get_summary()
        
        #print()
        
        self.lVie=widgets.ListBox(
            height,
            entr,
            name="vcards",
            add_scroll_bar=True,
            on_change=self._on_pick,
            on_select=self.editB)
            
        self.ediBut=widgets.Button("Edit",self.editB)
        self.dbBut=widgets.Button("DB Queries",self.queriesB)
        self.creBut=widgets.Button("Create",self.createB)
        self.exiBut=widgets.Button("Exit",self._exit)
        
        layout=widgets.Layout([100], fill_frame=True)
        self.add_layout(layout)
        layout.add_widget(self.lVie)
        layout.add_widget(widgets.Divider())
        
        self.stat=widgets.Label("")
        layout.add_widget(self.stat)
        
        layout2=widgets.Layout([1,1,1,1])
        self.add_layout(layout2)
        layout2.add_widget(self.creBut,0)
        layout2.add_widget(self.ediBut,1)
        layout2.add_widget(self.dbBut,2)
        layout2.add_widget(self.exiBut,3)
        self.fix()
        self._on_pick()

    #enables or disables the buttons and shows connection stat
    def _on_pick(self):
        direc=Path("cards")
        if self._model.connectStat==True:
            self.creBut.disabled=False
            if self.lVie.value==None:
                self.ediBut.disabled=True
                self.dbBut.disabled=True
            else:
                self.ediBut.disabled=False
                self.dbBut.disabled=False

            if not any(direc.iterdir()):
                self.stat.text="Database is connected but we have no valid vCard files."
            else:
                self.stat.text="Database is connected."
            
        else:
            self.creBut.disabled=True
            self.ediBut.disabled=True
            self.dbBut.disabled=True
            if not any(direc.iterdir()):
                self.stat.text="No Database connection but we have no valid vCard files."
            else:
                self.stat.text="No Database connection"

    

    def _reload_list(self, new_value=None):
        if self._model.connectStat==True:     # we sync the databases just in case
            self._model.sync()
            # self.
        entries=self._model.get_summary()
        self.lVie.options=entries
        self.lVie.value=new_value
        self._on_pick() 

    def createB(self):                         #for disabling the create button
        if self._model.connectStat==False:
            return
            
        self._model.presId=None
        raise exceptions.NextScene("Edit")

    def editB(self):                          #for disabling the edit button
        if self._model.connectStat==False:
            return
            
        self.save()
        self._model.presId = self.data["vcards"]
        raise exceptions.NextScene("Edit")

    def queriesB(self):                       #for disabling the DB queries button
        if self._model.connectStat==False:
            return
        raise exceptions.NextScene("DB")

    @staticmethod
    def _exit():
        raise exceptions.StopApplication("We have exited.")


class VCardView(widgets.Frame):
    def __init__(self, screen, model):
        super(VCardView, self).__init__(screen,
                                          screen.height,
                                          screen.width,
                                          hover_focus=True,
                                          can_scroll=False,
                                          title="VCard Details",
                                          reduce_cpu=True)
        self._model=model
        self.ogFname=""
        self.stat=""

        layout=widgets.Layout([100], fill_frame=True)
        self.add_layout(layout)

        self.fNameFie=widgets.Text("File Name:", "File name")
        layout.add_widget(self.fNameFie)

        layout.add_widget(widgets.Text("Contact:", "Contact"))
        
        self._fNameStatwidg=widgets.Label("Status: ")
        layout.add_widget(self._fNameStatwidg)
        
        bdayFie=widgets.Text("Birthday:", "Birthday")
        bdayFie.disabled=True
        layout.add_widget(bdayFie)
        
        anniFie=widgets.Text("Anniversary:", "Anniversary")
        anniFie.disabled=True
        layout.add_widget(anniFie)
        
        optPropsFie=widgets.Text("Optional Properties:", "Other properties")
        optPropsFie.disabled=True
        layout.add_widget(optPropsFie)
        
        layout2=widgets.Layout([1,1,1,1])
        self.add_layout(layout2)
        layout2.add_widget(widgets.Button("OK", self._ok), 0)
        layout2.add_widget(widgets.Button("Cancel", self._cancel), 3)
        self.fix()

    # this validates the contact when creating or editing a vcard
    def validC(self, cName):
        if self._model.presId is None:
            if not cName or cName.strip()=="":
                self.stat="Contact field is empty"
                self._fNameStatwidg.text="Status: " + self.stat
                return False
            return True
            
        try:
            index=0
            currFname=self.ogFname
            fInd=-1
            newDispVal=testlib.dispValid()
            while newDispVal[index]:
                tempStr=newDispVal[index].decode('utf-8')
                if tempStr==currFname:
                    fInd=index
                    break

                index=index+1
                    
            if fInd==-1:
                print("Cannot find index for file "+str(currFname)+" in valid cards list")
                self.stat="file not found"
                self._fNameStatwidg.text ="Status: "+self.stat
                return False
                
            validVlist= testlib.validCards()
            if not validVlist:
                print("validCards returned NULL value")
                self.stat="can't validate contact: error getting card data"
                self._fNameStatwidg.text="Status: "+self.stat
                return False
                
            cNameInC=cName.encode('utf-8')
            fNameInC=currFname.encode('utf-8')  
            cardPtr=validVlist[fInd]
            if isinstance(cardPtr, int):
                cardObj=c_void_p(cardPtr)
                result=testlib.contactChange(byref(cardObj),cNameInC,fNameInC)
            else:
                result=testlib.contactChange(byref(cardPtr),cNameInC,fNameInC)
            
            testlib.freeValidCards(validVlist)
            
            if result==1:
                self.stat="Contact field is valid"
                self._fNameStatwidg.text ="Status: " + self.stat
                return True
            else:
                self.stat="Contact field is invalid"
                self._fNameStatwidg.text ="Status: " + self.stat
                return False
                
        except Exception as error:
            print("Error validating contact: "+str(error))
            self.stat = "Error validating contact"
            self._fNameStatwidg.text ="Status: " + self.stat
            return False

    #to validate file name when creating vcard
    def validF(self,newFname):
        if self._model.presId is None:
            return True
            
        if newFname==self.ogFname:
            print("Filename is still same: "+str(self.ogFname))
            self.stat="valid"
            self._fNameStatwidg.text ="Status: "+self.stat
            return True
            
        try:
            oldF = self.ogFname.encode('utf-8')
            newF= newFname.encode('utf-8')
            result = testlib.fileNameChange(oldF,newF)
            
            if result==1:
                self.stat="File name field is valid"
                self._fNameStatwidg.text="Status: " + self.stat
                self._model.updateF(newFname,self._model.presId)
                self.ogFname=newFname
                return True
            else:
                self.stat="File name field is invalid"
                self._fNameStatwidg.text="Status: "+self.stat
                return False
        except Exception as error:
            print("Error validating filename: "+str(error))
            self.stat="invalid"
            self._fNameStatwidg.text ="Status: "+self.stat
            return False

    # this gives the correct vcard data
    def reset(self):
        super(VCardView, self).reset()
        self.data =self._model.CurrCard()

        if self._model.presId is not None:
            self.ogFname = self.data["File name"]
            self.fNameFie.disabled=True          # We Disable filename when editing
            self.stat=""
            self._fNameStatwidg.text="Status: "
        else:
            
            self.fNameFie.disabled=False      

        self.stat=""
        self._fNameStatwidg.text="Status: "


    #when we press the ok button, a new record is created or exisiting is updated
    def _ok(self):
        self.save()

        cName=self.data["Contact"]
        if not self.validC(cName):
            return
        
        newFname=self.data["File name"]
        new=self._model.presId is None
        
        if not new:
            oldFileName=self.ogFname
            rename=oldFileName!=newFname
            
            if rename:
                if not self.validF(newFname):
                    return
            
            self._model.updateCurrCard(self.data)
            if self._model.connectStat:
                if rename:
                    self._model.updateSql(newFname,new=False, oldFname=oldFileName)
                else:
                    self._model.updateSql(newFname,new=False)
        else:
            try:
                fNameInC = newFname.encode('utf-8')
                conNameInC = cName.encode('utf-8')
                result = testlib.createCardSql(fNameInC,conNameInC)

                if result==-1:
                    self.stat = "Invalid filename extension"
                    self._fNameStatwidg.text ="Status: "+self.stat
                    return

                elif result==-2:
                    self.stat = "A file with this name already exists"
                    self._fNameStatwidg.text ="Status: "+self.stat
                    return

                elif result==0:
                    self.stat = "Contact field is empty"
                    self._fNameStatwidg.text ="Status: "+self.stat
                    return
                elif result==1:
                    self.stat = "Valid"
                    self._fNameStatwidg.text ="Status: "+self.stat
                else:
                    self.stat="Unknown error"
                    self._fNameStatwidg.text ="Status: "+self.stat
                    return
            except Exception as error:
                print("Error calling createCardSql: "+str(error))
                self.stat = "Error validating card creation"
                self._fNameStatwidg.text ="Status: "+self.stat
                return

            # after creating card we add it to DB
            self._model.updateCurrCard(self.data)
            if self._model.connectStat:
                self._model.updateSql(newFname,new=True)
        
        raise exceptions.NextScene("Main")

    @staticmethod
    def _cancel():
        raise exceptions.NextScene("Main")

class DbView(widgets.Frame):
    def __init__(self,screen,model):
        super(DbView,self).__init__(screen,
                                           screen.height,
                                           screen.width,
                                           hover_focus=True,
                                           can_scroll=True,
                                           title="DB Queries",
                                           reduce_cpu=True)
        self._model=model
        self.lay()
        self.widg()
        self.fix()

    # this creates the layout
    def lay(self):
        self.statLay=widgets.Layout([100])
        self.add_layout(self.statLay)
        self.mainLay=widgets.Layout([100],fill_frame=True)
        self.add_layout(self.mainLay)
        self.buttLay=widgets.Layout([1,1,1])  
        self.add_layout(self.buttLay)

    #this creates the buttons for the DB view
    def widg(self):
        self.statText=widgets.Text()
        self.statText.disabled=True
        self.statText.custom_colour="label"
        self.statLay.add_widget(self.statText)
        
        self._list_box=widgets.ListBox(
            widgets.Widget.FILL_FRAME,[],name="results",add_scroll_bar=True,on_select=self.select)
        self.mainLay.add_widget(self._list_box)
        
        self.buttLay.add_widget(widgets.Button("Display all contacts", self.displayCo),0)
        self.buttLay.add_widget(widgets.Button("Find contacts born in June", self.juneBday),1)
        self.buttLay.add_widget(widgets.Button("Cancel", self._cancel),2)

    def select(self):              #this doesnt do anything
        pass  
        
    def cou(self):                 #we count files and print if we are connected to DB or not
        try:
            if self._model.connectStat==False:
                self.statText.value="Database not connected."
                return
                
            cursor=self._model.conn.cursor()
            q1="SELECT COUNT(*) FROM FILE"
            q2="SELECT COUNT(*) FROM CONTACT"
            cursor.execute(q1)
            fCou=cursor.fetchone()[0]
            
            cursor.execute(q2)
            cCou=cursor.fetchone()[0]
            
            self.statText.value ="Database has "+str(fCou)+" files and "+str(cCou)+" contacts"
 
        except Error as error:
            self.statText.value="some error getting DB status: "+str(error)

    def cle(self):
        self._list_box.options.clear()

        
    def formatR(self,wid,colNo):                   #when we print data in a table form
        row=""
        j=0
        for column in colNo:
            tempStr=str(column)
            if len(tempStr)>wid[j]-1:
                tempStr=tempStr[:wid[j]-4] + "..."
            row=row+tempStr.ljust(wid[j])
            j=j+1  
            
        return row

    def displayCo(self):                          #to display all the contacts in table 
        try:
            if self._model.connectStat==False:
                self._list_box.options=[("No connection.", 0)]
                self.cou()
                return

            q1='''
                SELECT c.contact_id,c.name, c.birthday, c.anniversary, f.file_name
                FROM CONTACT c
                JOIN FILE f ON c.file_id = f.file_id
                ORDER BY c.name ASC, f.file_name ASC
            '''
            cursor=self._model.conn.cursor()
            cursor.execute(q1)
            results=cursor.fetchall()

            if not results:
                self._list_box.options=[("No contacts are present.", 0)]
                self.cou()
                return

            colName=["Contact_ID","Name", "Birthday", "Anniversary", "File"]
            colWidth=[15,25,25,25,30] 
            output=[]
            
            header=self.formatR(colWidth,colName)            
            sep="*" *sum(colWidth)
            
            output.append((header,-1))  
            output.append((sep,-2)) 

            j=0  
            for row in results:
                if row[0]:
                    coId=row[0]
                else:
                    coId=""

                if row[1]:
                    name=row[1]
                else:
                    name=""

                if row[2]:
                    bday=str(row[2])
                else:
                    bday="NULL"

                if row[3]:
                    anni=str(row[3])
                else:
                    anni="NULL"

                if row[4]:
                    fName=row[4]
                else:
                    fName=""
                
                singRow=self.formatR(colWidth,[coId,name, bday, anni, fName])
                output.append((singRow,j))
                j=j+1  
          
            self._list_box.options=output            
            self.cou()
            
        except Error as error:
            self._list_box.options = [(f"error in getting contacts {error}", 0)]
            self.statText.value="Error: "+str(error)

    #This will find all the records whose bday is in june
    def juneBday(self):
        import math
        try:
            if self._model.connectStat==False:
                self._list_box.options = [("No connection.", 0)]
                self.cou()
                return

            cursor=self._model.conn.cursor()
            q1='''
                SELECT 
                    c.name, 
                    c.birthday, 
                    f.last_modified,
                    DATEDIFF(f.last_modified, c.birthday) / 365.0 AS age
                FROM CONTACT c
                JOIN FILE f ON c.file_id = f.file_id
                WHERE MONTH(c.birthday) = 6
                ORDER BY c.birthday ASC
            '''
            cursor.execute(q1)
            results=cursor.fetchall()

            if not results:
                self._list_box.options=[("no contacts born in June are present.", 0)]
                self.cou()
                return

            colName=["Name", "Birthday", "Age"]
            colWidth=[30, 20, 10]  
            output = []
            
            header=self.formatR(colWidth,colName)            
            sep="*" * sum(colWidth)
        
            output.append((header,-1))  
            output.append((sep,-2))
            
            ageEnt = []

            j=0 
            for row in results:
                name=row[0]
                birthday=row[1]
                last_modified=row[2]
                age=row[3]  

                try:
                    bDatDate= datetime.strptime(str(birthday),'%Y-%m-%d %H:%M:%S')
                    bDatDisp=bDatDate.strftime('%Y-%m-%d')                    
                    ageEnt.append({'name':name,'birthday':bDatDisp,'age':float(age),'index':j})
                    
                except ValueError as e:
                    ageEnt.append({'name':name,'birthday':str(birthday),'age':float('nan'),'index':j})

                j=j+1   


            #This is our own sort function for ages
            def sortAges(ent):
                if not math.isnan(ent['age']):
                    return (0,ent['age'])
                else:
                    return (1,float('inf'))

            sortedOnes=sorted(ageEnt,key=sortAges)

            for entry in sortedOnes:
                if not math.isnan(entry['age']):
                    actualAge=f"{entry['age']:.2f}"
                else:
                    actualAge="Error"
                
                singRow=self.formatR(colWidth,[entry['name'],entry['birthday'],actualAge])
                output.append((singRow,entry['index']))

            self._list_box.options=output            
            self.cou()
            
        except Error as error:
            self._list_box.options = [(f"Error fetching June birthdays: {error}", 0)]
            self.statText.value="Error: "+str(error)

    
    #this clears the DB views space above buttons
    def reset(self):
        super(DbView,self).reset()
        self.cle()
        self.cou()
        

    @staticmethod
    def _cancel():
        raise exceptions.NextScene("Main")

#this will switch between different views based on the button user clicks
def demo(screen_obj,scene_obj):
    scenes = [
        scene.Scene([LoginView(screen_obj, vcards)],-1,name="Login"),
        scene.Scene([mainView(screen_obj, vcards)],-1,name="Main"),
        scene.Scene([VCardView(screen_obj, vcards)],-1,name="Edit"),
        scene.Scene([DbView(screen_obj, vcards)],-1,name="DB")
    ]

    screen_obj.play(scenes, stop_on_resize=True, start_scene=scene_obj, allow_int=True)

#print("testing card")
vcards=VCardModel()
last_scene=None

#print("testing")
while True:
    try:
        screen.Screen.wrapper(demo,catch_interrupt=True,arguments=[last_scene])
        sys.exit(0)
    except exceptions.ResizeScreenError as error:
        last_scene=error.scene
    except Exception as err:
        print("something went wrong error: "+str(err))
        sys.exit(1)