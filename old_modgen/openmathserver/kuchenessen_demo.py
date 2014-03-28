# -*- coding: utf-8 -*-
#imports and basic objects
import sys
sys.path.insert(0,"/Users/michael/sing3/modules/openmathserver")
import CD.gametheory as gametheory
from objects import OMApply
from context import Context
from os import pipe
from subprocess import Popen,PIPE
from omxmlreader import *
context=Context()

#our cake
rectangle_list=[(4,3),(3,3),(1,1),(2,4),(4,2)]
print "Eingabeliste", rectangle_list
#create Openmath representation
cake=gametheory.cake_to_OM(rectangle_list)
task=OMApply(gametheory.left_wins_sym,[cake])
task=context.XMLEncodeObject(task)




#open a pipe
p=Popen(["python", "/Users/michael/sing3/modules/openmathserver/openmathhashunger.py"],
    stdout=PIPE,stdin=PIPE)
p.stdin.write(task)
p.stdin.close()

#parse it
root=readStream(p.stdout)
builder=OMFromXMLBuilder()
doc=builder.build(root)

#output
erg=doc.getValue()
if erg:
    print "Spieler 1 gewinnt"
else:
    print "Spieler 1 gewinnt nicht"
p.stdout.close()