
import struct


def convert(iname: str, oname: str = 'output'):
    ofile = open(oname, "w+")
    with open(iname, "w+") as ifile:
        bytes = ifile.read(4)
        while bytes:
            size = int(bytes)
            bytes = ifile.read(4*size)
            data = struct.unpack('i'*size, bytes)
            ofile.write(' '.join(data))
            ofile.write('\n')
            bytes = ifile.read(4)

    ofile.close()


if __name__ == '__main__':
    convert('../bsim/tests/fire.log', 'out.log')
