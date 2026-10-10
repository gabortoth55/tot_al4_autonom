from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument('discharge_per_s', default_value='2.0',
                              description='Merulesi sebesseg [%/s]'),
        DeclareLaunchArgument('warn_threshold', default_value='30.0',
                              description='WARNING kuszob [%]'),
        DeclareLaunchArgument('critical_threshold', default_value='15.0',
                              description='CRITICAL kuszob [%]'),
        Node(
            package='tot_al4_autonom',
            executable='battery_sim_node',
            name='battery_sim_node',
            output='screen',
            parameters=[{'discharge_per_s': LaunchConfiguration('discharge_per_s')}],
        ),
        Node(
            package='tot_al4_autonom',
            executable='battery_monitor_node',
            name='battery_monitor_node',
            output='screen',
            parameters=[{
                'warn_threshold': LaunchConfiguration('warn_threshold'),
                'critical_threshold': LaunchConfiguration('critical_threshold'),
            }],
        ),
    ])
