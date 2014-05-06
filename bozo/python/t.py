from pyasn1.type import univ, namedtype, tag
class Record(univ.Sequence):
    componentType = namedtype.NamedTypes(
    namedtype.NamedType('id', univ.Integer()),
    namedtype.OptionalNamedType(
        'room',
        univ.Integer().subtype(
            implicitTag=tag.Tag(tag.tagClassContext, tag.tagFormatSimple, 0)
        )
    ),
    namedtype.DefaultedNamedType(
        'house', 
        univ.Integer(0).subtype(
            implicitTag=tag.Tag(tag.tagClassContext, tag.tagFormatSimple, 1)
        )
    )
   )

record = Record()
record.setComponentByName('id', 123)
print record.prettyPrint()
