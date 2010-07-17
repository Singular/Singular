import lxml.etree as et
from objects import *

class OMFromXMLBuilder:
    def buildFromNode(self, node):
        erg=None
        if (node.tag=="OMI"):
            erg=int(node.text)
        if (node.tag=="OMSTR"):
            #print "data", node.text
            erg=node.text
        if (node.tag=="OMB"):
            erg=OMByteArray(base64.decodestring(node.text))
        if (node.tag=="OMV"):
            name=node.get("name") #node.attributes["name"]
            #print dir(name)
            erg= OMV(name)
        if (node.tag=="OMS"):
            if "cdbase" in node.attrib:
                #FIXME: obtain from ancestors
                cdbase=node.get("cdbase")
            else: 
                cdbase=None
            cdname=node.get("cd")
            name=node.get("name")
            #print repr(node)
            #print node.attributes
            #print "cdbase", cdbase
            if cdbase==None:
                cd=OMCD(cdname)
            else:
                cd=OMCD(cdname,cdbase)
            erg=OMS(name,cd)
        if (node.tag=="OMA"):
            children=[self.buildFromNode(c) for c in node]
            erg= OMA(children[0],children[1:])
        if (node.tag=="OMOBJ"):
            children=[self.buildFromNode(c) for c in node]
            erg= OMOBJ(children)
        if (node.tag=="OMBIND"):
            children=[self.buildFromNode(c) for c in node]
            erg= OMBIND(children[0],children[1:-1],children[-1])
        if (node.tag=="OMF"):
            if (node.hasAttribute("dec")):
                erg=float(node.getAttribute("dec"))
            else:
                raise NotImplementedError
            
        if (node.tag=="OMR"):
            erg=OMRef(node.get("xref"))
            self.refs.append(erg)
        if (node.tag=="OMATP"):
            children=[self.buildFromNode(c) for c in node]
            assert(len(children)==2)
            erg=OMAttributePair(children[0],children[1])
        if (node.tag=="OMATTR"):
            children=[self.buildFromNode(c) for c in node]
            erg=OMAttribution(*children)
        if None==erg:
            raise NotImplementedError
        else:
            if "id" in node.attrib:
                id=node.get("id")
                self.ids[id]=erg
        return erg
    def __init__(self):
        self.refs=[]
        self.ids={}
    def build(self, tree):
        root=tree.getroot()
        #remove_white_space(root)
        erg=self.buildFromNode(root)
        for r in self.refs:
            r.ref=self.ids[r.ref]
        return erg
def decode(tree):
    builder=OMFromXMLBuilder()
    om=builder.build(tree)
    return om