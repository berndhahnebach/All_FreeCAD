# FreeCAD module provding base classes for document objects and view provider  
# (c) 2011 Werner Mayer LGPL

class DocumentObject(object):
    def __init__(self):
        self.__object__=None
    def execute(self):
        raise Exception("Not yet implemented")
    #def __getattr__(self, attr):
    #    if hasattr(self.__object__,attr):
    #        return getattr(self.__object__,attr)
    #    else:
    #        return object.__getattribute__(self,attr)
    def addProperty(self,type,name='',group='',doc='',attr=0,readonly=False,hidden=False):
        self.__object__.addProperty(type,name,group,doc,attr,readonly,hidden)
    def supportedProperties(self):
        return self.__object__.supportedProperties()
    def isDerivedFrom(self, obj):
        return self.__object__.isDerivedFrom(obj)
    def getAllDerivedFrom(self):
        return self.__object__.getAllDerivedFrom()
    def getProperty(self,attr):
        return self.__object__.getPropertyByName(attr)
    def getTypeOfProperty(self,attr):
        return self.__object__.getTypeOfProperty(attr)
    def getGroupOfProperty(self,attr):
        return self.__object__.getGroupOfProperty(attr)
    def getDocumentationOfProperty(self,attr):
        return self.__object__.getDocumentationOfProperty(attr)
    def touch(self):
        return self.__object__.touch()
    def purgeTouched(self):
        return self.__object__.purgeTouched()
    @property
    def PropertiesList(self):
        return self.__object__.PropertiesList
    @property
    def Type(self):
        return self.__object__.Type
    @property
    def Module(self):
        return self.__object__.Module
    @property
    def Content(self):
        return self.__object__.Content
    @property
    def MemSize(self):
        return self.__object__.MemSize
    @property
    def Name(self):
        return self.__object__.Name
    @property
    def Document(self):
        return self.__object__.Document
    @property
    def State(self):
        return self.__object__.State
    @property
    def ViewObject(self):
        return self.__object__.ViewObject
    @ViewObject.setter
    def ViewObject(self,value):
        self.__object__.ViewObject=value
    @property
    def InList(self):
        return self.__object__.InList
    @property
    def OutList(self):
        return self.__object__.OutList

def testMethod():
    class MyFeature(DocumentObject):
        def execute(self):
            print "Execute my feature"
        def onChanged(self,prop):
            print "Property %s has changed" % (prop)
    import FreeCAD
    doc=FreeCAD.newDocument()
    obj=MyFeature()
    doc.addObject("Mesh::FeaturePython","MyName",obj,None)
    obj.addProperty("App::PropertyLinkList","Layers","Base", "Layers")
