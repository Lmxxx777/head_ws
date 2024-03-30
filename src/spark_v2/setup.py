from setuptools import setup
import os
from glob import glob

package_name = 'spark_v2'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        # Install marker file in the package index
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        # Include our package.xml file
        (os.path.join('share', package_name), ['package.xml']),
        # Include all launch files.
        (os.path.join('share', package_name, 'launch'), glob(os.path.join('launch', '*launch.py'))),
        # (os.path.join('lib', package_name), glob('listen_60s/iat_ws_python3.py')),
        # (os.path.join('lib', package_name), glob('ulaahead/*.xml')),
        # (os.path.join('lib', package_name, 'moto'), glob('ulaahead/moto/*.xml')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='lmx',
    maintainer_email='2351006771@qq.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
		    'spark_v2 = spark_v2.spark:main',
        ],
    },
)
