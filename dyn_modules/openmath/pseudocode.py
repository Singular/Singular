content = OMCD("arith1")
content.add("bla", "blupp", "blipper")
OMCD.implement
implementation = OMCDImplementation(content)

implementation.groebner=singular.groebner
#does the default mapping, forgets about attributes

@keep_attributions
def attr_play(a,b):
    attribs=attributes(a)
    #...

implementation.attr=attr_play


def evaluate(object, attributes):
    pass

