from pyasn1.type import univ
from pyasn1.codec.ber import encoder, decoder
substrate = encoder.encode(univ.Boolean(True))
substrate="\x02\x01\x01"
substrate="\x09\x05\x80\x82\x66\x00\x00"


print "\n"


value= encoder.encode(univ.Real(2));
print ":".join("{:02x}".format(ord(c)) for c in value)

print value
print decoder.decode(substrate)
value=decoder.decode(substrate)
print value
print "\n"
