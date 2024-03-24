from setuptools import setup
import os
from glob import glob

package_name = 'head'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
        (os.path.join('lib', package_name), glob('head/*.py')),
        (os.path.join('lib', package_name), glob('head/*.xml')),
        (os.path.join('lib', package_name, 'moto'), glob('head/moto/*.xml')),
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
            'facial_features = head.robot_control:main',
            'facial_expression = head.robot_action:main',
            'facial_mouth = head.mouth_talk:main',
        ],
    },
)
