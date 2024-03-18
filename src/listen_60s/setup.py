from setuptools import setup
import os
from glob import glob

package_name = 'listen_60s'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        # (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
        # (os.path.join('lib', package_name), glob('listen_60s/iat_ws_python3.py')),
        # (os.path.join('lib', package_name), glob('ulaahead/*.xml')),
        # (os.path.join('lib', package_name, 'moto'), glob('ulaahead/moto/*.xml')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='lmx',
    maintainer_email='lmx@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'content_pub = listen_60s.listen_60s:main',
        ],
    },
)
