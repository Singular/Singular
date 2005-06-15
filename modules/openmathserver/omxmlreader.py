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
from binding import *
from cd import *
from exceptions import *
from omexceptions import *
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
        erg=None
        if (node.nodeName=="OMI"):
            content=get_text_in_children(node)
            erg=OMint(content)
        if (node.nodeName=="OMV"):
            name=node.getAttribute("name") #node.attributes["name"]
            #print dir(name)
            erg= OMvar(name)
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
            erg=OMsymbol(name,cd)
        if (node.nodeName=="OMA"):
            children=[self.buildFromNode(c) for c in node.childNodes]
            erg= OMapplication(children[0],children[1:])
        if (node.nodeName=="OMBIND"):
            children=[self.buildFromNode(c) for c in node.childNodes]
            erg= OMbinding(children[0],children[1:-1],children[-1])
        if (node.nodeName=="OMF"):
            if (node.hasAttribute("dec")):
                value=float(node.getAttribute("dec"))
            else:
                raise NotImplementedError
            erg=OMfloat(value)
        if (node.nodeName=="OMR"):
            if node.hasAttribute("xref"):
                erg=OMref(node.getAttribute("xref"))
                self.refs.append(erg)
            else:
                raise UnresolvedReference
        if None==erg:
            raise NotImplementedError
        else:
            if node.hasAttribute("id"):
                id=node.getAttribute("id")
                self.ids[id]=erg
            return erg
    def __init__(self):
        self.refs=[]
        self.ids={}
    def build(self, root):
        remove_white_space(root)
        erg=self.buildFromNode(root)
        for r in self.refs:
            r.ref=self.ids[r.ref]
        return erg
#TODO: handle hex floats
#TODO: handle floats
#TODO: handle ancestors cdbase
if __name__=='__main__':
    import arith1
    
    if len(sys.argv)<=2:
        print "Usage: python omxmlreader [--evaluate] input output"
    state=0
    eval=False
    #TODO: use optparse
    for arg in sys.argv[1:]:
        if state==0:
            if arg=="--evaluate":
                eval=True
            else:
                inputf=arg
                state=1
            continue
        if state==1:
            outputf=arg
            continue
        if state==2:
            print "argument ignored:", arg
    from context import Context
    #inputf=sys.argv[1]
    root=readFile(inputf)
    builder=OMFromXMLBuilder()
    context=Context()
    context.addCDImplementation(arith1.implementation)
    doc=builder.build(root)
    if eval:
        doc=context.evaluate(doc)
    output=context.XMLEncodeObject(doc)
    try:
        out=open(outputf,"w")
    except NameError:
        print "no output file"
        sys.exit(1)
    out.write(output)
    out.close()
    #print repr(root.getAttribute("blabla"))