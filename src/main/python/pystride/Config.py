import os
import xml.etree.ElementTree as ET

class Config:
    def __init__(self, filename=None, root=None):
        if filename != None:
            if os.path.isfile(filename):
                self._etree = ET.parse(filename).getroot()
            else:
                print("Configuration file " + filename + " could not be found")
        else:
            self._etree = ET.Element(root)

    def getParameter(self, name: str):
        if self._etree != None:
            if self._etree.find(name) != None:
                if self._etree.find(name).text != None:
                    return self._etree.find(name).text

    def setParameter(self, name:str, value):
        if self._etree != None:
            if self._etree.find(name) != None:
                self._etree.find(name).text = str(value)
            else:
                newElems = name.split("/")
                root = self._etree
                for elem in newElems:
                    if root.find(elem) != None:
                        root = elem
                    else:
                        newElem = ET.SubElement(root, elem)
                        root = newElem
                self._etree.find(name).text = str(value)

    def toFile(self, filepath: str):
        ET.ElementTree(self._etree).write(filepath)

    def copy(self):
        newConfig = Config()
        newConfig._etree = ET.fromstring(ET.tostring(self._etree))
        return newConfig
