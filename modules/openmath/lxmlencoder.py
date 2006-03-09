import lxml.etree as et
from objects import OMV, OMS, OMA, OMBIND


def encodeSub(om, parent):
    #print om
    if isinstance(om, int):
        sub=et.SubElement(parent, "OMI")
        sub.text=str(om)
        return
    if isinstance(om, long):
        sub=et.SubElement(parent, "OMI")
        sub.text=str(om)
        return
    if isinstance(om, OMV):
        sub=et.SubElement(parent, "OMV")
        sub.set("name",om.name)
        return
    if isinstance(om, OMS):
        
        sub=et.SubElement(parent,"OMS")
        sub.set("name",om.name)
        sub.set("cdbase", om.cd.base)
        sub.set("cd",om.cd.name)
        return
    if isinstance(om, OMA):
        top=et.SubElement(parent, "OMA")
        encodeSub(om.func,top)
        for a in om.args:
            encodeSub(a,top)
        return
    if isinstance(om, OMBIND):
        top=et.SubElement(parent, "OMBIND")
        encodeSub(om.binder,top)
        for a in om.variables:
            encodeSub(a,top)
        encodeSub(om.block,top)
        return
    print om
    raise Exception
        
def encodeTop(om):
    if isinstance(om, int):
        sub=et.Element("OMI")
        sub.text=str(om)
        return sub
    if isinstance(om, long):
        sub=et.Element("OMI")
        sub.text=str(om)
        return sub
    if isinstance(om, OMV):
        sub=et.Element("OMV")
        sub.set("name",om.name)
        return sub
    if isinstance(om, OMS):
        sub=et.Element("OMS")
        sub.set("name",om.name)
        sub.set("cdbase", om.cd.base)
        sub.set("cd",om.cd.name)
        return sub
    if isinstance(om, OMA):
        top=et.Element("OMA")
        encodeSub(om.func, top)
        for a in om.args:
            encodeSub(a,top)
        return top
    #print om.__class__
    raise Exception
def encode(om):
    root=encodeTop(om)
    tree = et.ElementTree(root)
    return tree
    