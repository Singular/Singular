import sys
sys.path.insert(0,"/Users/michael/sing3/modules/openmathserver")
import CD.gametheory as gametheory
import CAS.kuchenessen as kuchenessen

kuchenessen.install_kuchenessen_plugin()

from context import Context
from omxmlreader import *
import sys




root=readStream(sys.stdin)
builder=OMFromXMLBuilder()
context=Context()

context.addCDImplementation(gametheory.implementation)
doc=builder.build(root)

doc=context.evaluate(doc)

output=context.XMLEncodeObject(doc)

sys.stdout.write(output)

