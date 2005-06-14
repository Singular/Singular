from xml.dom.xmlbuilder import *
#from xml.dom.ext import PrettyPrint
from xml.dom import Node
import xml.dom as dom
#from 
from StringIO import StringIO
import re
import sys
import os.path
from objects import *
from cd import *
def readFile(input_file_name):
    docstream = open(input_file_name)
    docIS=DOMInputSource()
    docIS.byteStream=docstream
    doc= DOMBuilder().parse(docIS)
    docstream.close()
    
    root=doc.lastChild
    return root
    
def  get_text_in_children(n):
    t = ""
    for c in n.childNodes:
        if c.nodeType== Node.TEXT_NODE:
            t += c.nodeValue
    return t
def remove_white_space(node):
    remove_list=[]
    if node.nodeName=="OMC":
        return
    for child in node.childNodes:
        if child.nodeType==dom.Node.TEXT_NODE and not child.data.strip():
            remove_list.append(child)
        elif child.hasChildNodes():
            remove_white_space(child)
    for node in remove_list:
        node.parentNode.removeChild(node)
        node.unlink()
        
class OMFromXMLBuilder:
    def buildFromNode(self, node):
        if (node.nodeName=="OMI"):
            content=get_text_in_children(node)
            return OMint(content)
        if (node.nodeName=="OMV"):
            name=node.getAttribute("name") #node.attributes["name"]
            #print dir(name)
            return OMvar(name)
        if (node.nodeName=="OMS"):
            if node.hasAttribute("cdbase"):
                #FIXME: obtain from ancestors
                cdbase=node.getAttribute("cdbase")
            else: 
                cdbase=None
            cdname=node.getAttribute("cd")
            name=node.getAttribute("name")
            #print repr(node)
            #print node.attributes
            #print "cdbase", cdbase
            if cdbase==None:
                cd=OMcd(cdname)
            else:
                cd=OMcd(cdname,cdbase)
            return OMsymbol(name,cd)
        if (node.nodeName=="OMA"):
            children=[self.buildFromNode(c) for c in node.childNodes]
            return OMapplication(children[0],children[1:])
    def build(self, root):
        remove_white_space(root)
        return self.buildFromNode(root)
#TODO: handle hex floats
#TODO: handle ancestors cdbase
if __name__=='__main__':
    if len(argv<=2):
        print "Usage: python omxmlreader [--evaluate] input output"
    for arg in argv:
        
    from context import Context
    inputf=sys.argv[1]
    root=readFile(inputf)
    builder=OMFromXMLBuilder()
    context=Context()
    
    print context.XMLEncodeObject(builder.build(root))
    #print repr(root.getAttribute("blabla"))