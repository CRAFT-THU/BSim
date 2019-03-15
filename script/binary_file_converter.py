
import struct

from bsim import pkg_dir


def convert(iname: str, oname: str = 'output'):
    ofile = open(oname, "w+")
    with open(iname, "rb") as ifile:
        bytes = ifile.read(4)
        while bytes:
            size = struct.unpack('i', bytes)[0]
            bytes = ifile.read(4*size)
            data = struct.unpack('i'*size, bytes)
            ofile.write(' '.join(map(str, data)))
            ofile.write('\n')
            bytes = ifile.read(4)

    ofile.close()


if __name__ == '__main__':
    convert(pkg_dir + '/tests/fire.bin.log', 'out.log')
