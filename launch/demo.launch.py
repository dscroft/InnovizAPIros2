from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions.path_join_substitution import PathJoinSubstitution
from launch_ros.actions import Node

def generate_launch_description():
    device = Node(
        package="innoviz_ros2",
        executable="device",
        output="screen",
        parameters=[
            {"file_path": PathJoinSubstitution([get_package_share_directory("innoviz_ros2"), "config", "om_config.json"])}
        ]
    )

    return LaunchDescription([
        device
    ])