import os
import xml.etree.ElementTree as ET

class Config:
    def __init__(self, filepath=None, root="config"):
        if filepath != None:
            self.fromFile(filepath)
        else:
            self._etree = ET.Element(root)

    def getParameter(self, name: str, default=None):
        if self._etree.find(name) != None:
            if self._etree.find(name).text != None:
                return self._etree.find(name).text
        return default

    def setParameter(self, name: str, value):
        if self._etree.find(name) != None:
            self._etree.find(name).text = str(value)
        else:
            new_elems = name.split(".")
            root = self._etree
            for elem in new_elems:
                if root.find(elem) != None:
                    root = elem
                else:
                    new_elem = ET.SubElement(root, elem)
                    root = new_elem
            self._etree.find(name.replace('.', '/')).text = str(value)

    def fromFile(self, filepath: str):
        if os.path.isfile(filepath):
            self._etree = ET.parse(filepath).getroot()
        else:
            print ("Configuration file " + filepath + " could not be found")
            exit(1)

    def toFile(self, filepath: str):
        ET.ElementTree(self._etree).write(filepath)

    def toString(self):
        return ET.tostring(self._etree, encoding="utf8", method="xml").decode("utf-8")

    def copy(self):
        """
            Make a different Config object, with the same
            configuration parameter values as this one.
        """
        new_config = Config()
        new_config._etree = ET.fromstring(ET.tostring(self._etree))
        return new_config
