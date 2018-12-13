
import os


pkg_dir = os.path.dirname(__file__)

if not os.path.exists(os.path.join(pkg_dir, '/code_gen')):
    os.mkdir(os.path.join(pkg_dir, '/code_gen'))

if not os.path.exists(os.path.join(pkg_dir, '/so_gen')):
    os.mkdir(os.path.join(pkg_dir, '/so_gen'))
